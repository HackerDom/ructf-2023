#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <memory>

#include <utils/result.hpp>

#include <types.hpp>

namespace werk::server {
    struct RunRequest {
        std::filesystem::path binaryPath;

        explicit RunRequest(std::filesystem::path binaryPath)
            : binaryPath(std::move(binaryPath)) {
            // empty
        }

        [[nodiscard]] std::string String() const;

        static utils::result<std::shared_ptr<RunRequest>> ReadFromSocket(int fd);
    };

    struct RunResponse {
        bool success;
        vd_t vd;
        std::string errorMessage;

        [[nodiscard]] std::string String() const;

        int WriteToSocket(int fd);
    };

    struct KillRequest {
        vd_t vd;

        [[nodiscard]] std::string String() const;

        static utils::result<std::shared_ptr<KillRequest>> ReadFromSocket(int fd);
    };

    struct KillResponse {
        bool success;

        [[nodiscard]] std::string String() const;

        int WriteToSocket(int fd);
    };
}