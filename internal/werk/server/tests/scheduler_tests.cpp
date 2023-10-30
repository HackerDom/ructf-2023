#include <gtest/gtest.h>

#include <scheduler.hpp>

using namespace werk::server;
using namespace werk::vm;

constexpr int kMinTicks = 15;
constexpr int kMaxTicks = 30;

class TestRun : public Run {
public:
    explicit TestRun(void *m) : Run(0, std::make_shared<Vm>(m), 100000000) {
    }

    void Update(int opsCount) override {
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

std::shared_ptr<Run> GetRun() {
    auto tv = std::make_shared<TestRun>(reinterpret_cast<void*>(0xdeadbeefcafebabe));

    return std::static_pointer_cast<Run>(tv);
}

TEST(Scheduler, OneVm) {
    auto scheduler = GetScheduler();
    auto vm1 = GetRun();

    scheduler->Append(vm1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 2);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 3);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 3);
}

TEST(Scheduler, OneVmAppendedTwice) {
    auto scheduler = GetScheduler();
    auto vm1 = GetRun();

    scheduler->Append(vm1);
    scheduler->Append(vm1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 1);
}


TEST(Scheduler, OneRemovedVm) {
    auto scheduler = GetScheduler();
    auto vm1 = GetRun();

    scheduler->Append(vm1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 1);

    scheduler->Remove(vm1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 1);
}

TEST(Scheduler, TwoVms) {
    auto scheduler = GetScheduler();
    auto vm1 = GetRun();
    auto vm2 = GetRun();

    scheduler->Append(vm1);
    scheduler->Append(vm2);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 1);
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm2)->totalCall, 1);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm2)->totalTicks, kMinTicks * 1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 2);
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm2)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm2)->totalTicks, kMinTicks * 2);

    scheduler->Remove(vm1);

    scheduler->UpdateAll();
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm1)->totalCall, 2);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm1)->totalTicks, kMinTicks * 2);
    ASSERT_EQ(std::dynamic_pointer_cast<TestRun>(vm2)->totalCall, 3);
    ASSERT_GE(std::dynamic_pointer_cast<TestRun>(vm2)->totalTicks, kMinTicks * 3);
}
