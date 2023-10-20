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

    static ssize_t recvAll(int fd, void *buf, std::size_t n) {
        auto nrecv = recv(fd, buf, n, MSG_WAITALL);
        if (nrecv == 0) {
            LOG(WARNING) << "client closed the connection unexpectedly";
            return 0;
        } else if (nrecv < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG(WARNING) << "command reading timeout: probably hanging client?";
            }
            LOG(ERROR) << utils::PError("recv");
            return -1;
        } else if (static_cast<std::size_t>(nrecv) != n) {
            LOG(WARNING) << "not full read, probably hanging client?";
            LOG(WARNING) << "perror, just in case: " << utils::PError("recv");
            return -1;
        }

        return nrecv;
    }

    static ssize_t sendAll(int fd, const void *buf, std::size_t n) {
        auto nsend = send(fd, buf, n, 0);
        if (nsend == 0) {
            LOG(WARNING) << "client close connection unexpectedly";
            return 0;
        } else if (nsend < 0) {
            LOG(ERROR) << utils::PError("send");
            return nsend;
        } else if (static_cast<std::size_t>(nsend) != n) {
            LOG(WARNING) << "not full send, probably hanging client?";
            return 1;
        }

        return nsend;
    }

    void Server::handleClient(int fd) {
        utils::Defer closer([fd] {
            fsync(fd);
            close(fd);
        });

        timeval timeout{};
        timeout.tv_sec = 0;
        timeout.tv_usec = 300 * 1000; // 300 ms

        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            LOG(ERROR) << utils::PError("setsockopt(SO_RVBTIMEO) for client fd");
            return;
        }

        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            LOG(ERROR) << utils::PError("setsockopt(SO_SNDTIMEO) for client fd");
            return;
        }

        auto acceptCommands = true;
        while (acceptCommands) {
            int8_t command;
            auto nrecv = recvAll(fd, &command, sizeof(command));
            if (nrecv == 0) {
                LOG(INFO) << "client closed connection";
                break;
            } else if (nrecv < 0) {
                LOG(WARNING) << "reading command failed, closing connection";
                break;
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
                case 'Q':
                    acceptCommands = false;
                    break;
                default:
                    LOG(WARNING) << "unknown command from client";
                    writeInvalidRequest(fd);
                    acceptCommands = false;
                    break;
            }
        }
    }

    void Server::SetRunHandler(Server::RunHandlerT handler) {
        runHandler = std::move(handler);
    }

    void Server::SetKillHandler(Server::KillHandlerT handler) {
        killHandler = std::move(handler);
    }

    void Server::SetStatusHandler(Server::StatusHandlerT handler) {
        statusHandler = std::move(handler);
    }

    void Server::handleRunRequest(int fd) {
        if (!runHandler) {
            writeInvalidRequest(fd);
            return;
        }

        uint8_t binaryPathLen;
        if (recvAll(fd, &binaryPathLen, sizeof(binaryPathLen)) <= 0) {
            LOG(WARNING) << "reading binary path len failed, closing connection";
            return;
        }

        uint8_t serialOutPathLen;
        if (recvAll(fd, &serialOutPathLen, sizeof(serialOutPathLen)) <= 0) {
            LOG(WARNING) << "reading serial out path len failed, closing connection";
            return;
        }

        std::string binaryPath;
        binaryPath.resize(binaryPathLen);
        if (recvAll(fd, binaryPath.data(), binaryPath.size()) <= 0) {
            LOG(WARNING) << "reading binary path failed, closing connection";
            return;
        }

        std::string serialOutPath;
        serialOutPath.resize(serialOutPathLen);
        if (recvAll(fd, serialOutPath.data(), serialOutPath.size()) <= 0) {
            LOG(WARNING) << "reading serial out path failed, closing connection";
            return;
        }

        auto request = RunRequest{
                std::filesystem::path(binaryPath),
                std::filesystem::path(serialOutPath)
        };

        LOG(INFO) << "request = " << request.String();

        RunResponse response;

        try {
            response = runHandler(request);
        } catch (std::exception &e) {
            LOG(ERROR) << "run handler failed with exception: " << e.what();
            return;
        } catch (...) {
            LOG(ERROR) << "run handler failed with exception";
            return;
        }

        LOG(INFO) << "response = " << response.String();

        if (sendAll(fd, &response.success, sizeof(response.success)) < 0) {
            LOG(WARNING) << "sending success value failed, closing connection";
            return;
        }

        if (response.success) {
            if (sendAll(fd, &response.vd, sizeof(response.vd)) < 0) {
                LOG(WARNING) << "sending vm descriptor value failed, closing connection";
                return;
            }
        } else {
            uint16_t len = response.errorMessage.size();
            if (sendAll(fd, &len, sizeof(len)) < 0) {
                LOG(WARNING) << "sending error message len failed, closing connection";
                return;
            }

            if (sendAll(fd, response.errorMessage.data(), response.errorMessage.size()) <= 0) {
                LOG(WARNING) << "sending error message failed, closing connection";
                return;
            }
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

    void Server::writeInvalidRequest(int fd) {
        sendAll(fd, "INVALID", sizeof("INVALID"));
    }
}
