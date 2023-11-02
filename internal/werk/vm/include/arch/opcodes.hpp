#pragma once

#include <unordered_set>
#include <string_view>

#include <utils/strings.hpp>

namespace werk::arch::opcodes {
    class OpcodesManager {
    public:
        OpcodesManager();

        // case-insensitive
        [[nodiscard]] bool IsValidOpcodeName(std::string_view name) const;

    private:
        std::unordered_set<std::string_view,
                utils::CaseInsensitiveStringViewsHash,
                utils::CaseInsensitiveStringViewsEquals> allOpcodes;
    };
}
