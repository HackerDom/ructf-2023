#include <gtest/gtest.h>

#include <lexer.hpp>

using namespace werk::assembler;
using namespace std::literals;
using werk::assembler::Token;

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

TEST(Lexer, Opcodes) {
    const char *text = R"(
mov
load
store
add
sub
mul
setpixel
getpixel
clearscreen
out
hexout
cmp
jmp
je
jne
jl
jg
jle
jge
call
ret
and
or
shl
shr
)";
    assertTokenizeResult(std::string(text), {
            Token(Token::Type::Opcode, "mov"),
            Token(Token::Type::Opcode, "load"),
            Token(Token::Type::Opcode, "store"),
            Token(Token::Type::Opcode, "add"),
            Token(Token::Type::Opcode, "sub"),
            Token(Token::Type::Opcode, "mul"),
            Token(Token::Type::Opcode, "setpixel"),
            Token(Token::Type::Opcode, "getpixel"),
            Token(Token::Type::Opcode, "clearscreen"),
            Token(Token::Type::Opcode, "out"),
            Token(Token::Type::Opcode, "hexout"),
            Token(Token::Type::Opcode, "cmp"),
            Token(Token::Type::Opcode, "jmp"),
            Token(Token::Type::Opcode, "je"),
            Token(Token::Type::Opcode, "jne"),
            Token(Token::Type::Opcode, "jl"),
            Token(Token::Type::Opcode, "jg"),
            Token(Token::Type::Opcode, "jle"),
            Token(Token::Type::Opcode, "jge"),
            Token(Token::Type::Opcode, "call"),
            Token(Token::Type::Opcode, "ret"),
            Token(Token::Type::Opcode, "and"),
            Token(Token::Type::Opcode, "or"),
            Token(Token::Type::Opcode, "shl"),
            Token(Token::Type::Opcode, "shr"),
            Token(Token::Type::Eof, "")
    }, "");
}

TEST(Lexer, LabelDefinition) {
    const char *text = R"(
label: mov
)";

    assertTokenizeResult(std::string(text), {
            Token(Token::Type::Label, "label"),
            Token(Token::Type::Colon, ":"),
            Token(Token::Type::Opcode, "mov"),
            Token(Token::Type::Eof, "")
    }, "");
}

TEST(Lexer, LabelUsage) {
    const char *text = R"(
jmp label
)";

    assertTokenizeResult(std::string(text), {
            Token(Token::Type::Opcode, "jmp"),
            Token(Token::Type::Label, "label"),
            Token(Token::Type::Eof, "")
    }, "");
}

TEST(Lexer, OpcodeInsteadOfLabelName) {
    const char *text = R"(
jmp moV
)";

    assertTokenizeResult(std::string(text), {
            Token(Token::Type::Opcode, "jmp"),
            Token(Token::Type::Opcode, "moV"),
            Token(Token::Type::Eof, "")
    }, "");
}
