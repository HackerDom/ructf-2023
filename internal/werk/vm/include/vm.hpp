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

        [[nodiscard]] Status GetStatus() const;

        [[nodiscard]] uint64_t GetTotalTicksCount() const;

        [[nodiscard]] virtual const std::vector<char>& GetSerial() const;

        void *GetMemory();

    private:
        void *memory;

        Status status;
        std::vector<char> serial;
        RegistersSet registers;
        uint64_t totalTicksCount;

        Status tickInternal(int &remainOpsCount);

        struct ParsedInstruction {
            Opcode opcode;
            struct {
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

        typedef std::function<bool(ParsedInstruction&)> InstructionHandlerT;

        std::vector<InstructionHandlerT> instructionHandlers;
        void appendHandler(Opcode opcode, InstructionHandlerT handler);

        bool load(ParsedInstruction&);
        bool store(ParsedInstruction&);
        bool mov(ParsedInstruction&);
        bool push(ParsedInstruction&);
        bool pop(ParsedInstruction&);
        bool add(ParsedInstruction&);
        bool sub(ParsedInstruction&);
        bool mul(ParsedInstruction&);
        bool and_(ParsedInstruction&);
        bool or_(ParsedInstruction&);
        bool xor_(ParsedInstruction&);
        bool shl(ParsedInstruction&);
        bool shr(ParsedInstruction&);
        bool call(ParsedInstruction&);
        bool nop(ParsedInstruction&);
        bool jmp(ParsedInstruction&);
        bool jl(ParsedInstruction&);
        bool jg(ParsedInstruction&);
        bool jle(ParsedInstruction&);
        bool jge(ParsedInstruction&);
        bool je(ParsedInstruction&);
        bool jne(ParsedInstruction&);
        bool hlt(ParsedInstruction&);
        bool pushb(ParsedInstruction&);
        bool popb(ParsedInstruction&);
        bool cmp(ParsedInstruction&);
        bool out(ParsedInstruction&);
        bool hexout(ParsedInstruction&);
        bool hexouta(ParsedInstruction&);
        bool ret(ParsedInstruction&);
        bool poweroff(ParsedInstruction&);

        bool parseInstruction(ParsedInstruction &out);

        uint16_t readTwoBytesAt(uint16_t addr);
    };
}
