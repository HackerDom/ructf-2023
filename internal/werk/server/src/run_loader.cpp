#include <cstring>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utils/strings.hpp>
#include <utils/defer.hpp>

#include <run_loader.hpp>

namespace werk::server {
    RunLoader::RunLoader(uint64_t ticksLimit, std::shared_ptr<VdGenerator> vdGenerator,
                         std::shared_ptr<PagesPool> pagesPool)
            : ticksLimit(ticksLimit), vdGenerator(std::move(vdGenerator)), pagesPool(std::move(pagesPool)) {
        // empty
    }

    RunLoader::RunResultT RunLoader::LoadFromCode(uint8_t *code, std::size_t size) {
        if (vm::kProgramLoadOffset + size >= vm::kMemorySize) {
            return RunResultT::of_error("the program is too big");
        }

        auto vd = vdGenerator->Generate();
        if (!vd) {
            return RunResultT::of_error("cant generate vd");
        }
        auto page = pagesPool->Allocate(vd.value);
        if (!page.success) {
            return RunResultT::of_error("cant allocate space for vm");
        }

        std::memcpy(
                page.page->memory + vm::kProgramLoadOffset,
                code,
                size
        );

        auto vm = std::make_shared<vm::Vm>(page.page);
        auto run = std::make_shared<Run>(vd.value, vm, ticksLimit);

        return RunResultT::of_success(std::move(run));
    }

    RunLoader::RunResultT RunLoader::LoadFromFile(const std::filesystem::path &path) {
        auto fd = open(path.c_str(), O_RDONLY);
        if (fd < 0) {
            return RunResultT::of_error(utils::PError("open"));
        }
        utils::Defer fileClose(close, fd);

        struct stat statResult{};
        if (fstat(fd, &statResult) == -1) {
            return RunResultT::of_error(utils::PError("fstat"));
        }

        if (static_cast<std::size_t>(statResult.st_size) > (vm::kMemorySize - vm::kProgramLoadOffset)) {
            return RunResultT::of_error("program file is too big");
        }

        auto mapped = mmap(nullptr, statResult.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (mapped == MAP_FAILED) {
            return RunResultT::of_error(utils::PError("mmap"));
        }
        utils::Defer um(munmap, mapped, statResult.st_size);

        return LoadFromCode(reinterpret_cast<uint8_t *>(mapped), statResult.st_size);
    }
}
