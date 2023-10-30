#include <vector>
#include <unordered_set>
#include <thread>

#include <gtest/gtest.h>

#include <vd_generator.hpp>

using namespace werk::server;

static void generateAndWrite(
        const std::shared_ptr<VdGenerator> &gen,
        std::unordered_set<vd_t> &out,
        int count) {
    for (int i = 0; i < count; ++i) {
        auto res = gen->Generate();
        ASSERT_TRUE(res.success);

        out.insert(res.value);
    }
}

TEST(RunIdGenerator, GeneratesDifferentStrings) {
    constexpr int kIdsCount = 1'000'000;
    auto gen = std::make_shared<VdGenerator>();
    std::unordered_set<vd_t> allGeneratedIds;
    allGeneratedIds.reserve(kIdsCount);

    generateAndWrite(gen, allGeneratedIds, kIdsCount);

    ASSERT_EQ(allGeneratedIds.size(), kIdsCount);
}

TEST(RunIdGenerator, IsThreadSafe) {
    constexpr int kIdsPerThreadCount = 100'000;
    constexpr int kThreadsCount = 8;
    auto gen = std::make_shared<VdGenerator>();
    std::vector<std::unordered_set<vd_t>> results;
    results.resize(kThreadsCount);
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreadsCount; ++i) {
        threads.emplace_back([&results, i, gen] {
            generateAndWrite(gen, results[i], kIdsPerThreadCount);
        });
    }

    std::unordered_set<vd_t> joinedIds;
    joinedIds.reserve(kIdsPerThreadCount * kThreadsCount);

    for (int i = 0; i < kThreadsCount; ++i) {
        threads[i].join();

        ASSERT_EQ(results[i].size(), kIdsPerThreadCount);

        for (const auto &s: results[i]) {
            joinedIds.insert(s);
        }
    }

    ASSERT_EQ(joinedIds.size(), kIdsPerThreadCount * kThreadsCount);
}
