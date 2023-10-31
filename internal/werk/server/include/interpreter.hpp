#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_map>

#include <scheduler.hpp>
#include <run_loader.hpp>
#include <models.hpp>

namespace werk::server {
    class Interpreter {
    public:
        Interpreter(
                std::shared_ptr<RunLoader> runLoader,
                std::shared_ptr<Scheduler> scheduler,
                std::chrono::milliseconds sleepPeriodMs);

        ~Interpreter();

        [[nodiscard]] bool HasVms() const;

        [[nodiscard]] RunResponse Run(const RunRequest &rq);

        [[nodiscard]] KillResponse Kill(const KillRequest &rq);

        [[nodiscard]] StatusResponse Status(const StatusRequest &rq);

        [[nodiscard]] DeleteResponse Delete(const DeleteRequest &rq);

        [[nodiscard]] GetSerialResponse GetSerial(const GetSerialRequest &rq);

    private:
        const std::shared_ptr<RunLoader> runLoader;
        const std::shared_ptr<Scheduler> scheduler;
        const std::chrono::milliseconds sleepPeriod;

        std::unordered_map<vd_t, std::shared_ptr<werk::server::Run>> vdToRun;
        mutable std::mutex vdMapMutex;

        void executorThreadTask();

        std::shared_ptr<std::thread> executorThread;
        std::atomic_bool executorThreadStop;
    };
}
