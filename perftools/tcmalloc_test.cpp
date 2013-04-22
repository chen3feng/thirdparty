#include "thirdparty/gtest/gtest.h"

TEST(Tcmalloc, AllocAndFree)
{
    void* p = malloc(4);
    EXPECT_TRUE(p != NULL);
    free(p);
}

TEST(Tcmalloc, NewAndDelete)
{
    double* p = new double;
    EXPECT_TRUE(p != NULL);
    delete p;
}

TEST(Tcmalloc, NewArrayAndDeleteArray)
{
    double* p = new double[2];
    EXPECT_TRUE(p != NULL);
    delete[] p;
}

TEST(TcmallocDeathTest, NewArrayAndDelete)
{
    double* p = new double[2];
    EXPECT_TRUE(p != NULL);
    EXPECT_DEATH(delete p,
                 "allocated with new \\[\\] being deallocated with delete");
    delete[] p;
}

TEST(TcmallocDeathTest, NewAndFree)
{
    double* p = new double;
    EXPECT_TRUE(p != NULL);
    EXPECT_DEATH(free(p),
                 "allocated with new being deallocated with free");
    delete p;
}

TEST(TcmallocDeathTest, Overflow)
{
    char* p = new char[2];
    EXPECT_DEATH({ p[2] = 'A'; delete[] p; },
                 "a word after object at 0x\\w+ has been corrupted");
    delete[] p;
}

TEST(TcmallocDeathTest, Underflow)
{
    char* p = new char[2];
    EXPECT_DEATH({ p[-1] = 'A'; delete[] p; },
                 "a word before object at 0x\\w+ has been corrupted");
    delete[] p;
}

