#pragma once

#include <atomic>
#include <string>
#include <filesystem>
#include <functional>

#include <glog/logging.h>

#include <utils/thread_pool.hpp>
#include <utils/strings.hpp>

#include <models.hpp>

namespace werk::server {
    class Server {
    public:
        Server(
                std::shared_ptr<utils::ThreadPool> threadPool,
                std::filesystem::path socketPath
        );

        struct ListenResult {
            bool success;
            std::string errorMessage;
        };

        ListenResult Listen();

        void StopListen();

        using RunHandlerT = std::function<RunResponse(const RunRequest&)>;
        void SetRunHandler(RunHandlerT handler);

    private:
        std::shared_ptr<utils::ThreadPool> threadPool;
        const std::filesystem::path socketPath;

        std::atomic_bool listening;

        ListenResult listenInternal();

        void handleClient(int fd);

        RunHandlerT runHandler;

        static void writeInvalidRequest(int fd);

        template <class RequestT, class ResponseT, class HandlerT>
        bool executeHandler(HandlerT handler, int fd) {
            if (!handler) {
                LOG(ERROR) << "run request handler not set";
                writeInvalidRequest(fd);
                return false;
            }

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

            if (response.WriteToSocket(fd) != 0) {
                LOG(WARNING) << "writing run request failed, closing connection";
                return false;
            }

            return true;
        }
    };
}
