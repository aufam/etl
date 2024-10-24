#include "etl/logic.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Logic, Logic) {
    val t = True;
    val f = False;
    val i = Indeterminate;

    val tv = True.value;
    val fv = False.value;
    val iv = Indeterminate.value;

    EXPECT_EQ((!i).value, iv);

    EXPECT_EQ((t || t).value, tv);
    EXPECT_EQ((t || f).value, tv);
    EXPECT_EQ((t || i).value, tv);

    EXPECT_EQ((f || t).value, tv);
    EXPECT_EQ((f || f).value, fv);
    EXPECT_EQ((f || i).value, iv);

    EXPECT_EQ((i || t).value, tv);
    EXPECT_EQ((i || f).value, iv);
    EXPECT_EQ((i || i).value, iv);


    EXPECT_EQ((t && t).value, tv);
    EXPECT_EQ((t && f).value, fv);
    EXPECT_EQ((t && i).value, iv);

    EXPECT_EQ((f && t).value, fv);
    EXPECT_EQ((f && f).value, fv);
    EXPECT_EQ((f && i).value, fv);

    EXPECT_EQ((i && t).value, iv);
    EXPECT_EQ((i && f).value, fv);
    EXPECT_EQ((i && i).value, iv);


    EXPECT_EQ((t == t).value, tv);
    EXPECT_EQ((t == f).value, fv);
    EXPECT_EQ((t == i).value, iv);

    EXPECT_EQ((f == t).value, fv);
    EXPECT_EQ((f == f).value, tv);
    EXPECT_EQ((f == i).value, iv);

    EXPECT_EQ((i == t).value, iv);
    EXPECT_EQ((i == f).value, iv);
    EXPECT_EQ((i == i).value, iv);


    EXPECT_EQ((t != t).value, fv);
    EXPECT_EQ((t != f).value, tv);
    EXPECT_EQ((t != i).value, iv);

    EXPECT_EQ((f != t).value, tv);
    EXPECT_EQ((f != f).value, fv);
    EXPECT_EQ((f != i).value, iv);

    EXPECT_EQ((i != t).value, iv);
    EXPECT_EQ((i != f).value, iv);
    EXPECT_EQ((i != i).value, iv);
}