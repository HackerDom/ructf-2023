#pragma once

#include <functional>

namespace werk::utils {
    class Defer {
    public:
        typedef std::function<void()> HandlerT;

        template<class F, class ...Args>
        explicit Defer(F &&f, Args &&...args) : handler(std::bind(std::forward<F>(f), std::forward<Args>(args)...)) {
        }

        ~Defer() {
            handler();
        }

    private:
        const HandlerT handler;
    };
}
