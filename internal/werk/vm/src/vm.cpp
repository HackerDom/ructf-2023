#include <glog/logging.h>

#include <vm.hpp>

namespace werk::vm {
    Vm::Vm(void *memory) : memory(memory) {
        LOG(INFO) << "some vm started on page " << memory;
    }

    void Vm::Tick(int opsCount) {
        LOG(INFO) << opsCount << " ticks on vm with memory at " << memory;
    }

    bool Vm::Stopped() {
        LOG(INFO) << "is vm with memory at address " << memory << " stopped?";

        return false;
    }
}