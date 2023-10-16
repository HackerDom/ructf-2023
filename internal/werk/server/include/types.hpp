#pragma once

#include <cstdint>

namespace werk::server {
    // vm identifier type
    typedef uint64_t vd_t;

    constexpr vd_t kEmptyVmDescriptor = ~static_cast<vd_t>(0);
}
