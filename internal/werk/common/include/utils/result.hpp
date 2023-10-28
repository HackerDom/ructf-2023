#pragma once

#include <string>

namespace werk::utils {
    template<class T>
    class result {
    public:
        const bool success;
        const T value;
        const std::string message;

        template<class... Args>
        static result of_emplace_success(Args &&... args) {
            return {true, "", std::piecewise_construct, std::forward<Args>(args)...};
        }

        static result of_success(T &&value) {
            return {true, std::forward<T>(value), ""};
        }

        static result of_success(T value) {
            return {true, std::move(value), ""};
        }

        static result of_success(T value, std::string message) {
            return {true, std::move(value), std::move(message)};
        }

        static result of_error() {
            return {false, T(), ""};
        }

        static result of_error(std::string message) {
            return {false, T(), std::move(message)};
        }

    private:
        result(bool success, T value, std::string message)
                : success(success), value(std::move(value)), message(std::move(message)) {
        }

        result(bool success, T &&value, std::string message)
                : success(success), value(std::forward<T>(value)), message(std::move(message)) {
        }

        template<class... Args>
        result(bool success, std::string message, std::piecewise_construct_t, Args &&... args)
                : success(success), value(std::forward<Args>(args)...), message(std::move(message)) {
        }
    };
}