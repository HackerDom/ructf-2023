#pragma once

//TODO: blank interface

namespace werk::vm {
    class Vm {
    public:
        explicit Vm(void *memory);

        void Tick();

    private:
        void *memory;
    };
}