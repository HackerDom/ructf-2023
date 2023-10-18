#pragma once

#include <atomic>
#include <string>
#include <filesystem>
#include <functional>

#include <utils/thread_pool.hpp>

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

        using KillHandlerT = std::function<KillResponse(const KillRequest&)>;
        void SetKillHandler(KillHandlerT handler);

        using StatusHandlerT = std::function<StatusResponse(const StatusRequest &request)>;
        void SetStatusHandler(StatusHandlerT handler);

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
