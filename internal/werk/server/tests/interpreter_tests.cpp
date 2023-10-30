#include <vector>

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

        static std::vector<std::shared_ptr<TestVm>> allVms;
    };

    inline std::vector<std::shared_ptr<TestVm>> TestVm::allVms;
}

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

        return RunResultT::of_success(std::make_shared<Run>(0xa, vm, 10000));
    }

    bool fail = false;

    std::vector<std::filesystem::path> loadedPaths;
};

std::pair<std::shared_ptr<Interpreter>, std::shared_ptr<TestRunLoader>> GetInterpreter() {
    TestVm::allVms.clear();

    auto scheduler = std::make_shared<Scheduler>(500, 1000);
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
