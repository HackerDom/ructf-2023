#include <random>

#include <scheduler.hpp>

namespace werk::server {
    Scheduler::Scheduler(int minTicks, int maxTicks) :
            randomDevice(),
            randomEngine(randomDevice()),
            uniformDistribution(minTicks, maxTicks) {
    }

    void Scheduler::Append(std::shared_ptr<Run> vm) {
        std::lock_guard<std::mutex> _(runsMutex);

        runs.insert(std::move(vm));
    }

    void Scheduler::Remove(const std::shared_ptr<Run> &run) {
        std::lock_guard<std::mutex> _(runsMutex);

        runs.erase(run);
    }

    void Scheduler::UpdateAll() {
        std::lock_guard<std::mutex> _(runsMutex);

        for (auto &it: runs) {
            auto tickCount = uniformDistribution(randomEngine);

            it->Update(tickCount);
        }
    }
}
