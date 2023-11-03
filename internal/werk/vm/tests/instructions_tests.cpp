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
    ASSERT_EQ(GetShortInstructionFirstOp(b), 5);
    ASSERT_EQ(GetShortInstructionSecondOp(b), 3);
    ASSERT_EQ(GetShortInstructionThirdOp(b), 6);
}

TEST(Instructions, MovOps) {
    uint16_t b = 0xfd58;
    ASSERT_EQ(GetLongInstructionFirstOp(b), 10);
    ASSERT_EQ(GetLongInstructionSecondOp(b), 11);
}
