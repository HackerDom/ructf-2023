#include <memory>

#include <gtest/gtest.h>

#include <arch/registers.hpp>

using namespace werk::arch::registers;

std::shared_ptr<RegistersManager> RegsManager() {
    return std::make_shared<RegistersManager>();
}

TEST(OpcodesManager, IsValidRegisterName) {
    auto man = RegsManager();

    ASSERT_FALSE(man->IsValidRegisterName(""));
    ASSERT_FALSE(man->IsValidRegisterName(" v15l"));

    ASSERT_TRUE(man->IsValidRegisterName("pc"));
    ASSERT_TRUE(man->IsValidRegisterName("sp"));
    for (int i = 0; i < 32; ++i) {
        if (i < 16) {
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%d", i))));
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%dl", i))));
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%dh", i))));
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("V%d", i))));
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("V%dl", i))));
            ASSERT_TRUE(man->IsValidRegisterName(std::string_view(werk::utils::Format("V%dh", i))));
        } else {
            ASSERT_FALSE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%d", i))));
            ASSERT_FALSE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%dl", i))));
            ASSERT_FALSE(man->IsValidRegisterName(std::string_view(werk::utils::Format("v%dh", i))));
        }
    }
}
