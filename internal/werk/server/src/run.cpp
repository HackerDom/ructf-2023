#include <fstream>

#include <utils/strings.hpp>

#include <run.hpp>

namespace werk::server {
    Run::Run(vd_t vd, std::shared_ptr<vm::Vm> vm, std::uint64_t ticksLimit)
            : vd(vd), vm(std::move(vm)), ticksLimit(ticksLimit) {
        state = Running;
    }

    Run::State Run::GetState() const {
        return state;
    }

    void Run::Update(int ticksCount) {
        std::lock_guard<std::mutex> _(updateMutex);

        if (state == Running) {
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

        if (vm->GetTotalTicksCount() > ticksLimit) {
            state = State::Timeout;
            return;
        }
    }

    std::shared_ptr<vm::Vm> Run::GetVm() const {
        return vm;
    }

    std::uint64_t Run::GetTotalTicks() const {
        std::lock_guard<std::mutex> _(updateMutex);

        return vm->GetTotalTicksCount();
    }

    std::string Run::GetSerial() const {
        std::lock_guard<std::mutex> _(updateMutex);

        auto &serial = vm->GetSerial();

        return utils::Join(serial.begin(), serial.end(), "");
    }
}
