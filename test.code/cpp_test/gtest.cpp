#include <gtest/gtest.h>

int add(int a, int b)
{
    return a+b;
}

TEST(main, add)
{
    EXPECT_EQ(2,add(1,1));
    EXPECT_EQ(5,add(4,1));
    EXPECT_EQ(3,add(4,1));
    EXPECT_EQ(8,add(4,1));
}


int main(int argc, char* argv[])
{

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
