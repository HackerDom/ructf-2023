#pragma once

#include <atomic>
#include <thread>
#include <unordered_map>

#include <pages_pool.hpp>
#include <scheduler.hpp>

namespace werk::server {
    class Interpreter {
    public:
        Interpreter(
                std::shared_ptr<Scheduler> scheduler,
                std::shared_ptr<PagesPool> pagesPool);

        ~Interpreter();

        vd_t Create(uint8_t *code, std::size_t codeSize);

        bool StartExecutorThread();

    private:
        std::shared_ptr<Scheduler> scheduler;
        std::shared_ptr<PagesPool> pagesPool;

        std::unordered_map<vd_t, std::shared_ptr<vm::Vm>> vdToVm;
        std::mutex vdMapMutex;

        std::atomic<vd_t> currentDescriptor;
        vd_t generateDescriptor();

        std::shared_ptr<std::thread> executorThread;
        std::atomic<bool> executorThreadStop;

        void executorThreadTask();
    };
}
