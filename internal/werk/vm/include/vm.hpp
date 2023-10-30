#pragma once

#include <vector>

//TODO: blank interface

namespace werk::vm {
    class Vm {
    public:
        explicit Vm(void *memory);
        virtual ~Vm() = default;

        void Tick(int opsCount);

        bool Stopped();

        [[nodiscard]] const std::vector<char>& GetSerial() const;

        void *GetMemory();

    private:
        void *memory;

        std::vector<char> serial;
    };
}