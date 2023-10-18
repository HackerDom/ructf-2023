#pragma once

#include <string>
#include <filesystem>
#include <atomic>

#include <interpreter.hpp>

namespace werk::server {
    class Server {
    public:
        explicit Server(std::shared_ptr<Interpreter> interpreter,
                        std::filesystem::path socketPath) : interpreter(std::move(interpreter)),
                                                            socketPath(std::move(socketPath)) {
            // empty
        }

        bool Listen();

    private:
        std::shared_ptr<Interpreter> interpreter;
        const std::filesystem::path socketPath;

        std::atomic_bool listening;
    };
}
