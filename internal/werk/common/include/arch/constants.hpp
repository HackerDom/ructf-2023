#pragma once

#include <cstdint>

namespace werk::arch::constants {
    constexpr std::size_t kMemorySize = 1 << 16; // 64 kB
    constexpr std::size_t kProgramLoadOffset = 0x200;
}
