#include <interpreter.hpp>

namespace werk::server {
    Interpreter::Interpreter(
            std::shared_ptr<Scheduler> scheduler,
            std::shared_ptr<PagesPool> pagesPool,
            std::shared_ptr<VdGenerator> vdGenerator,
            std::chrono::milliseconds sleepPeriodMs)
            : vdGenerator(std::move(vdGenerator)),
              scheduler(std::move(scheduler)),
              pagesPool(std::move(pagesPool)),
              sleepPeriod(sleepPeriodMs) {
    }

    Interpreter::~Interpreter() {
        if (executorThread != nullptr) {
            executorThreadStop = true;
            executorThread->join();
        }
    }

    RunResponse Interpreter::Run(const RunRequest &request) {
        auto vd = vdGenerator->Generate();
        if (!vd) {
            return {false, kEmptyVmDescriptor, vd.message};
        }
        auto page = pagesPool->Allocate(vd.value);
        if (!page.success) {
            return {false, kEmptyVmDescriptor, "cant allocate space for vm"};
        }

//        std::memcpy(
//                reinterpret_cast<void *>(page.page->memory + arch::constants::kProgramLoadOffset),
//                code,
//                codeSize
//        );

//        auto vm = std::make_shared<vm::Vm>(page.page->memory);
//
//        {
//            std::lock_guard<std::mutex> _(vdMapMutex);
//            vdToVm[vd.value] = vm;
//        }
//
//        scheduler->Append(vm);

        return {true, vd.value, ""};
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

    StatusResponse Interpreter::Status(const StatusRequest &request) {
        return StatusResponse();
    }

    KillResponse Interpreter::Kill(const KillRequest &request) {
        return KillResponse();
    }

    void Interpreter::executorThreadTask() {
        while (!this->executorThreadStop) {
            this->scheduler->UpdateAll();

            //TODO: check vms for too long run

            std::this_thread::sleep_for(sleepPeriod);
        }
    }
}

