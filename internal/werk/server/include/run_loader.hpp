#pragma once

#include <memory>
#include <cstdint>
#include <filesystem>

#include <run.hpp>
#include <utils/result.hpp>

#include <vd_generator.hpp>
#include <pages_pool.hpp>

namespace werk::server {
    class RunLoader {
    public:
        explicit RunLoader(
                uint64_t ticksLimit,
                std::shared_ptr<VdGenerator> vdGenerator,
                std::shared_ptr<PagesPool> pagesPool
        );

        virtual ~RunLoader() = default;

        typedef utils::result<std::shared_ptr<Run>> RunResultT;

        [[nodiscard]] virtual RunResultT LoadFromCode(uint8_t *code, std::size_t size);

        [[nodiscard]] virtual RunResultT LoadFromFile(const std::filesystem::path &path);

    private:
        const uint64_t ticksLimit;
        const std::shared_ptr<VdGenerator> vdGenerator;
        const std::shared_ptr<PagesPool> pagesPool;
    };
}
