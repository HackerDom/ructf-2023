#include <sstream>
#include <utility>

#include <utils/strings.hpp>

#include <lexer.hpp>

static const char *TokenTypeNames[] = {
        "Opcode",
        "Label",
        "Immediate",
        "Address",
        "RegisterName",
        "Colon",
        "Comma",
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

    static inline bool isNameOrOpcodeSymbolStart(char c) {
        return c == '_' || std::isalpha(c);
    }

    static inline bool isNameOrOpcodeAfterStartSymbol(char c) {
        return isNameOrOpcodeSymbolStart(c) || std::isdigit(c);
    }

    static inline bool isCommaSymbol(char c) {
        return c == ',';
    }

    static inline bool isImmSymbol(char c) {
        return std::isdigit(c);
    }

    static inline bool isAddressStartSymbol(char c) {
        return c == '$';
    }

    static inline bool isCommentaryStartSymbol(char c) {
        return c == '#';
    }

    static inline bool isColon(char c) {
        return c == ':';
    }

    Lexer::TokenReadResult Lexer::readImm(std::string_view text, int &i) {
        const auto l = static_cast<int>(text.size());
        const auto start = i;

        while (i < l && isImmSymbol(text[i])) {
            ++i;
        }

        auto s = text.substr(start, i - start);

        if (s.empty()) {
            return std::make_pair(Token(Token::Type::Immediate, ""), "empty immediate or address");
        }

        --i; // for reading cycle

        return std::make_pair(Token(Token::Type::Immediate, s), "");
    }

    Lexer::TokenReadResult Lexer::readAddress(std::string_view text, int &i) {
        // move to number
        ++i;

        auto [immToken, err] = readImm(text, i);
        if (!err.empty()) {
            return std::make_pair(Token(Token::Type::Address, ""), err);
        }

        return std::make_pair(Token(Token::Type::Address, immToken.value), "");
    }

    static void skipCommentary(std::string_view text, int &i) {
        const auto l = static_cast<int>(text.size());

        while (i < l && i != '\n') {
            ++i;
        }

        --i; // for reading cycle
    }

    Lexer::TokenReadResult Lexer::readOpcodeOrLabel(
            std::string_view text,
            int &i) {
        const auto start = i;
        const auto l = static_cast<int>(text.size());

        while (i < l && isNameOrOpcodeAfterStartSymbol(text[i])) {
            ++i;
        }

        auto s = text.substr(start, i - start);

        if (s.empty()) {
            return std::make_pair(Token(Token::Type::RegisterName, ""), "empty name");
        }

        --i; // for reading cycle

        if (s.size() > 20) {
            return std::make_pair(Token(Token::Type::Label, ""),
                                  utils::Format("name '%s' is too long", std::string(s).c_str()));
        }

        if (opcodesManager->IsValidOpcodeName(s)) {
            return std::make_pair(Token(Token::Type::Opcode, s), "");
        }

        if (registersManager->IsValidRegisterName(s)) {
            return std::make_pair(Token(Token::Type::RegisterName, s), "");
        }

        return std::make_pair(Token(Token::Type::Label, s), "");
    }

    Lexer::TokenizeResult Lexer::Tokenize(std::string_view text) {
        auto tokens = std::vector<Token>();

        auto len = static_cast<int>(text.size());

        for (auto i = 0; i < len; ++i) {
            const auto c = text[i];

            if (std::isspace(c)) {
                continue;
            } else if (isImmSymbol(c)) {
                auto [token, err] = readImm(text, i);
                if (!err.empty()) {
                    return {false, err, std::move(tokens)};
                }

                tokens.emplace_back(token);
            } else if (isAddressStartSymbol(c)) {
                auto [token, err] = readAddress(text, i);
                if (!err.empty()) {
                    return {false, err, std::move(tokens)};
                }

                tokens.emplace_back(token);
            } else if (isNameOrOpcodeSymbolStart(c)) {
                auto [token, err] = readOpcodeOrLabel(text, i);
                if (!err.empty()) {
                    return {false, err, std::move(tokens)};
                }

                tokens.emplace_back(token);
            } else if (isCommaSymbol(c)) {
                tokens.emplace_back(Token::Type::Comma, text.substr(i, 1));
            } else if (isColon(c)) {
                tokens.emplace_back(Token::Type::Colon, text.substr(i, 1));
            } else if (isCommentaryStartSymbol(c)) {
                skipCommentary(text, i);
            }
        }

        tokens.emplace_back(Token::Type::Eof, "");

        return {true, "", std::move(tokens)};
    }
}