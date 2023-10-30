#include <gtest/gtest.h>

#include <run.hpp>

using namespace werk::server;
using namespace werk::vm;

namespace {
    class TestVm : public Vm {
    public:
        explicit TestVm(void *m) : Vm(m) {
        }

        Vm::Status Tick(int opsCount) override {
            totalTicks += opsCount;
            return status;
        }

        Vm::Status status = Running;
        int totalTicks = 0;
    };
}

std::shared_ptr<Vm> GetVm() {
    auto vm = std::make_shared<TestVm>(reinterpret_cast<void *>(0xcafebabedeadbeef));

    return std::static_pointer_cast<Vm>(vm);
}

TEST(Run, StateRunningAfterCreate) {
    auto vm = GetVm();
    auto run = werk::server::Run(0, vm, 10000);

    ASSERT_EQ(run.GetState(), Run::State::Running);
}

TEST(Run, StateTimeoutAfterTooManyTicks) {
    auto vm = GetVm();
    auto run = werk::server::Run(0, vm, 10000);

    for (int i = 0; i < 100; ++i) {
        run.Update(100);
    }

    ASSERT_EQ(run.GetState(), Run::State::Running);

    run.Update(1);

    ASSERT_EQ(run.GetState(), Run::State::Timeout);
}

TEST(Run, StateIsCrashedAfterVmReturnsCrashed) {
    auto vm = GetVm();
    auto run = werk::server::Run(0, vm, 10000);

    for (int i = 0; i < 50; ++i) {
        run.Update(100);
    }

    ASSERT_EQ(run.GetState(), Run::State::Running);

    std::dynamic_pointer_cast<TestVm>(vm)->status = werk::vm::Vm::Crashed;

    ASSERT_EQ(run.GetState(), Run::State::Running);

    run.Update(100);

    ASSERT_EQ(run.GetState(), Run::State::Crashed);
}

TEST(Run, StateIsFinishedAfterVmReturnsFinished) {
    auto vm = GetVm();
    auto run = werk::server::Run(0, vm, 10000);

    for (int i = 0; i < 50; ++i) {
        run.Update(100);
    }

    ASSERT_EQ(run.GetState(), Run::State::Running);

    std::dynamic_pointer_cast<TestVm>(vm)->status = werk::vm::Vm::Finished;

    ASSERT_EQ(run.GetState(), Run::State::Running);

    run.Update(100);

    ASSERT_EQ(run.GetState(), Run::State::Finished);
}

TEST(Run, StateIsKilledAfterKill) {
    auto vm = GetVm();
    auto vmt = std::dynamic_pointer_cast<TestVm>(vm);
    auto run = werk::server::Run(0, vm, 10000);

    for (int i = 0; i < 50; ++i) {
        run.Update(100);
    }

    ASSERT_EQ(vmt->totalTicks, 50 * 100);

    run.Kill();

    ASSERT_EQ(run.GetState(), Run::State::Killed);

    for (int i = 0; i < 50; ++i) {
        run.Update(100);
    }

    ASSERT_EQ(run.GetState(), Run::State::Killed);
    ASSERT_EQ(vmt->totalTicks, 50 * 100); //no Tick() was performed
}