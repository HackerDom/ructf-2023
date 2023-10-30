#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <mutex>

#include <utils/result.hpp>

#include <vm.hpp>

#include <vd_generator.hpp>

namespace werk::server {
    class Run {
    public:
        Run(vd_t vd, std::shared_ptr<vm::Vm> vm, std::uint64_t ticksLimit);

        virtual ~Run() = default;

        enum State {
            Running,
            Finished,
            Crashed,
            Killed,
            Timeout,
            InternalError
        };

        virtual void Update(int ticksCount);

        void Kill();

        [[nodiscard]] State GetState() const;

        [[nodiscard]] vd_t GetVd() const;

        [[nodiscard]] std::shared_ptr<vm::Vm> GetVm() const;

    private:
        const vd_t vd;
        const std::shared_ptr<vm::Vm> vm;

        std::mutex updateMutex;
        State state;
        std::uint64_t totalTicksCount;
        std::uint64_t ticksLimit;

        void updateStatusInternal(vm::Vm::Status status);
    };
}
