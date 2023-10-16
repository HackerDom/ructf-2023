#include <sstream>

#include <lexer.hpp>

static const char *TokenTypeNames[] = {
    "Opcode",
    "LabelDefinition",
    "Immediate",
    "Address",
    "RegisterName",
    "LabelName",
    "Eof",
};

static const char *GetTokenTypeName(werk::assembler::Token::Type t) {
    return TokenTypeNames[static_cast<int>(t)];
}

namespace werk::assembler {
    std::string Token::String() const {
        std::stringstream ss;

        ss << "Token(Type::" << GetTokenTypeName(type) << ", \"" << value << "\")";

        return ss.str();
    }


    Lexer::TokenizeResult Lexer::Tokenize(std::string_view text) {
        return Lexer::TokenizeResult();
    }
}