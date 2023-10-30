#pragma once

#include <memory>
#include <unordered_set>
#include <thread>
#include <vector>
#include <random>
#include <mutex>

#include <vm.hpp>

namespace werk::server {
    class Scheduler {
    public:
        Scheduler(int minTicks, int maxTicks);

        void Append(std::shared_ptr<vm::Vm> vm);

        void Remove(const std::shared_ptr<vm::Vm> &vm);

        void TickAll();

    private:
        std::unordered_set<std::shared_ptr<vm::Vm>> vms;
        std::mutex vmsListMutex;

        std::random_device randomDevice;
        std::mt19937 randomEngine;
        std::uniform_int_distribution<int> uniformDistribution;
    };
}
