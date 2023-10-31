#pragma once

#include <string>

namespace werk::utils {
    template<class T>
    class result {
    public:
        const bool success;
        const T value;
        const std::string message;

        [[nodiscard]] explicit operator bool() const noexcept {
            return success;
        }

        template<class... Args>
        static result of_emplace_success(Args &&... args) {
            return {true, "", std::piecewise_construct, std::forward<Args>(args)...};
        }

        static result of_success() {
            return {true, T(), ""};
        }

        static result of_success(T &&value) {
            return {true, std::forward<T>(value), ""};
        }

        static result of_success(const T &value) {
            return {true, value, ""};
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
        result(bool success, const T &value, std::string message)
                : success(success), value(value), message(std::move(message)) {
        }

        result(bool success, T &&value, std::string message)
                : success(success), value(std::forward<T>(value)), message(std::move(message)) {
        }

        template<class... Args>
        result(bool success, std::string message, std::piecewise_construct_t, Args &&... args)
                : success(success), value(std::forward<Args>(args)...), message(std::move(message)) {
        }
    };

    struct empty_result_struct{};
    typedef result<empty_result_struct> result_no_value;
}