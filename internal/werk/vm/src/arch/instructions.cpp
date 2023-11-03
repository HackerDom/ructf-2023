#include <cstring>

#include <arch/instructions.hpp>

namespace werk::vm {
    std::size_t EmitUnpackedInstruction(const UnpackedInstruction &instr, uint8_t *out, std::size_t max) {
        uint16_t firstPart = 0;
        uint16_t secondPart = 0;
        std::size_t size = instr.imm.defined ? 4 : 2;

        firstPart |= (static_cast<uint16_t>(instr.opcode) << 11);

        if (instr.size == 4) {
            firstPart |= kLoadImmBitMask;
        }

        if (instr.operands.extended) {
            firstPart |= kExtendendArgsMask;
            firstPart |= (static_cast<uint16_t>(instr.operands.first) << 7);
            firstPart |= (static_cast<uint16_t>(instr.operands.second) << 3);
        } else {
            firstPart |= (static_cast<uint16_t>(instr.operands.first) << 8);
            firstPart |= (static_cast<uint16_t>(instr.operands.second) << 5);
            firstPart |= (static_cast<uint16_t>(instr.operands.third) << 2);
        }

        std::memcpy(out, &firstPart, sizeof(firstPart));
        if (size == 4) {
            std::memcpy(out + 2, &secondPart, sizeof(secondPart));
        }

        return size;
    }
}
