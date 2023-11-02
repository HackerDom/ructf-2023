#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <memory>

#include <arch/opcodes.hpp>
#include <arch/registers.hpp>

namespace werk::assembler {
    struct Token {
        enum Type {
            Opcode = 0,
            Label = 1,
            Immediate = 2,
            Address = 3,
            RegisterName = 4,
            Colon = 5,
            Comma = 6,
            Eof = 7,
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
        Lexer(std::shared_ptr<vm::OpcodesManager> om,
              std::shared_ptr<vm::RegistersManager> rm) : opcodesManager(std::move(om)),
                                                                       registersManager(std::move(rm)) {
        }

        struct TokenizeResult {
            bool Success;
            std::string ErrorMessage;
            std::vector<Token> Tokens;
        };

        TokenizeResult Tokenize(std::string_view text);

    private:
        std::shared_ptr<vm::OpcodesManager> opcodesManager;
        std::shared_ptr<vm::RegistersManager> registersManager;

        using TokenReadResult = std::pair<Token, std::string>;

        TokenReadResult readImm(std::string_view text, int &i);
        TokenReadResult readAddress(std::string_view text, int &i);
        TokenReadResult readOpcodeOrLabel(std::string_view text, int &i);
    };
}