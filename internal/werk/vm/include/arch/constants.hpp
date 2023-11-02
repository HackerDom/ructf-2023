#pragma once

#include <cstdint>

namespace werk::vm {
    constexpr std::size_t kMemorySize = 1 << 16; // 64 kB
    constexpr std::size_t kProgramLoadOffset = 0x8;
    constexpr std::size_t kVRegistersCount = 8;
}
