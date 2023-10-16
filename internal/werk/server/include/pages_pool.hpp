#pragma once

#include <vector>
#include <mutex>

#include <arch/constants.hpp>

#include <types.hpp>

namespace werk::server {
    union Page {
        vd_t vd;
        uint8_t memory[arch::constants::kMemorySize];
    };

    class PagesPool {
    public:
        // warning: heavy operation
        explicit PagesPool(std::size_t count);

        PagesPool() = delete;

        PagesPool(const PagesPool &) = delete;

        PagesPool(PagesPool &&) = delete;

        PagesPool &operator=(const PagesPool &) = delete;

        PagesPool &operator=(PagesPool &&) = delete;

        struct PageTakeResult {
            bool success;
            Page *page;
        };

        PageTakeResult Allocate(vd_t vd);

        void Deallocate(Page *page);

    private:
        std::vector<Page> pages;
        std::mutex pagesMutex;

        static void deallocateUnsafe(Page *page);
    };
}
