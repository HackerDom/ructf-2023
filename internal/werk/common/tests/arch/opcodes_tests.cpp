#include <memory>

#include <gtest/gtest.h>

#include <arch/opcodes.hpp>

using namespace werk::arch::opcodes;

std::shared_ptr<OpcodesManager> OpsManager() {
    return std::make_shared<OpcodesManager>();
}

TEST(OpcodesManager, IsValidOpcodeName) {
    auto man = OpsManager();

    ASSERT_FALSE(man->IsValidOpcodeName(""));
    ASSERT_FALSE(man->IsValidOpcodeName(" mov"));

    ASSERT_TRUE(man->IsValidOpcodeName("mov"));
    ASSERT_TRUE(man->IsValidOpcodeName("moV"));
    ASSERT_TRUE(man->IsValidOpcodeName("mOv"));
    ASSERT_TRUE(man->IsValidOpcodeName("Mov"));
    ASSERT_TRUE(man->IsValidOpcodeName("MOV"));
}
