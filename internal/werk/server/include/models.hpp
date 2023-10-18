#pragma once

#include <filesystem>

#include <types.hpp>

namespace werk::server {
    struct RunRequest {
        std::filesystem::path binaryFilePath;
    };

    struct RunResponse {
        bool success;
        vd_t vd;
        std::string errorMessage;
    };

    struct KillRequest {
        vd_t vd;
    };

    struct KillResponse {
        bool success;
    };

    struct StatusRequest {
        vd_t vd;
    };

    struct StatusResponse {
        enum Status {
            RUNNING = 0,
            EXECUTION_TIMEOUT = 1,
            NOT_FOUND = 2,
            KILLED = 3,
        };
        Status status;
    };
}