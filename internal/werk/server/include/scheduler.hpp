#pragma once

#include <memory>
#include <unordered_set>
#include <thread>
#include <vector>
#include <random>
#include <mutex>

#include <run.hpp>

namespace werk::server {
    class Scheduler {
    public:
        Scheduler(int minTicks, int maxTicks);

        void Append(std::shared_ptr<Run> run);

        void Remove(const std::shared_ptr<Run> &run);

        void UpdateAll();

    private:
        std::unordered_set<std::shared_ptr<Run>> runs;
        std::mutex runsMutex;

        std::random_device randomDevice;
        std::mt19937 randomEngine;
        std::uniform_int_distribution<int> uniformDistribution;
    };
}
