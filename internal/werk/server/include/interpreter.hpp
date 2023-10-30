#pragma once

#include <atomic>
#include <chrono>
#include <mutex>

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

        [[nodiscard]] RunResponse Run(const RunRequest &rq);

    private:
        const std::shared_ptr<RunLoader> runLoader;
        const std::shared_ptr<Scheduler> scheduler;
        const std::chrono::milliseconds sleepPeriod;

        void executorThreadTask();

        std::shared_ptr<std::thread> executorThread;
        std::atomic_bool executorThreadStop;
    };
}
