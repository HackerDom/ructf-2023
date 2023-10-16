#include <gtest/gtest.h>

#include <pages_pool.hpp>

using namespace werk::server;

std::shared_ptr<PagesPool> Pool(std::size_t count) {
    return std::make_shared<PagesPool>(count);
}

bool PagesOverlaps(Page *p1, Page *p2) {
    void *a = p1->memory;
    void *b = p2->memory;

    if (b < a) {
        std::swap(a, b);
    }

    return b < reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(a) + werk::arch::constants::kMemorySize);
}

TEST(PagesPool, SeqAllocate) {
    auto pool = Pool(3);

    auto p1 = pool->Allocate(0x121314);
    ASSERT_TRUE(p1.success);
    ASSERT_EQ(p1.page->vd, 0x121314);

    auto p2 = pool->Allocate(0x123daddd);
    ASSERT_TRUE(p2.success);
    ASSERT_EQ(p2.page->vd, 0x123daddd);

    auto p3 = pool->Allocate(0x343536);
    ASSERT_TRUE(p3.success);
    ASSERT_EQ(p3.page->vd, 0x343536);

    ASSERT_FALSE(PagesOverlaps(p1.page, p2.page));
    ASSERT_FALSE(PagesOverlaps(p1.page, p3.page));
    ASSERT_FALSE(PagesOverlaps(p2.page, p3.page));

    auto failed = pool->Allocate(0x65453434);
    ASSERT_FALSE(failed.success);
}

TEST(PagesPool, DeallocateThenAllocate) {
    auto pool = Pool(2);

    auto p1 = pool->Allocate(0x121314);
    ASSERT_TRUE(p1.success);
    ASSERT_EQ(p1.page->vd, 0x121314);

    auto p2 = pool->Allocate(0x123daddd);
    ASSERT_TRUE(p2.success);
    ASSERT_EQ(p2.page->vd, 0x123daddd);

    auto failed = pool->Allocate(0x65453434);
    ASSERT_FALSE(failed.success);

    pool->Deallocate(p1.page);

    auto p3 = pool->Allocate(0xdeadbeefcafebabe);
    ASSERT_TRUE(p3.success);
    ASSERT_EQ(reinterpret_cast<void*>(p3.page), reinterpret_cast<void*>(p1.page));
}
