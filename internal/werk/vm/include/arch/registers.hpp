#include <unordered_set>
#include <string_view>
#include <vector>

#include <utils/strings.hpp>

namespace werk::arch::registers {
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
