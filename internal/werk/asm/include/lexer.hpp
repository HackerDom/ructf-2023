#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace werk::assembler {
    struct Token {
        enum Type {
            Opcode = 0,
            LabelDefinition = 1,
            Immediate = 2,
            Address = 3,
            RegisterName = 4,
            LabelName = 5,
            Eof = 6,
        };

        const Type type;
        const std::string_view value;

        [[nodiscard]] std::string String() const;

        bool operator==(const Token &other) const {
            return type == other.type && value == other.value;
        }

        Token(Type t, std::string_view v) : type(t), value(v) {
            // empty
        }
    };

    class Lexer {
    public:
        struct TokenizeResult {
            bool Success;
            std::string ErrorMessage;
            std::vector<Token> Tokens;
        };

        TokenizeResult Tokenize(std::string_view text);
    };
}