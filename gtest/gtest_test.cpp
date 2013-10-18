#include "thirdparty/gtest/gtest.h"
#include <string>

TEST(GTest, Test)
{
    EXPECT_EQ(1, 1);
    std::string s = "hello";
    EXPECT_EQ("hello", s) << s;
}
