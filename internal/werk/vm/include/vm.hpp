#pragma once

//TODO: blank interface

namespace werk::vm {
    class Vm {
    public:
        explicit Vm(void *memory);
        virtual ~Vm() = default;

        virtual void Tick(int opsCount);

        [[nodiscard]] virtual int GetTotalTicksCount() const;

        virtual bool Stopped();

    private:
        void *memory;
    };
}