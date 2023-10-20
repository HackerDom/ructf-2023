#pragma once

#include <filesystem>
#include <string>

#include <types.hpp>

namespace werk::server {
    struct RunRequest {
        std::filesystem::path binaryPath;
        std::filesystem::path serialOutPath;

        [[nodiscard]] std::string String() const;
    };

    struct RunResponse {
        bool success;
        vd_t vd;
        std::string errorMessage;

        [[nodiscard]] std::string String() const;
    };

    struct KillRequest {
        vd_t vd;

        [[nodiscard]] std::string String() const;
    };

    struct KillResponse {
        bool success;

        [[nodiscard]] std::string String() const;
    };

    struct StatusRequest {
        vd_t vd;

        [[nodiscard]] std::string String() const;
    };

    struct StatusResponse {
        enum Status {
            RUNNING = 0,
            EXECUTION_TIMEOUT = 1,
            NOT_FOUND = 2,
            KILLED = 3,
        };
        Status status;

        [[nodiscard]] std::string String() const;
    };
}