#pragma once

#include <unordered_set>
#include <string_view>
#include <vector>

#include <utils/strings.hpp>

#include <arch/types.hpp>
#include <arch/constants.hpp>

namespace werk::vm {
    struct RegistersSet {
        v_register_t v[kVRegistersCount];
        pc_register_t pc;
        sp_register_t sp;
        i_register_t i;
        f_register_t f;

        register_t *GetRegisterByOperandNum(int num);
    };

    class RegistersManager {
    public:
        RegistersManager();

        // case-insensitive
        [[nodiscard]] bool IsValidRegisterName(std::string_view name) const;

    private:
        std::unordered_set<std::string,
                utils::CaseInsensitiveStringHash,
                utils::CaseInsensitiveStringEquals> allRegisterNames;
    };
}
