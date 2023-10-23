#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace werk::server {
    class RunIdGenerator {
    public:
        static constexpr int kIdLength = 16;
        static constexpr std::string_view kAlphabet = "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef"
                                                      "0123456789abcdef0123456789abcdef";

        explicit RunIdGenerator(std::string prefix) : prefix(std::move(prefix)) {
            // empty
        }

        struct GenerateResult {
            bool success;
            std::string id;
            std::string errorMessage;
        };

        [[nodiscard]] GenerateResult Generate() const;

        const std::string prefix;
    };
}
