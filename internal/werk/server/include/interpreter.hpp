#pragma once

#include <atomic>
#include <thread>
#include <unordered_map>
#include <chrono>

#include <pages_pool.hpp>
#include <scheduler.hpp>
#include <models.hpp>

namespace werk::server {
    class Interpreter {
    public:
        Interpreter(
                std::shared_ptr<Scheduler> scheduler,
                std::shared_ptr<PagesPool> pagesPool,
                std::chrono::milliseconds sleepPeriodMs);

        ~Interpreter();

        bool StartExecutorThread();

        RunResponse Run(const RunRequest &request);

        StatusResponse Status(const StatusRequest &request);

        KillResponse Kill(const KillRequest &request);

    private:
        std::shared_ptr<Scheduler> scheduler;
        std::shared_ptr<PagesPool> pagesPool;

        std::unordered_map<vd_t, std::shared_ptr<vm::Vm>> vdToVm;
        std::mutex vdMapMutex;

        std::atomic<vd_t> currentDescriptor;

        vd_t generateDescriptor();

        const std::chrono::milliseconds sleepPeriod;

        void executorThreadTask();

        std::shared_ptr<std::thread> executorThread;
        std::atomic_bool executorThreadStop;
    };
}
