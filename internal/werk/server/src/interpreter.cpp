#include <cstring>

#include <interpreter.hpp>

namespace werk::server {
    vd_t Interpreter::generateDescriptor() {
        do {
            ++currentDescriptor;
        } while (currentDescriptor == kEmptyVmDescriptor);

        return currentDescriptor;
    }

    Interpreter::Interpreter(
            std::shared_ptr<Scheduler> scheduler,
            std::shared_ptr<PagesPool> pagesPool,
            std::chrono::milliseconds sleepPeriodMs)
            : scheduler(std::move(scheduler)), pagesPool(std::move(pagesPool)), sleepPeriod(sleepPeriodMs) {
        currentDescriptor = kEmptyVmDescriptor;
    }

    Interpreter::~Interpreter() {
        if (executorThread != nullptr) {
            executorThreadStop = true;
            executorThread->join();
        }
    }

    vd_t Interpreter::Create(uint8_t *code, std::size_t codeSize) {
        auto vd = generateDescriptor();
        auto page = pagesPool->Allocate(vd);
        if (!page.success) {
            return kEmptyVmDescriptor;
        }

        std::memcpy(
                reinterpret_cast<void *>(page.page->memory + arch::constants::kProgramLoadOffset),
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

    bool Interpreter::StartExecutorThread() {
        if (executorThread != nullptr) {
            return false;
        }

        executorThreadStop = false;

        executorThread = std::make_shared<std::thread>([this] {
            this->executorThreadTask();
        });

        return true;
    }

    void Interpreter::executorThreadTask() {
        while (!this->executorThreadStop) {
            this->scheduler->TickAll();

            //TODO: check vms for too long run

            std::this_thread::sleep_for(sleepPeriod);
        }
    }
}

