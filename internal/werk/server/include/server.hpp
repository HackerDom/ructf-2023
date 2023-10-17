#pragma once

#include <string>
#include <filesystem>

#include <interpreter.hpp>

namespace werk::server {
    class Server {
    public:
        explicit Server(std::shared_ptr<Interpreter> interpreter,
                        std::filesystem::path socketPath) : interpreter(std::move(interpreter)),
                                                            socketPath(std::move(socketPath)) {
            // empty
        }

        void Listen();

    private:
        std::shared_ptr<Interpreter> interpreter;
        const std::filesystem::path socketPath;
    };
}
