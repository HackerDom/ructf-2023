#pragma once

#include <functional>
#include <vector>

#include <arch/instructions.hpp>
#include <arch/registers.hpp>

namespace werk::vm {
    class Vm {
    public:
        enum Status {
            Running,
            Crashed,
            Finished
        };

        explicit Vm(void *memory);
        virtual ~Vm() = default;

        virtual Status Tick(int opsCount);

        [[nodiscard]] virtual Status GetStatus() const;

        [[nodiscard]] virtual uint64_t GetTotalTicksCount() const;

        [[nodiscard]] virtual const std::vector<char>& GetSerial() const;

        void *GetMemory();

        RegistersSet registers;

    private:
        void *memory;
        uint8_t *memoryBytePtr;

        Status status;
        std::vector<char> serial;
        uint64_t totalTicksCount;

        Status tickInternal(int &remainOpsCount);

        struct ParsedInstruction {
            Opcode opcode;
            struct {
                bool extended;
                int first;
                int second;
                int third;
            } operands;
            struct {
                uint16_t value;
                bool defined;
            } imm;
            uint16_t size;
            bool setPc;
        };

        typedef std::function<Status(ParsedInstruction&)> InstructionHandlerT;

        std::vector<InstructionHandlerT> instructionHandlers;
        void appendHandler(Opcode opcode, InstructionHandlerT handler);

        Status load(ParsedInstruction&);
        Status store(ParsedInstruction&);
        Status mov(ParsedInstruction&);
        Status push(ParsedInstruction&);
        Status pop(ParsedInstruction&);
        Status add(ParsedInstruction&);
        Status sub(ParsedInstruction&);
        Status mul(ParsedInstruction&);
        Status and_(ParsedInstruction&);
        Status or_(ParsedInstruction&);
        Status xor_(ParsedInstruction&);
        Status shl(ParsedInstruction&);
        Status shr(ParsedInstruction&);
        Status call(ParsedInstruction&);
        Status nop(ParsedInstruction&);
        Status jmp(ParsedInstruction&);
        Status jl(ParsedInstruction&);
        Status jg(ParsedInstruction&);
        Status jle(ParsedInstruction&);
        Status jge(ParsedInstruction&);
        Status je(ParsedInstruction&);
        Status jne(ParsedInstruction&);
        Status hlt(ParsedInstruction&);
        Status pushb(ParsedInstruction&);
        Status popb(ParsedInstruction&);
        Status cmp(ParsedInstruction&);
        Status out(ParsedInstruction&);
        Status hexout(ParsedInstruction&);
        Status hexouta(ParsedInstruction&);
        Status ret(ParsedInstruction&);
        Status poweroff(ParsedInstruction&);

        bool parseInstruction(ParsedInstruction &out);

        uint16_t readTwoBytesAt(uint16_t addr);

        void push16(uint16_t val);
        void pop16(uint16_t *val);

        Status extractAddressArg(ParsedInstruction &instr, uint16_t *out);
    };
}
