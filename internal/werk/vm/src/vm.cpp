#include <glog/logging.h>

#include <arch/instructions.hpp>

#include <vm.hpp>

namespace werk::vm {
    Vm::Vm(void *memory) : memory(memory), memoryBytePtr(reinterpret_cast<uint8_t*>(memory)) {
        status = Running;
        totalTicksCount = 0;
        appendHandler(Opcode::Load, [this](ParsedInstruction &i) {return this->load(i);});
        appendHandler(Opcode::Store, [this](ParsedInstruction &i) {return this->store(i);});
        appendHandler(Opcode::Mov, [this](ParsedInstruction &i) {return this->mov(i);});
        appendHandler(Opcode::Push, [this](ParsedInstruction &i) {return this->push(i);});
        appendHandler(Opcode::Pop, [this](ParsedInstruction &i) {return this->pop(i);});
        appendHandler(Opcode::Add, [this](ParsedInstruction &i) {return this->add(i);});
        appendHandler(Opcode::Sub, [this](ParsedInstruction &i) {return this->sub(i);});
        appendHandler(Opcode::Mul, [this](ParsedInstruction &i) {return this->mul(i);});
        appendHandler(Opcode::And, [this](ParsedInstruction &i) {return this->and_(i);});
        appendHandler(Opcode::Or, [this](ParsedInstruction &i) {return this->or_(i);});
        appendHandler(Opcode::Xor, [this](ParsedInstruction &i) {return this->xor_(i);});
        appendHandler(Opcode::Shl, [this](ParsedInstruction &i) {return this->shl(i);});
        appendHandler(Opcode::Shr, [this](ParsedInstruction &i) {return this->shr(i);});
        appendHandler(Opcode::Call, [this](ParsedInstruction &i) {return this->call(i);});
        appendHandler(Opcode::Nop, [this](ParsedInstruction &i) {return this->nop(i);});
        appendHandler(Opcode::Jmp, [this](ParsedInstruction &i) {return this->jmp(i);});
        appendHandler(Opcode::Jl, [this](ParsedInstruction &i) {return this->jl(i);});
        appendHandler(Opcode::Jg, [this](ParsedInstruction &i) {return this->jg(i);});
        appendHandler(Opcode::Jle, [this](ParsedInstruction &i) {return this->jle(i);});
        appendHandler(Opcode::Jge, [this](ParsedInstruction &i) {return this->jge(i);});
        appendHandler(Opcode::Je, [this](ParsedInstruction &i) {return this->je(i);});
        appendHandler(Opcode::Jne, [this](ParsedInstruction &i) {return this->jne(i);});
        appendHandler(Opcode::Hlt, [this](ParsedInstruction &i) {return this->hlt(i);});
        appendHandler(Opcode::Pushb, [this](ParsedInstruction &i) {return this->pushb(i);});
        appendHandler(Opcode::Popb, [this](ParsedInstruction &i) {return this->popb(i);});
        appendHandler(Opcode::Cmp, [this](ParsedInstruction &i) {return this->cmp(i);});
        appendHandler(Opcode::Out, [this](ParsedInstruction &i) {return this->out(i);});
        appendHandler(Opcode::Hexout, [this](ParsedInstruction &i) {return this->hexout(i);});
        appendHandler(Opcode::Hexouta, [this](ParsedInstruction &i) {return this->hexouta(i);});
        appendHandler(Opcode::Ret, [this](ParsedInstruction &i) {return this->ret(i);});
        appendHandler(Opcode::Poweroff, [this](ParsedInstruction &i) {return this->poweroff(i);});
    }

    void Vm::appendHandler(Opcode opcode, Vm::InstructionHandlerT handler) {
        if (static_cast<int>(opcode) != static_cast<int>(instructionHandlers.size())) {
            LOG(ERROR) << "opcodes (" << opcode << ") order error, terminate";
            std::terminate();
        }
        instructionHandlers.push_back(std::move(handler));
    }

    void Vm::push16(uint16_t val) {
        memoryBytePtr[static_cast<uint16_t>(registers.sp + 1)] = static_cast<uint8_t>(val & 0xff);
        memoryBytePtr[static_cast<uint16_t>(registers.sp + 2)] = static_cast<uint8_t>((val & 0xff00) >> 8);
        registers.sp += 2;
    }

    void Vm::pop16(uint16_t *out) {
        *(reinterpret_cast<uint8_t*>(out) + 1) = memoryBytePtr[registers.sp--];
        *(reinterpret_cast<uint8_t*>(out)) = memoryBytePtr[registers.sp--];
    }

    bool Vm::load(ParsedInstruction &instr) {
        auto *target = registers.GetRegisterByOperandNum(instr.operands.first);
        if (target == nullptr) {
            return false;
        }

        auto i = registers.i;

        if (kServiceRegionStart <= i && i <= kServiceRegionEnd) {
            return false;
        }

        auto low = memoryBytePtr[i];
        auto high = memoryBytePtr[i + 1];

        *target = (high << 8) | low;

        return true;
    }

    bool Vm::store(ParsedInstruction &instr) {
        auto *target = registers.GetRegisterByOperandNum(instr.operands.first);
        if (target == nullptr) {
            return false;
        }

        auto i = registers.i;

        if (kServiceRegionStart <= i && i <= kServiceRegionEnd) {
            return false;
        }

        memoryBytePtr[i] = (*target & 0xff);
        memoryBytePtr[i + 1] = ((*target & 0xff00) >> 8);

        return true;
    }

    bool Vm::mov(ParsedInstruction &instr) {
        uint16_t src = 0;
        if (instr.operands.first == 9) {
            if (!instr.imm.defined) {
                return false;
            }

            src = instr.imm.value;
        } else {
            auto *s = registers.GetRegisterByOperandNum(instr.operands.first);
            if (s == nullptr) {
                return false;
            }
            src = *s;
        }

        uint16_t *dst = registers.GetRegisterByOperandNum(instr.operands.second);
        if (dst == nullptr) {
            return false;
        }

        *dst = src;

        return true;
    }

    bool Vm::push(ParsedInstruction &instr) {
        auto *val = registers.GetRegisterByOperandNum(instr.operands.first);
        if (val == nullptr) {
            return false;
        }

        push16(*val);

        return true;
    }

    bool Vm::pop(ParsedInstruction &instr) {
        auto *val = registers.GetRegisterByOperandNum(instr.operands.first);
        if (val == nullptr) {
            return false;
        }

        pop16(val);

        return true;
    }

    bool Vm::add(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 + *op2);

        return true;
    }

    bool Vm::sub(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 - *op2);

        return true;
    }

    bool Vm::mul(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 * *op2);

        return true;
    }

    bool Vm::and_(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 & *op2);

        return true;
    }

    bool Vm::or_(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 | *op2);

        return true;
    }

    bool Vm::xor_(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = static_cast<uint16_t>(*op1 ^ *op2);

        return true;
    }

    bool Vm::shl(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = (*op1 << *op2);

        return true;
    }

    bool Vm::shr(ParsedInstruction &instr) {
        auto *op1 = registers.GetRegisterByOperandNum(instr.operands.first);
        if (op1 == nullptr) {
            return false;
        }
        auto *op2 = registers.GetRegisterByOperandNum(instr.operands.second);
        if (op2 == nullptr) {
            return false;
        }
        auto *dst = registers.GetRegisterByOperandNum(instr.operands.third);
        if (dst == nullptr) {
            return false;
        }

        *dst = (*op1 >> *op2);

        return true;
    }

    bool Vm::call(ParsedInstruction &instr) {
        return false;
    }

    bool Vm::nop(ParsedInstruction &) {
        return true;
    }

    bool Vm::jmp(ParsedInstruction &) {
        return false;
    }

    bool Vm::jl(ParsedInstruction &) {
        return false;
    }

    bool Vm::jg(ParsedInstruction &) {
        return false;
    }

    bool Vm::jle(ParsedInstruction &) {
        return false;
    }

    bool Vm::jge(ParsedInstruction &) {
        return false;
    }

    bool Vm::je(ParsedInstruction &) {
        return false;
    }

    bool Vm::jne(ParsedInstruction &) {
        return false;
    }

    bool Vm::hlt(ParsedInstruction &) {
        return false;
    }

    bool Vm::pushb(ParsedInstruction &) {
        return false;
    }

    bool Vm::popb(ParsedInstruction &) {
        return false;
    }

    bool Vm::cmp(ParsedInstruction &) {
        return false;
    }

    bool Vm::out(ParsedInstruction &) {
        return false;
    }

    bool Vm::hexout(ParsedInstruction &) {
        return false;
    }

    bool Vm::hexouta(ParsedInstruction &) {
        return false;
    }

    bool Vm::ret(ParsedInstruction &) {
        return false;
    }

    bool Vm::poweroff(ParsedInstruction &) {
        return false;
    }

    Vm::Status Vm::Tick(int opsCount) {
        while (opsCount > 0) {
            if (status != Status::Running) {
                break;
            }
            status = tickInternal(opsCount);
        }

        return status;
    }

    const std::vector<char> &Vm::GetSerial() const {
        return serial;
    }

    void *Vm::GetMemory() {
        return memory;
    }

    Vm::Status Vm::GetStatus() const {
        return status;
    }

    uint64_t Vm::GetTotalTicksCount() const {
        return totalTicksCount;
    }

    Vm::Status Vm::tickInternal(int &remainOpsCount) {
        ParsedInstruction instruction{};
        if (!parseInstruction(instruction)) {
            return Status::Crashed;
        }

        if (!instructionHandlers[static_cast<int>(instruction.opcode)](instruction)) {
            return Status::Crashed;
        }

        if (!instruction.setPc) {
            registers.pc += instruction.size;
        }

        --remainOpsCount;
        ++totalTicksCount;

        return Status::Running;
    }

    uint16_t Vm::readTwoBytesAt(uint16_t addr) {
        auto low = static_cast<uint16_t>(reinterpret_cast<uint8_t*>(memory)[addr]);
        auto high = static_cast<uint16_t>(reinterpret_cast<uint8_t*>(memory)[addr + 1]);

        return (high << 8) | low;
    }

    bool Vm::parseInstruction(Vm::ParsedInstruction &out) {
        if (registers.pc == 0xffff) {
            return false;
        }

        auto firstPart = readTwoBytesAt(registers.pc);
        auto opcode = GetOpcode(firstPart);
        if (opcode < 0 || opcode > Opcode::Poweroff) {
            return false;
        }
        out.opcode = static_cast<Opcode>(opcode);
        out.size = 2;
        out.imm.defined = IsExtendedInstruction(firstPart);

        if (out.imm.defined) {
            // imm is in out of memory bound
            if (registers.pc > 0xfffc) {
                return false;
            }
            out.imm.value = readTwoBytesAt(registers.pc + 2);
            out.size = 4;
        }

        if (IsLongInstruction(out.opcode)) {
            out.operands.first = GetLongInstructionFirstOp(firstPart);
            out.operands.second = GetLongInstructionSecondOp(firstPart);
        } else {
            out.operands.first = GetShortInstructionFirstOp(firstPart);
            out.operands.second = GetShortInstructionSecondOp(firstPart);
            out.operands.third = GetShortInstructionThirdOp(firstPart);
        }

        out.setPc = IsSetPcInstruction(static_cast<Opcode>(opcode));

        return true;
    }
}
