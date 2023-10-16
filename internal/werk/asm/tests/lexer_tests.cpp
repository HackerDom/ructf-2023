#include <gtest/gtest.h>

#include <lexer.hpp>

using namespace werk::assembler;
using namespace std::literals;

static std::string tokensToString(const std::vector<Token> &tokens) {
    std::stringstream ss;

    for (auto t: tokens) {
        ss << ", " << t.String();
    }

    return ss.str();
}

static void assertTokenizeResult(std::string_view text, std::initializer_list<Token> expectedTokens,
                                 const std::string &expectedError) {
    auto lexer = Lexer(std::make_shared<werk::arch::opcodes::OpcodesManager>(),
                       std::make_shared<werk::arch::registers::RegistersManager>());

    auto result = lexer.Tokenize(text);

    EXPECT_EQ(tokensToString(result.Tokens), tokensToString(expectedTokens));
    EXPECT_EQ(result.ErrorMessage, expectedError);
    if (expectedError.empty()) {
        EXPECT_TRUE(result.Success);
    } else {
        EXPECT_FALSE(result.Success);
    }
}

TEST(Lexer, EmptyString) {
    assertTokenizeResult(""s, {Token(Token::Type::Eof, "")}, "");
}

TEST(Lexer, OnlyWhitespacesString) {
    assertTokenizeResult("  \n \t    \t\t\t\n\n\n   \n"s, {Token(Token::Type::Eof, "")}, "");
}
