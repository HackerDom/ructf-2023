#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <glog/logging.h>

#include <utils/defer.hpp>
#include <utils/strings.hpp>

#include <server.hpp>

namespace werk::server {
    Server::Server(std::shared_ptr<utils::ThreadPool> threadPool,
                   std::filesystem::path socketPath) : threadPool(std::move(threadPool)),
                                                       socketPath(std::move(socketPath)) {
        // empty
    }

    Server::ListenResult Server::Listen() {
        if (!listening.exchange(true)) {
            return listenInternal();
        }

        return {false, "server already listening"};
    }

    void Server::StopListen() {
        listening = false;
    }

    Server::ListenResult Server::listenInternal() {
        // after this method, listening value should always be false
        utils::Defer dropListeningFlag([this] { this->listening = false; });

        if (socketPath.string().size() > sizeof(sockaddr_un::sun_path)) {
            return {false, utils::Format("too long socket path name (>%d)", sizeof(sockaddr_un::sun_path))};
        }

        if (std::filesystem::is_socket(socketPath)) {
            LOG(WARNING) << socketPath << " already exists, deleting";
            std::error_code errorCode;
            std::filesystem::remove(socketPath, errorCode);
            if (errorCode) {
                LOG(ERROR) << "remove of socket path failed: " << errorCode.message();
            }
        }

        auto sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockFd < 0) {
            return {false, utils::PError("socket")};
        }
        utils::Defer socketCloser(close, sockFd);

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strcpy(addr.sun_path, socketPath.c_str());

        if (bind(sockFd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) < 0) {
            return {false, utils::PError("bind")};
        }

        if (listen(sockFd, 1024) < 0) {
            return {false, utils::PError("listen")};
        }

        pollfd pollFds[1];
        std::memset(pollFds, 0, sizeof(pollfd));
        pollFds[0].fd = sockFd;
        pollFds[0].events = POLLIN;

        LOG(INFO) << "start listening on " << socketPath << "...";

        while (listening) {
            auto ready = poll(pollFds, 1, 500 /*ms*/);
            if (ready < 0) {
                if (errno == EINTR) {
                    return {true, ""};
                }
                return {false, utils::PError("poll")};
            }
            if (ready > 0) {
                if (pollFds[0].revents & POLLIN) {
                    auto clientFd = accept(sockFd, nullptr, nullptr);
                    if (clientFd < 0) {
                        return {false, utils::PError("accept")};
                    }

                    threadPool->EnqueueUserTask([this, clientFd] { this->handleClient(clientFd); });
                } else if (pollFds[0].revents & POLLERR) {
                    LOG(WARNING) << "POLERR in socket fd";
                    break;
                }
            }
        }

        LOG(INFO) << "finish listening on " << socketPath;

        return {true, ""};
    }

    void Server::handleClient(int fd) {
        utils::Defer closer([fd] {
            fsync(fd);
            close(fd);
        });

        timeval timeout{};
        timeout.tv_sec = 0;
        timeout.tv_usec = 500 * 1000; // 500 ms

        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            LOG(ERROR) << utils::PError("setsockopt(SO_RVBTIMEO) for cliend fd");
            return;
        }

        char command;
        auto nrecv = recv(fd, &command, sizeof(command), MSG_WAITALL);
        if (nrecv == 0) {
            LOG(WARNING) << "client closed connection unexpectedly";
            return;
        } else if (nrecv < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG(WARNING) << "command reading timeout: probably hanging client?";
            }
            LOG(ERROR) << utils::PError("recv");
            return;
        } else if (nrecv != sizeof(command)) {
            LOG(WARNING) << "not full read, probably hanging client?";
            LOG(WARNING) << "perror, just in case: " << utils::PError("recv");
            return;
        }

        switch (command) {
            case 'R':
                handleRunRequest(fd);
                break;
            case 'S':
                handleStatusRequest(fd);
                break;
            case 'K':
                handleKillRequest(fd);
                break;
            default:
                LOG(WARNING) << "unknown command from client";
                send(fd, "NOCOMMND", 8, 0);
                break;
        }
    }

    void Server::SetRunHandler(Server::RunHandlerT handler) {
        runHandler = std::move(handler);
    }

    void Server::SetKillHandler(Server::KillHandlerT handler) {
        killHandler = std::move(handler);
    }

    void Server::SetStatusRequestHandler(Server::StatusHandlerT handler) {
        statusHandler = std::move(handler);
    }

    void Server::handleRunRequest(int fd) {
        if (!runHandler) {
            write(fd, "not implemented", 15);
            return;
        }
    }

    void Server::handleStatusRequest(int fd) {
        if (!statusHandler) {
            write(fd, "not implemented", 15);
            return;
        }
    }

    void Server::handleKillRequest(int fd) {
        if (!killHandler) {
            write(fd, "not implemented", 15);
            return;
        }
    }
}
