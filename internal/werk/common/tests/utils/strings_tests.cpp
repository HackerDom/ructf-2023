#include <gtest/gtest.h>

#include <utils/strings.hpp>

TEST(CaseInsensitiveStringsHash, IsCaseInsensitive) {
    auto h = werk::utils::CaseInsensitiveStringsHash();

    ASSERT_EQ(h.operator()("Mov"), h.operator()("mov"));
    ASSERT_NE(h.operator()("mov"), h.operator()("mov "));
}

TEST(CaseInsensitiveStringsHash, IsSameAsLoweredStdHash) {
    auto h = werk::utils::CaseInsensitiveStringsHash();

    ASSERT_EQ(h.operator()("ThIs is tExT!"), std::hash<std::string>().operator()("this is text!"));
}

TEST(CaseInsensitiveStringsEquals, IsCaseInsensitive) {
    auto e = werk::utils::CaseInsensitiveStringsEquals();

    ASSERT_TRUE(e.operator()("", ""));
    ASSERT_TRUE(e.operator()("aaa", "aaa"));
    ASSERT_TRUE(e.operator()("Aaa bbB", "aAA BBb"));
}
