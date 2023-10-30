#include <fstream>

#include <run.hpp>

namespace werk::server {
    utils::result<std::shared_ptr<Run>> Run::CreateFromFile(
            const std::filesystem::path &binaryPath,
            vd_t vd,
            void *memory,
            std::uint64_t ticksLimit) {
        return utils::result<std::shared_ptr<Run>>::of_error("not implemented");
    }

    Run::Run(vd_t vd, std::shared_ptr<vm::Vm> vm, std::uint64_t ticksLimit)
            : vd(vd), vm(std::move(vm)), ticksLimit(ticksLimit) {
        state = Running;
        totalTicksCount = 0;
    }

    Run::State Run::GetState() const {
        return state;
    }

    void Run::Update(int ticksCount) {
        totalTicksCount += ticksCount;
        if (state == Running) {
            vm->Tick(ticksCount);
        }
    }

    vd_t Run::GetVd() const {
        return vd;
    }
}
