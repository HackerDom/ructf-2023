#include <vector>
#include <cmath>

#include <gtest/gtest.h>

#include <interpreter.hpp>

using namespace werk::server;
using namespace std::literals::chrono_literals;

namespace {
    class TestVm : public werk::vm::Vm {
    public:
        explicit TestVm(void *m) : Vm(m) {
        }

        Vm::Status Tick(int opsCount) override {
            totalTicks += opsCount;
            return Running;
        }

        int totalTicks = 0;

        uint64_t GetTotalTicksCount() const override {
            return static_cast<uint64_t>(totalTicks);
        }

        static std::vector<std::shared_ptr<TestVm>> allVms;
    };

    inline std::vector<std::shared_ptr<TestVm>> TestVm::allVms;
}

std::vector<std::shared_ptr<Run>> allRuns;

class TestRunLoader : public RunLoader {
public:
    TestRunLoader() : RunLoader(10000, nullptr, nullptr) {
    }

    ~TestRunLoader() override = default;

    RunResultT LoadFromFile(const std::filesystem::path &p) override {
        if (fail) {
            return RunResultT::of_error("some_error");
        }

        loadedPaths.push_back(p);

        auto tvm = std::make_shared<TestVm>(reinterpret_cast<void *>(0xcafebabedeadbeef));
        TestVm::allVms.push_back(tvm);
        auto vm = std::static_pointer_cast<werk::vm::Vm>(tvm);
        auto run = std::make_shared<Run>(0xa, vm, 10000);
        allRuns.push_back(run);

        return RunResultT::of_success(run);
    }

    bool fail = false;

    std::vector<std::filesystem::path> loadedPaths;
};

std::pair<std::shared_ptr<Interpreter>, std::shared_ptr<TestRunLoader>> GetInterpreter() {
    TestVm::allVms.clear();
    allRuns.clear();

    auto scheduler = std::make_shared<Scheduler>(500, 500);
    auto pagesPool = std::make_shared<PagesPool>(3);
    auto generator = std::make_shared<VdGenerator>();
    auto runLoader = std::make_shared<TestRunLoader>();
    auto interpreter = std::make_shared<Interpreter>(
            std::static_pointer_cast<RunLoader>(runLoader),
            scheduler,
            300ms
    );

    return std::make_pair(interpreter, runLoader);
}

TEST(Interpreter, RunShouldFailWhenLoaderFailed) {
    auto [interpreter, loader] = GetInterpreter();

    loader->fail = true;

    auto res = interpreter->Run(RunRequest{"~/.bashrc"});

    ASSERT_FALSE(res.success);
    ASSERT_EQ(res.errorMessage, "some_error");
}

TEST(Interpreter, ShouldCallLoader) {
    auto [interpreter, loader] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    res = interpreter->Run(RunRequest{"~/.bashrc2"});
    ASSERT_TRUE(res.success);

    res = interpreter->Run(RunRequest{"~/.bashrc3"});
    ASSERT_TRUE(res.success);

    auto ex = std::vector<std::filesystem::path>({
                                                         std::filesystem::path("~/.bashrc1"),
                                                         std::filesystem::path("~/.bashrc2"),
                                                         std::filesystem::path("~/.bashrc3")
                                                 });

    ASSERT_EQ(loader->loadedPaths, ex);
}

TEST(Interpreter, ShouldTickRunnedVms) {
    auto [interpreter, loader] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    res = interpreter->Run(RunRequest{"~/.bashrc2"});
    ASSERT_TRUE(res.success);

    res = interpreter->Run(RunRequest{"~/.bashrc3"});
    ASSERT_TRUE(res.success);

    std::this_thread::sleep_for(1100ms);

    for (const auto &v: TestVm::allVms) {
        // 300ms pause between execution
        // > 900ms sleep for interpreter
        // at least 3 time ticks from 500 to 1000
        ASSERT_GE(v->totalTicks, 500 * 3);
    }
}

TEST(Interpreter, ShouldReturnFailWhenKillNotExistingVm) {
    auto [interpreter, _] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    auto killRes = interpreter->Kill(KillRequest{res.vd + 1});
    ASSERT_FALSE(killRes.success);
}

TEST(Interpreter, ShouldReturnTrueWhenVmExistsAndKillRun) {
    auto [interpreter, _] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    ASSERT_EQ(allRuns[0]->GetState(), Run::State::Running);

    auto k = interpreter->Kill(KillRequest{res.vd});
    ASSERT_TRUE(k.success);
    ASSERT_EQ(allRuns[0]->GetState(), Run::State::Killed);
}

TEST(Interpreter, ShouldReturnFalseWhenVmAlreadyKilled) {
    auto [interpreter, _] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    ASSERT_EQ(allRuns[0]->GetState(), Run::State::Running);

    auto k = interpreter->Kill(KillRequest{res.vd});
    ASSERT_TRUE(k.success);
    ASSERT_EQ(allRuns[0]->GetState(), Run::State::Killed);

    k = interpreter->Kill(KillRequest{res.vd});
    ASSERT_FALSE(k.success);
}

TEST(Interpreter, NoTicksOnVmAfterDelete) {
    auto [interpreter, _] = GetInterpreter();

    auto res = interpreter->Run(RunRequest{"~/.bashrc1"});
    ASSERT_TRUE(res.success);

    auto st = interpreter->Status(StatusRequest{res.vd});
    ASSERT_TRUE(st.success);
    ASSERT_EQ(st.state, Run::State::Running);

    auto run = allRuns[0];

    std::this_thread::sleep_for(std::chrono::milliseconds(700));

    auto startTicks = run->GetTotalTicks();
    ASSERT_GE(startTicks, 2 * 500);

    auto d = interpreter->Delete(DeleteRequest{res.vd});
    ASSERT_TRUE(d.success);

    st = interpreter->Status(StatusRequest{res.vd});
    ASSERT_FALSE(st.success);

    d = interpreter->Delete(DeleteRequest{res.vd});
    ASSERT_FALSE(d.success);

    auto k = interpreter->Kill(KillRequest{res.vd});
    ASSERT_FALSE(k.success);

    std::this_thread::sleep_for(std::chrono::milliseconds(700));

    // one run can occure between GetTotalTicks and Delete
    ASSERT_LE(run->GetTotalTicks() - startTicks, 500);
}
