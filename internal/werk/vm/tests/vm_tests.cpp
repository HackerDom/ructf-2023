#include <functional>
#include <cstring>
#include <memory>

#include <gtest/gtest.h>

#include <utils/defer.hpp>

#include <vm.hpp>

using namespace werk::vm;
using namespace werk::utils;

void testVmRunInstruction(
    uint8_t *memory,
    void *instruction,
    std::size_t instructionSize,
    std::size_t loadOffset,
    RegistersSet startState,
    std::function<void(std::shared_ptr<Vm>)> afterExecuteStateChecker) {
    auto vm = std::make_shared<Vm>(memory);

    std::memcpy(memory + loadOffset, instruction, instructionSize);

    std::memcpy(&vm->registers, &startState, sizeof(RegistersSet));

    vm->registers.pc = static_cast<uint16_t>(loadOffset);

    vm->Tick(1);

    afterExecuteStateChecker(vm);
}

TEST(Vm, LoadInstruction) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });
    uint16_t instruction = 0x0300; // load v3

    memory[0x888] = 0xaa;
    memory[0x889] = 0xbb;

    testVmRunInstruction(
        memory,
        &instruction,
        sizeof(instruction),
        0x1337,
        RegistersSet{.i = 0x888},
        [](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[3], 0xbbaa);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    instruction = 0x0000; // load v0

    memory[0x888] = 0xad;
    memory[0x889] = 0xde;

    testVmRunInstruction(
        memory,
        &instruction,
        sizeof(instruction),
        0x1337,
        RegistersSet{.i = 0x888},
        [](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[0], 0xdead);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    instruction = 0x0700; // load v7

    memory[0x888] = 0xfe;
    memory[0x889] = 0xca;

    testVmRunInstruction(
        memory,
        &instruction,
        sizeof(instruction),
        0x1337,
        RegistersSet{.i = 0x888},
        [](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[7], 0xcafe);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    for (std::uint16_t i = kServiceRegionStart; i <= kServiceRegionEnd; ++i) {
        instruction = 0x0700; // load v7

        memory[0x888] = 0xfe;
        memory[0x889] = 0xca;

        testVmRunInstruction(
            memory,
            &instruction,
            sizeof(instruction),
            0x1337,
            RegistersSet{.i = i},
            [](std::shared_ptr<Vm> v){
                ASSERT_EQ(v->GetStatus(), Vm::Status::Crashed);
            }
        );
    }
}

TEST(Vm, StoreInstruction) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });
    uint16_t instruction = 0x0d00; // store v5

    std::memset(memory, 0, kMemorySize);

    testVmRunInstruction(
        memory,
        &instruction,
        sizeof(instruction),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0xdead, 0, 0}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[5], 0xdead);
            ASSERT_EQ(memory[0x1000], 0xad);
            ASSERT_EQ(memory[0x1001], 0xde);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    for (std::uint16_t i = kServiceRegionStart; i <= kServiceRegionEnd; ++i) {
        instruction = 0x0d00; // store v5

        std::memset(memory, 0, kMemorySize);

        testVmRunInstruction(
            memory,
            &instruction,
            sizeof(instruction),
            0x1337,
            RegistersSet{.v = {0, 0, 0, 0, 0, 0xdead, 0, 0}, .i = i},
            [memory](std::shared_ptr<Vm> v){
                ASSERT_EQ(v->registers.v[5], 0xdead);
                ASSERT_EQ(v->GetStatus(), Vm::Status::Crashed);
                ASSERT_EQ(memory[0x1000], 0);
                ASSERT_EQ(memory[0x1001], 0);
            }
        );
    }
}

TEST(Vm, MovInstruction) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });

    uint16_t instructionShort = 0x1008; // mov v0, v1
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0xdead, 0, 0, 0, 0, 0, 0, 0}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[1], 0xdead);
            ASSERT_EQ(v->registers.v[0], 0xdead);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    instructionShort = 0x1390; // mov v7, v2
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0xdead, 0, 0xdead, 0, 0, 0, 0, 0xcafe}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->registers.v[2], 0xcafe);
            ASSERT_EQ(v->registers.v[7], 0xcafe);
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
        }
    );

    instructionShort = 0x11c8; // mov v3, reg9 (invalid)
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0xdead, 0, 0, 0xcafe, 0, 0, 0, 0}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Crashed);
        }
    );

    instructionShort = 0x14a0; // mov $imm, v4 (no size bit set)
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Crashed);
        }
    );

    uint8_t instructionLong[] = {0xa1, 0x14, 0xfe, 0xca}; // mov $imm, v4 | 0x14a1cafe
    testVmRunInstruction(
        memory,
        instructionLong,
        sizeof(instructionLong),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.v[4], 0xcafe);
        }
    );

    instructionShort = 0x13c0; // mov v7, i
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0xdead}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.i, 0xdead);
        }
    );

    instructionShort = 0x17c0; // mov reg15 (invalid), i
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0xdead}, .i = 0x1000},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Crashed);
        }
    );
}

TEST(Vm, PushInstruction) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });

    uint16_t instructionShort = 0x1b00; // push v3
    std::memset(memory, 0, kMemorySize);
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0xdead, 0, 0, 0, 0}, .sp = 0x888},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.sp, 0x88a);
            ASSERT_EQ(memory[0x889], 0xad);
            ASSERT_EQ(memory[0x88a], 0xde);
        }
    );

    instructionShort = 0x1b00; // push v3
    std::memset(memory, 0, kMemorySize);
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0xdead, 0, 0, 0, 0}, .sp = 0xfffe},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.sp, 0x0);
            ASSERT_EQ(memory[0xffff], 0xad);
            ASSERT_EQ(memory[0x0000], 0xde);
        }
    );

    instructionShort = 0x1b00; // push v3
    std::memset(memory, 0, kMemorySize);
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0xdead, 0, 0, 0, 0}, .sp = 0xffff},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.sp, 0x1);
            ASSERT_EQ(memory[0x0000], 0xad);
            ASSERT_EQ(memory[0x0001], 0xde);
        }
    );
}

TEST(Vm, PopInstruction) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });

    uint16_t instructionShort = 0x2400; // push v4
    std::memset(memory, 0, kMemorySize);
    memory[0x88a] = 0xde;
    memory[0x889] = 0xad;
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0}, .sp = 0x88a},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.sp, 0x888);
            ASSERT_EQ(v->registers.v[4], 0xdead);
        }
    );

    instructionShort = 0x2600; // push v6
    std::memset(memory, 0, kMemorySize);
    memory[0x0001] = 0xde;
    memory[0x0000] = 0xad;
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0, 0, 0, 0}, .sp = 0x1},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.sp, 0xffff);
            ASSERT_EQ(v->registers.v[6], 0xdead);
        }
    );
}

TEST(Vm, ArithInstructions) {
    uint8_t *memory = new uint8_t[kMemorySize];
    Defer f([memory]{ delete[] memory; });

    uint16_t instructionShort = 0x3ca4; // mul v4, v5, v1
    testVmRunInstruction(
        memory,
        &instructionShort,
        sizeof(instructionShort),
        0x1337,
        RegistersSet{.v = {0, 0, 0, 0, 0xde, 0x13, 0, 0}},
        [memory](std::shared_ptr<Vm> v){
            ASSERT_EQ(v->GetStatus(), Vm::Status::Running);
            ASSERT_EQ(v->registers.v[1], 0x107a);
            ASSERT_EQ(v->registers.v[4], 0xde);
            ASSERT_EQ(v->registers.v[5], 0x13);
        }
    );
}
