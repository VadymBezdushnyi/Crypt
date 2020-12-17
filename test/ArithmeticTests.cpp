//
// Created by heimdall on 25.10.18.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <BigInt/BigInt.h>

using testing::Eq;

TEST(BigIntAdd, ADD_TEST_1){
    BigInt a(4), b(998), res(1002);
    ASSERT_THAT(a+b, a+b);
}

TEST(BigIntAdd, ADD_TEST_2){
    ASSERT_THAT(
            BigInt("1378428734234174513415267865") +
            BigInt("16843385785723684354957368763458293"),
            BigInt("16843387164152418589131882178726158"));
}

TEST(BigIntAdd, ADD_TEST_3){
    ASSERT_THAT(
            BigInt("1831") +
            BigInt("-142"),
            BigInt("1689"));
}
TEST(BigIntAdd, ADD_TEST_4){
    ASSERT_THAT(
            BigInt("-1831") +
            BigInt("142"),
            BigInt("-1689"));
}

TEST(BigIntAdd, ADD_TEST_5){
    ASSERT_THAT(
            BigInt("-1831") +
            BigInt("-142"),
            BigInt("-1973"));
}

TEST(BigIntAdd, ADD_TEST_6){
    ASSERT_THAT(
            BigInt("-142") +
            BigInt("1831"),
            BigInt("1689"));
}

TEST(BigIntAdd, 7){
    ASSERT_THAT(
            BigInt("-143646346347342") +
            BigInt("183253463574611"),
            BigInt("39607117227269"));
}

TEST(BigIntAdd, ADD_TEST_7){
    for(int i = -10; i <= 10; i++){
        for(int j = -10; j <= 10; j++){
            ASSERT_EQ(BigInt(i) + BigInt(j), BigInt(i+j));
            ASSERT_EQ(BigInt(i) - BigInt(j), BigInt(i-j));
        }
    }
}

TEST(BigIntAdd, ADD_TEST_8){
    for(int i = -10; i <= 10; i++){
        for(int j = -10; j <= 10; j++){
            ASSERT_EQ(BigInt(i) * BigInt(j), BigInt(i*j));
        }
    }
}

TEST(BigIntCompare, COMPARE_TEST_1){
    ASSERT_TRUE(BigInt(123) < BigInt(422));
}
TEST(BigIntCompare, COMPARE_TEST_2){
    ASSERT_TRUE(BigInt(92934283) == BigInt(92934283));
}

TEST(BigIntCompare, COMPARE_TEST_3){
    ASSERT_TRUE(BigInt(92934284) > BigInt(92934283));
}

TEST(BigIntCompare, COMPARE_TEST_4){
    ASSERT_TRUE(BigInt(314) != BigInt(-314));
}

TEST(BigIntCompare, COMPARE_TEST_5){
    ASSERT_TRUE(BigInt(314) == BigInt(314));
}

TEST(BigIntCompare, COMPARE_TEST_6){
    ASSERT_TRUE(BigInt(-314) < BigInt(314));
}

TEST(BigIntCompare, COMPARE_TEST_7){
    ASSERT_TRUE(BigInt(0) == BigInt(0));
}

//TEST(BigIntShift, SHIFT_1){
//    BigInt num("123");
//    ASSERT_THAT(num<<=2, BigInt("12300"));
//}
//
//TEST(BigIntShift, SHIFT_2){
//    BigInt num("12345");
//    ASSERT_THAT(num>>=2, BigInt("123"));
//}
//
//TEST(BigIntShift, SHIFT_3){
//    BigInt num("123");
//    ASSERT_THAT(num>>=3, BigInt("0"));
//}

TEST(BigIntDivMod, DIVMOD_1){
    ASSERT_THAT(BigInt(100)/BigInt(5), BigInt(100/5));
}

TEST(BigIntDivMod, DIVMOD_2){
    ASSERT_THAT(BigInt(12314425)/BigInt(5623), BigInt(12314425/5623));
}


TEST(BigIntDivMod, DIVMOD_3){
    for(int i = 1; i <= 100; i++){
        for(int j = 1; j <= 100; j++){
            ASSERT_EQ(BigInt(i) / BigInt(j), BigInt(i/j));
            ASSERT_EQ(BigInt(i) % BigInt(j), BigInt(i%j));
        }
    }
}



/*
TEST(BigIntPow, POW_1){
    for(long long x = 2; x <= 7; x++){
        long long res = 1;
        for(long long y = 0; y <= 10; y++){
            ASSERT_EQ(pow(BigInt(x), BigInt(y)), BigInt(res));
            res *= x;
        }
    }
}


TEST(BigIntSqrt, SQRT_1){
    for(long long x = 2; x <= 1000; x++){
        ASSERT_EQ(BigInt(static_cast<long long>(sqrt(x))), (BigInt(x).sqrt()));
    }
}

TEST(BigIntSqrt, SQRT_2){
    for(long long x = 2; x <= 1'000'000'000'000LL; x *= 1.6){
        ASSERT_EQ(BigInt(static_cast<long long>(sqrt(x))), (BigInt(x).sqrt()));
    }
}*/
