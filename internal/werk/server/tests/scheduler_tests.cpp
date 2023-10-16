#include <gtest/gtest.h>

#include <scheduler.hpp>

using namespace werk::server;
using namespace werk::vm;

constexpr int kMinTicks = 15;
constexpr int kMaxTicks = 30;

class TestVm : public Vm {
public:
    explicit TestVm(void *m) : Vm(m) {
    }

    void Tick(int opsCount) override {
        ASSERT_GE(opsCount, kMinTicks);
        ASSERT_LE(opsCount, kMaxTicks);

        ++totalCall;
        totalTicks += opsCount;
    }

    int totalTicks = 0;
    int totalCall = 0;
};

std::shared_ptr<Scheduler> GetScheduler() {
    return std::make_shared<Scheduler>(kMinTicks, kMaxTicks);
}

std::shared_ptr<Vm> GetVm() {
    auto tv = std::make_shared<TestVm>(reinterpret_cast<void*>(0xdeadbeefcafebabe));

    return std::static_pointer_cast<Vm>(tv);
}

TEST(Scheduler, OneVm) {
    auto scheduler = GetScheduler();
    auto vm1 = GetVm();

    scheduler->Append(vm1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 2);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 3);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 3);
}

TEST(Scheduler, OneVmAppendedTwice) {
    auto scheduler = GetScheduler();
    auto vm1 = GetVm();

    scheduler->Append(vm1);
    scheduler->Append(vm1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 1);
}


TEST(Scheduler, OneRemovedVm) {
    auto scheduler = GetScheduler();
    auto vm1 = GetVm();

    scheduler->Append(vm1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 1);

    scheduler->Remove(vm1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 1);
}

TEST(Scheduler, TwoVms) {
    auto scheduler = GetScheduler();
    auto vm1 = GetVm();
    auto vm2 = GetVm();

    scheduler->Append(vm1);
    scheduler->Append(vm2);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 1);
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm2)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm2)->totalTicks, kMinTicks * 1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 2);
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm2)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm2)->totalTicks, kMinTicks * 2);

    scheduler->Remove(vm1);

    scheduler->TickAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm1)->totalTicks, kMinTicks * 2);
    ASSERT_EQ(std::dynamic_pointer_cast<TestVm>(vm2)->totalCall, 3);
    ASSERT_GE(std::dynamic_pointer_cast<TestVm>(vm2)->totalTicks, kMinTicks * 3);
}
