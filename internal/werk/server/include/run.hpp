#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
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
        static utils::result<std::shared_ptr<Run>> CreateFromFile(
                const std::filesystem::path &binaryPath,
                vd_t vd,
                void *memory,
                std::uint64_t ticksLimit
        );

        enum State {
            Running,
            Paused,
            Finished,
            Crashed,
            Killed,
            Timeout
        };

        void Update(int ticksCount);

        [[nodiscard]] State GetState() const;

    private:
        Run(vd_t vd, std::shared_ptr<vm::Vm> vm, std::uint64_t ticksLimit);

        const vd_t vd;
        const std::shared_ptr<vm::Vm> vm;

        std::mutex updateMutex;
        State state;
        std::uint64_t totalTicksCount;
        std::uint64_t ticksLimit;
    };
}
