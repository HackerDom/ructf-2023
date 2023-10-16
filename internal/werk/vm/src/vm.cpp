#include <glog/logging.h>

#include <vm.hpp>

namespace werk::vm {
    Vm::Vm(void *memory) : memory(memory) {
        LOG(INFO) << "some vm started on page " << memory;
    }

    void Vm::Tick() {
        LOG(INFO) << "tick on vm at memory " << memory;
    }
}