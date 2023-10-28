#include <gtest/gtest.h>

#include <utils/result.hpp>

int constructCount = 0;
int copyCount = 0;
int moveCount = 0;
int destroyCount = 0;

class Count {
public:
    int x, y;

    ~Count() {
        std::cout << "destroy" << std::endl;
        destroyCount++;
    }

    Count(int x, int y) : x(x), y(y) {
        std::cout << "construct" << std::endl;
        constructCount++;
    }

    Count(const Count &o) : x(o.x), y(o.y) {
        std::cout << "copy" << std::endl;
        copyCount++;
    }

    Count(Count &&o) noexcept: x(o.x), y(o.y) {
        std::cout << "move" << std::endl;
        moveCount++;
    }

    Count &operator=(const Count &o) {
        std::cout << "copy assign" << std::endl;
        x = o.x;
        y = o.y;
        copyCount++;
        return *this;
    }

    Count &operator=(Count &&o) noexcept {
        std::cout << "move assign" << std::endl;
        x = o.x;
        y = o.y;
        moveCount++;
        return *this;
    }

};

TEST(Result, EmplaceConstructDoNotCreateCopyOrTempObjects) {
    {
        auto r = werk::utils::result<Count>::of_emplace_success(1, 2);
        ASSERT_EQ(r.value.x, 1);
        ASSERT_EQ(r.value.y, 2);
    }

    ASSERT_EQ(destroyCount, 1);
    ASSERT_EQ(constructCount, 1);
    ASSERT_EQ(copyCount, 0);
    ASSERT_EQ(moveCount, 0);
}
