#pragma once

#include <string>
#include <string_view>
#include <utility>

#include <utils/result.hpp>

#include <types.hpp>

namespace werk::server {
    class VdGenerator {
    public:
        [[nodiscard]] utils::result<vd_t> Generate() const;
    };
}
