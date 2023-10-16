#include <random>

#include <scheduler.hpp>

namespace werk::server {
    Scheduler::Scheduler(int minTicks, int maxTicks) :
            randomDevice(),
            randomEngine(randomDevice()),
            uniformDistribution(minTicks, maxTicks) {
    }

    void Scheduler::Append(std::shared_ptr<vm::Vm> vm) {
        std::lock_guard<std::mutex> _(vmsListMutex);

        vms.insert(std::move(vm));
    }

    void Scheduler::Remove(const std::shared_ptr<vm::Vm> &vm) {
        std::lock_guard<std::mutex> _(vmsListMutex);

        vms.erase(vm);
    }

    void Scheduler::TickAll() {
        std::lock_guard<std::mutex> _(vmsListMutex);

        for (auto &it: vms) {
            auto tickCount = uniformDistribution(randomEngine);

            it->Tick(tickCount);
        }
    }
}
