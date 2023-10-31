#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <glog/logging.h>

#include <utils/defer.hpp>
#include <utils/strings.hpp>

#include <server.hpp>

namespace werk::server {
    template <class RequestT, class ResponseT, class HandlerT>
    bool executeHandler(const HandlerT &handler, int fd) {
        auto request = RequestT::ReadFromSocket(fd);
        if (!request) {
            LOG(WARNING) << utils::Format("reading run request failed: '%s', closing connection",
                                            request.message.c_str());
            return false;
        }

        LOG(INFO) << "request = " << request.value->String();

        ResponseT response;

        try {
            response = handler(*request.value);
        } catch (std::exception &e) {
            LOG(ERROR) << "run handler failed with exception: " << e.what();
            return false;
        } catch (...) {
            LOG(ERROR) << "run handler failed with exception";
            return false;
        }

        LOG(INFO) << "response = " << response.String();

        auto writeResult = response.WriteToSocket(fd);

        if (!writeResult) {
            LOG(WARNING) << "writing run request failed: '" << writeResult.message << "', closing connection";
            return false;
        }

        return true;
    }

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

                    LOG(INFO) << "task put to thread pool, current queue size = " << threadPool->GetQueueSize();
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
                    acceptCommands = executeHandler<RunRequest, RunResponse, RunHandlerT>(runHandler, fd);
                    break;
                case 'K':
                    acceptCommands = executeHandler<KillRequest, KillResponse, KillHandlerT>(killHandler, fd);
                    break;
                case 'S':
                    acceptCommands = executeHandler<StatusRequest, StatusResponse, StatusHandlerT>(statusHandler, fd);
                    break;
                case 'D':
                    acceptCommands = executeHandler<DeleteRequest, DeleteResponse, DeleteHandlerT>(deleteHandler, fd);
                    break;
                case 'O':
                    acceptCommands = executeHandler<GetSerialRequest, GetSerialResponse, GetSerialHandlerT>(getSerialHandler, fd);
                    break;
                case 'Q':
                    acceptCommands = false;
                    break;
                default:
                    LOG(WARNING) << "unknown command from client " << command;
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

    void Server::SetDeleteHandler(Server::DeleteHandlerT handler) {
        deleteHandler = std::move(handler);
    }

    void Server::SetGetSerialHandler(Server::GetSerialHandlerT handler) {
        getSerialHandler = std::move(handler);
    }

    void Server::writeInvalidRequest(int fd) {
        sendAll(fd, "INVALID", sizeof("INVALID"));
    }
}
