#pragma once

#include <atomic>
#include <string>
#include <filesystem>
#include <functional>

#include <utils/thread_pool.hpp>

#include <interpreter.hpp>

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

        struct RunRequest {
            std::filesystem::path binaryFilePath;
        };

        struct RunResponse {
            enum Status {
                OK = 0,
                FAIL = 1,
            };
            Status status;
            std::string errorMessage;
        };

        using RunHandlerT = std::function<RunResponse(const RunRequest&)>;

        void SetRunHandler(RunHandlerT handler);

        struct KillRequest {
            vd_t vd;
        };

        struct KillResponse {
            bool success;
        };

        using KillHandlerT = std::function<KillResponse(const KillRequest&)>;

        void SetKillHandler(KillHandlerT handler);

        struct StatusRequest {
            vd_t vd;
        };

        struct StatusResponse {
            enum Status {
                RUNNING = 0,
                EXECUTION_TIMEOUT = 1,
                NOT_FOUND = 2,
                KILLED = 3,
            };
            Status status;
        };

        using StatusHandlerT = std::function<StatusResponse(const StatusRequest &request)>;

        void SetStatusRequestHandler(StatusHandlerT handler);

    private:
        std::shared_ptr<utils::ThreadPool> threadPool;
        const std::filesystem::path socketPath;

        std::atomic_bool listening;

        ListenResult listenInternal();

        void handleClient(int fd);
        void handleRunRequest(int fd);
        void handleKillRequest(int fd);
        void handleStatusRequest(int fd);

        RunHandlerT runHandler;
        KillHandlerT killHandler;
        StatusHandlerT statusHandler;
    };
}
