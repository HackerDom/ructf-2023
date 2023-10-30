#include <glog/logging.h>

#include <vm.hpp>

namespace werk::vm {
    Vm::Vm(void *memory) : memory(memory) {
        LOG(INFO) << "some vm started on page " << this->memory;
    }

    Vm::Status Vm::Tick(int opsCount) {
        LOG(INFO) << opsCount << " ticks on vm with memory at " << memory;
        serial.push_back('A');
        return Status::Running;
    }

    const std::vector<char> &Vm::GetSerial() const {
        return serial;
    }

    void *Vm::GetMemory() {
        return memory;
    }
}
