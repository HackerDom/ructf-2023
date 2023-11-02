#pragma once

#include <string_view>

namespace werk::vm {
    enum OperationNum {
        Load = 0,
        Store = 1,
        Mov = 2,
        Push = 3,
        Pop = 4,
        Add = 5,
        Sub = 6,
        Mul = 7,
        And = 8,
        Or = 9,
        Xor = 10,
        Shl = 11,
        Shr = 12,
        Call = 13,
        Nop = 14,
        Jmp = 15,
        Jl = 16,
        Jg = 17,
        Jle = 18,
        Jge = 19,
        Je = 20,
        Jne = 21,
        Hlt = 22,
        Pushb = 23,
        Popb = 24,
        Cmp = 25,
        Out = 26,
        Hexout = 27,
        Hexouta = 28,
        Poweroff = 29,
    };

    struct Instruction {
        OperationNum Num;
    };

    class InstructionSet {
    public:
        [[nodiscard]] virtual const Instruction &GetInstructionByName(std::string_view name) const = 0;
        [[nodiscard]] virtual const Instruction &GetInstructionByOpcode(int opcode) const = 0;
    };
}
