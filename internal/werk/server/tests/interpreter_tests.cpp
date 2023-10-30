#include <gtest/gtest.h>

#include <interpreter.hpp>

using namespace werk::server;
using namespace std::literals::chrono_literals;

std::shared_ptr<Interpreter> GetInterpreter() {
    auto scheduler = std::make_shared<Scheduler>(500, 1000);
    auto pagesPool = std::make_shared<PagesPool>(3);
    auto generator = std::make_shared<VdGenerator>();

    return std::make_shared<Interpreter>(scheduler, pagesPool, generator, 300ms);
}

TEST(Interpreter, RunWhenAllPagesAllocatedShouldFail) {
    auto interpreter = GetInterpreter();

    auto pr1 = interpreter->Run(RunRequest{"", ""});
    ASSERT_TRUE(pr1.success);
    ASSERT_NE(pr1.vd, kEmptyVmDescriptor);

    auto pr2 = interpreter->Run(RunRequest{"", ""});
    ASSERT_TRUE(pr2.success);
    ASSERT_NE(pr2.vd, kEmptyVmDescriptor);

    auto pr3 = interpreter->Run(RunRequest{"", ""});
    ASSERT_TRUE(pr3.success);
    ASSERT_NE(pr3.vd, kEmptyVmDescriptor);

    auto pr4 = interpreter->Run(RunRequest{"", ""});
    ASSERT_FALSE(pr4.success);
    ASSERT_EQ(pr4.vd, kEmptyVmDescriptor);
}
