#pragma once

#include <cstdint>

namespace werk::vm {
    typedef uint16_t v_register_t;
    typedef uint16_t pc_register_t;
    typedef uint16_t sp_register_t;
    typedef uint8_t f_register_t;
    typedef uint16_t screen_index_register_t;
    typedef uint8_t opcode_t;
}