#include <cstring>

#include <pages_pool.hpp>

namespace werk::server {
    void PagesPool::deallocateUnsafe(Page *page) {
        std::memset(page->memory, 0, sizeof(page->memory));
        page->vd = kEmptyVmDescriptor;
    }

    PagesPool::PagesPool(std::size_t count) {
        std::lock_guard<std::mutex> lg(pagesMutex);

        pages.resize(count);

        for (auto &p : pages) {
            deallocateUnsafe(&p);
        }
    }

    void PagesPool::Deallocate(Page *page) {
        std::lock_guard<std::mutex> lg(pagesMutex);

        deallocateUnsafe(page);
    }

    PagesPool::PageTakeResult PagesPool::Allocate(vd_t vd) {
        std::lock_guard<std::mutex> lg(pagesMutex);

        for (auto &p : pages) {
            if (p.vd == kEmptyVmDescriptor) {
                p.vd = vd;
                return {true, &p};
            }
        }

        return {false, nullptr};
    }
}
