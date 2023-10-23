#include <cstring>

#include <sys/random.h>

#include <utils/strings.hpp>

#include <run_id_generator.hpp>

namespace werk::server {
    RunIdGenerator::GenerateResult RunIdGenerator::Generate() const {
        static_assert(kAlphabet.size() == (1 << (sizeof(uint8_t) * 8)));

        uint8_t buffer[kIdLength];
        auto ngenerate = getrandom(buffer, sizeof(buffer), 0);
        if (ngenerate < 0) {
            return {false, "", utils::PError("getrandom")};
        }
        if (ngenerate != kIdLength) {
            return {false, "", "too little bytes generated (out of entropy?)"};
        }

        std::string result;
        result.reserve(prefix.size() + kIdLength);
        result.resize(prefix.size());

        std::memcpy(result.data(), prefix.data(), prefix.size());

        for (uint8_t i: buffer) {
            result.push_back(kAlphabet[i]);
        }

        return {true, std::move(result), ""};
    }
}