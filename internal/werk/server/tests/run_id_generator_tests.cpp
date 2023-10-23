#include <vector>
#include <unordered_set>
#include <thread>

#include <gtest/gtest.h>

#include <run_id_generator.hpp>

using werk::server::RunIdGenerator;

static void generateAndWrite(
        const std::shared_ptr<RunIdGenerator> &gen,
        std::unordered_set<std::string> &out,
        int count) {
    for (int i = 0; i < count; ++i) {
        auto res = gen->Generate();
        ASSERT_TRUE(res.success);
        ASSERT_EQ(res.errorMessage, "");
        ASSERT_EQ(res.id.size(), gen->prefix.size() + RunIdGenerator::kIdLength);
        for (std::size_t j = 0; j < gen->prefix.size(); ++j) {
            ASSERT_EQ(res.id[j], gen->prefix[j]);
        }

        for (std::size_t j = gen->prefix.size(); j < res.id.size(); ++j) {
            auto found = false;
            for (char c : RunIdGenerator::kAlphabet) {
                if (c == res.id[j]) {
                    found = true;
                    break;
                }
            }
            ASSERT_TRUE(found);
        }

        out.insert(std::move(res.id));
    }
}

TEST(RunIdGenerator, GeneratesDifferentStrings) {
    constexpr int kIdsCount = 1'000'000;
    auto gen = std::make_shared<RunIdGenerator>("foobar_");
    std::unordered_set<std::string> allGeneratedIds;
    allGeneratedIds.reserve(kIdsCount);

    generateAndWrite(gen, allGeneratedIds, kIdsCount);

    ASSERT_EQ(allGeneratedIds.size(), kIdsCount);
}

TEST(RunIdGenerator, IsThreadSafe) {
    constexpr int kIdsPerThreadCount = 100'000;
    constexpr int kThreadsCount = 8;
    auto gen = std::make_shared<RunIdGenerator>("zzzzz_");
    std::vector<std::unordered_set<std::string>> results;
    results.resize(kThreadsCount);
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreadsCount; ++i) {
        threads.emplace_back([&results, i, gen] {
            generateAndWrite(gen, results[i], kIdsPerThreadCount);
        });
    }

    std::unordered_set<std::string> joinedIds;
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
