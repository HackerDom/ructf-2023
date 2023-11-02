#include <gtest/gtest.h>

#include <arch/instructions.hpp>

using namespace werk::vm;

TEST(Instructions, GetOpcode) {
    ASSERT_EQ(GetOpcode(static_cast<uint16_t>(~kOpcodeNumMask)), 0);
    ASSERT_EQ(GetOpcode(static_cast<uint16_t>(0xafff)), 21);
    ASSERT_EQ(GetOpcode(static_cast<uint16_t>(0xe7ff)), 28);
    ASSERT_EQ(GetOpcode(static_cast<uint16_t>(0x67ff)), 12);
    ASSERT_EQ(GetOpcode(static_cast<uint16_t>(0xffff)), 31);
}

TEST(Instructions, IsExtendedInstruction) {
    ASSERT_EQ(IsExtendedInstruction(static_cast<uint16_t>(0xfafe)), false);
    ASSERT_EQ(IsExtendedInstruction(static_cast<uint16_t>(0x8101)), true);
}

TEST(Instructions, NonMovOps) {
    uint16_t b = 0xfd78;
    ASSERT_EQ(GetNonMovFirstOp(b), 5);
    ASSERT_EQ(GetNonMovSecondOp(b), 3);
    ASSERT_EQ(GetNonMovThirdOp(b), 6);
}

TEST(Instructions, MovOps) {
    uint16_t b = 0xfd58;
    ASSERT_EQ(GetMovFirstOp(b), 10);
    ASSERT_EQ(GetMovSecondOp(b), 11);
}
