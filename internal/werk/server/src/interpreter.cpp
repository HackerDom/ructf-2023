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

    bool Interpreter::HasVms() const {
        std::lock_guard<std::mutex> _(vdMapMutex);

        return vdToRun.size() != 0;
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

    KillResponse Interpreter::Kill(const KillRequest &rq) {
        std::lock_guard<std::mutex> _(vdMapMutex);

        if (auto it = vdToRun.find(rq.vd); it != vdToRun.end() && it->second->GetState() == Run::State::Running) {
            it->second->Kill();
            return KillResponse{true};
        }

        return KillResponse{false};
    }

    StatusResponse Interpreter::Status(const StatusRequest &rq) {
        std::lock_guard<std::mutex> _(vdMapMutex);

        if (auto it = vdToRun.find(rq.vd); it != vdToRun.end()) {
            return StatusResponse{true, it->second->GetState()};
        }

        return StatusResponse{false, Run::State::InternalError};
    }

    DeleteResponse Interpreter::Delete(const DeleteRequest &rq) {
        std::lock_guard<std::mutex> _(vdMapMutex);

        if (auto it = vdToRun.find(rq.vd); it != vdToRun.end()) {
            scheduler->Remove(it->second);

            vdToRun.erase(it);

            return DeleteResponse{true};
        }

        return DeleteResponse{false};
    }

    GetSerialResponse Interpreter::GetSerial(const GetSerialRequest &rq) {
        std::lock_guard<std::mutex> _(vdMapMutex);

        if (auto it = vdToRun.find(rq.vd); it != vdToRun.end()) {
            return GetSerialResponse{true, it->second->GetSerial()};
        }

        return GetSerialResponse{false, ""};
    }
}
