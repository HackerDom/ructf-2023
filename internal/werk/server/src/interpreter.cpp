#include <thread>

#include <interpreter.hpp>

namespace werk::server {
    Interpreter::Interpreter(
            std::shared_ptr<RunLoader> runLoader,
            std::shared_ptr<Scheduler> scheduler,
            std::chrono::milliseconds sleepPeriodMs)
            : runLoader(std::move(runLoader)),
              scheduler(std::move(scheduler)),
              sleepPeriod(sleepPeriodMs) {
        executorThreadStop = false;
        executorThread = std::make_shared<std::thread>([this] {
            this->executorThreadTask();
        });
    }

    Interpreter::~Interpreter() {
        if (executorThread != nullptr) {
            executorThreadStop = true;
            executorThread->join();
        }
    }

    void Interpreter::executorThreadTask() {
        while (!this->executorThreadStop) {
            this->scheduler->UpdateAll();

            std::this_thread::sleep_for(sleepPeriod);
        }
    }

    RunResponse Interpreter::Run(const RunRequest &rq) {
        auto run = runLoader->LoadFromFile(rq.binaryPath);
        if (!run) {
            return RunResponse{false, kEmptyVmDescriptor, run.message};
        }

        scheduler->Append(run.value);

        auto vd = run.value->GetVd();

        {
            std::lock_guard<std::mutex> _(vdMapMutex);
            vdToRun[vd] = run.value;
        }

        return RunResponse{true, vd, ""};
    }
}
