#pragma once

#include <vector>

//TODO: blank interface

namespace werk::vm {
    class Vm {
    public:
        enum Status {
            Running,
            Crashed,
            Finished
        };

        explicit Vm(void *memory);
        virtual ~Vm() = default;

        virtual Status Tick(int opsCount);

        [[nodiscard]] virtual const std::vector<char>& GetSerial() const;

        void *GetMemory();

    private:
        void *memory;

        std::vector<char> serial;
    };
}