#include <cstring>

#include <interpreter.hpp>

namespace werk::server {
    vd_t Interpreter::generateDescriptor() {
        do {
            ++currentDescriptor;
        } while(currentDescriptor == kEmptyVmDescriptor);

        return currentDescriptor;
    }

    Interpreter::Interpreter(std::shared_ptr<Scheduler> scheduler, std::shared_ptr<PagesPool> pagesPool)
            : scheduler(std::move(scheduler)), pagesPool(std::move(pagesPool)) {
        currentDescriptor = kEmptyVmDescriptor;
    }

    vd_t Interpreter::Create(uint8_t *code, std::size_t codeSize) {
        auto vd = generateDescriptor();
        auto page = pagesPool->Allocate(vd);
        if (!page.success) {
            return kEmptyVmDescriptor;
        }

        std::memcpy(
                reinterpret_cast<void*>(page.page->memory + arch::constants::kProgramLoadOffset),
                code,
                codeSize
        );

        auto vm = std::make_shared<vm::Vm>(page.page->memory);

        {
            std::lock_guard<std::mutex> _(vdMapMutex);
            vdToVm[vd] = vm;
        }

        scheduler->Append(vm);

        return vd;
    }
}

