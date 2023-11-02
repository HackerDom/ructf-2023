#pragma once

#include <string_view>

namespace werk::vm {
    // !!! do not change opcode numbers !!!
    enum Opcode {
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
        Ret = 29,
        Poweroff = 30,
    };

    constexpr uint16_t kOpcodeNumMask      = 0xF800; // 0b1111_1000_0000_0000
    constexpr uint16_t kNonMovFirstOpMask  = 0x0700; // 0b0000_0111_0000_0000
    constexpr uint16_t kNonMovSecondOpMask = 0x00e0; // 0b0000_0000_1110_0000
    constexpr uint16_t kNonMovThirdOpMask  = 0x001c; // 0b0000_0000_0001_1100
    constexpr uint16_t kMovFirstOpMask     = 0x0780; // 0b0000_0111_1000_0000
    constexpr uint16_t kMovSecondOpMask    = 0x0078; // 0b0000_0000_0111_1000
    constexpr uint16_t kSizeBitMask        = 0x0001; // 0b0000_0000_0000_0001

    inline int GetOpcode(uint16_t b) {
        return (b & kOpcodeNumMask) >> 11;
    }

    inline bool IsExtendedInstruction(uint16_t b) {
        return (b & kSizeBitMask) != 0;
    }

    inline int GetNonMovFirstOp(uint16_t b) {
        return (b & kNonMovFirstOpMask) >> 8;
    }

    inline int GetNonMovSecondOp(uint16_t b) {
        return (b & kNonMovSecondOpMask) >> 5;
    }

    inline int GetNonMovThirdOp(uint16_t b) {
        return (b & kNonMovThirdOpMask) >> 2;
    }

    inline int GetMovFirstOp(uint16_t b) {
        return (b & kMovFirstOpMask) >> 7;
    }

    inline int GetMovSecondOp(uint16_t b) {
        return (b & kMovSecondOpMask) >> 3;
    }

    inline int IsSetPcInstruction(Opcode opcode) {
        return opcode == Call ||
            opcode == Jmp ||
            opcode == Ret ||
            opcode == Je ||
            opcode == Jne ||
            opcode == Jl ||
            opcode == Jg ||
            opcode == Jle ||
            opcode == Jge;
    }
}
