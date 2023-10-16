#include <gtest/gtest.h>

#include <utils/strings.hpp>

TEST(CaseInsensitiveStringViewsHash, IsCaseInsensitive) {
    auto h = werk::utils::CaseInsensitiveStringViewsHash();

    ASSERT_EQ(h.operator()("Mov"), h.operator()("mov"));
    ASSERT_NE(h.operator()("mov"), h.operator()("mov "));
}

TEST(CaseInsensitiveStringViewsHash, IsSameAsLoweredStdHash) {
    auto h = werk::utils::CaseInsensitiveStringViewsHash();

    ASSERT_EQ(h.operator()("ThIs is tExT!"), std::hash<std::string>().operator()("this is text!"));
}

TEST(CaseInsensitiveStringViewsEquals, IsCaseInsensitive) {
    auto e = werk::utils::CaseInsensitiveStringViewsEquals();

    ASSERT_TRUE(e.operator()("", ""));
    ASSERT_TRUE(e.operator()("aaa", "aaa"));
    ASSERT_TRUE(e.operator()("Aaa bbB", "aAA BBb"));
}
