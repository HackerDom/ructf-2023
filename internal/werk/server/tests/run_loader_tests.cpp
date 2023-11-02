#include <vector>

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <run_loader.hpp>

using namespace werk::server;
using namespace werk::vm;

std::shared_ptr<RunLoader> Loader() {
    auto generator = std::make_shared<VdGenerator>();
    auto pagesPool = std::make_shared<PagesPool>(3);

    return std::make_shared<RunLoader>(10000, generator, pagesPool);
}

void GetCode(std::vector<uint8_t> &code) {
    code.clear();

    int i = 0;
    const char *alpha = "ABCDEF";
    while (code.size() < 10000) {
        code.push_back(alpha[i % std::strlen(alpha)]);
        i++;
    }
}

TEST(RunLoader, LoadFromCode) {
    auto loader = Loader();
    std::vector<uint8_t> code;
    GetCode(code);

    auto p = loader->LoadFromCode(code.data(), code.size());
    ASSERT_TRUE(p.success);

    for (int i = 0; i < static_cast<int>(code.size()); ++i) {
        ASSERT_EQ(
                *(reinterpret_cast<uint8_t *>(p.value->GetVm()->GetMemory()) + i + kProgramLoadOffset),
                code[i]
        );
    }
}
