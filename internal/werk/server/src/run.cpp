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
        if (state == Running) {
            totalTicksCount += ticksCount;
            updateStatusInternal(vm->Tick(ticksCount));
        }
    }

    vd_t Run::GetVd() const {
        return vd;
    }

    void Run::Kill() {
        state = State::Killed;
    }

    void Run::updateStatusInternal(vm::Vm::Status status) {
        switch (status) {
            case vm::Vm::Running:
                state = State::Running;
                break;
            case vm::Vm::Crashed:
                state = State::Crashed;
                break;
            case vm::Vm::Finished:
                state = State::Finished;
                break;
            default:
                state = State::InternalError;
                break;
        }

        if (totalTicksCount > ticksLimit) {
            state = State::Timeout;
            return;
        }
    }
}
