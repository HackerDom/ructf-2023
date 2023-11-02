#include <utils/strings.hpp>

#include <arch/registers.hpp>

namespace werk::vm {
    constexpr std::string_view kSpName = "sp";
    constexpr std::string_view kPcName = "pc";

    RegistersManager::RegistersManager() {
        allRegisterNames.insert(std::string(kSpName));
        allRegisterNames.insert(std::string(kPcName));

        for (int i = 0; i < 16; ++i) {
            allRegisterNames.insert(utils::Format("v%d", i));
            allRegisterNames.insert(utils::Format("v%dl", i));
            allRegisterNames.insert(utils::Format("v%dh", i));
        }
    }

    bool RegistersManager::IsValidRegisterName(std::string_view name) const {
        auto it = allRegisterNames.find(std::string(name));

        return it != allRegisterNames.end();
    }
}
