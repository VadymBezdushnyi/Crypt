//
// Created by heimdall on 01.11.18.
//

#include "gtest/gtest.h"
#include <Algorithms/BigIntMath.h>

using testing::Eq;

TEST(NormMod, NormMod_1_Test) {
    for (int i = 0; i < 20; i++) {
        BigInt x, y, d;
        d = algo::math::gcdex(84, 33, x, y);
        ASSERT_EQ(84 * x + 33 * y, d);
    }
}

TEST(NormMod, NormMod_2_Test) {
    ASSERT_EQ(algo::math::inverseMod(34795381, 226339651), BigInt(38572232));
}

TEST(Chineese, Chineese_Chineese1_Test) {
    vector<pair<BigInt, BigInt>> v = {{134972356,    226339651},
                                      {122670865,    226336049},
                                      {187543924341, 316767252433},
                                      {1311433,      2801863},
                                      {1561821,      1632523}};
    BigInt ans = algo::math::chineese(v);
    vector<BigInt> x(v.size());
    vector<BigInt> y(v.size());
    for (int i = 0; i < x.size(); i++) {
        x[i] = ans % v[i].second;
        y[i] = v[i].first;
    }
    ASSERT_EQ(x, y);
}

TEST(Chineese, Chineese_Chineese4_Test) {
    vector<pair<BigInt, BigInt>> v = {{134972356, 226339651},
                                      {122670865, 226336049}};
    BigInt ans = algo::math::chineese(v);
    vector<BigInt> x(v.size());
    vector<BigInt> y(v.size());
    for (int i = 0; i < x.size(); i++) {
        x[i] = ans % v[i].second;
        y[i] = v[i].first;
    }
    ASSERT_EQ(x, y);
}


TEST(Chineese, Chineese_Chineese2_Test) {
    vector<pair<BigInt, BigInt>> v = {{3, 5},
                                      {2, 7},
                                      {2, 3}};
    BigInt ans = algo::math::chineese(v);
    vector<BigInt> x(v.size());
    vector<BigInt> y(v.size());
    for (int i = 0; i < x.size(); i++) {
        x[i] = ans % v[i].second;
        y[i] = v[i].first;
    }
    ASSERT_EQ(x, y);
}

TEST(Chineese, Chineese_Chineese3_Test) {
    vector<pair<BigInt, BigInt>> v = {{2, 5},
                                      {3, 7}};
    BigInt tens("17");
    ASSERT_EQ(tens, algo::math::chineese(v));
}


TEST(Legendre, Legendre_test_1) {
    ASSERT_EQ(algo::math::legendre_symbol(18, 31), 1);
    ASSERT_EQ(algo::math::legendre_symbol(83, 17), 1);

    ASSERT_EQ(algo::math::legendre_symbol(8, 61), -1);
    ASSERT_EQ(algo::math::legendre_symbol(20, 107), -1);

    ASSERT_EQ(algo::math::legendre_symbol(25, 5), 0);
}


TEST(IsPrimeFermat, FERMAT_1) {
    ASSERT_TRUE(algo::math::isPrimeFermat(1023949, 5));
    ASSERT_TRUE(algo::math::isPrimeFermat(846739, 10));

    ASSERT_FALSE(algo::math::isPrimeFermat(867017552311, 5));
    ASSERT_FALSE(algo::math::isPrimeFermat(867017552311, 20));

    const int MAX = 10000;
    vector<int> pr, lp;
    algo::math::sieve(MAX, pr, lp);
    for (int i = MAX - 100; i < MAX; i++) {
        EXPECT_EQ(lp[i] == i, algo::math::isPrimeFermat(i, 10)) << " num:" << i;
    }
    ASSERT_TRUE(algo::math::isPrimeFermat(BigInt("340561"), 5)); // CARMICHAEL
}

TEST(IsPrimeMillerRabin, MILLER_RABIN_1) {
    ASSERT_TRUE(algo::math::isPrimeMillerRabin(1023949, 5));
    ASSERT_TRUE(algo::math::isPrimeMillerRabin(846739, 10));

    ASSERT_FALSE(algo::math::isPrimeMillerRabin(867017552311, 5));
    ASSERT_FALSE(algo::math::isPrimeMillerRabin(867017552311, 20));

    const int MAX = 10000;
    vector<int> pr, lp;
    algo::math::sieve(MAX, pr, lp);
    for (int i = MAX - 100; i < MAX; i++) {
        EXPECT_EQ(lp[i] == i, algo::math::isPrimeMillerRabin(i, 10)) << " num:" << i;
    }

    ASSERT_TRUE(algo::math::isPrimeFermat(BigInt("340561"), 5)); // CARMICHAEL
}



TEST(IsPrimeMillerRabin, MILLER_RABIN_2) {
    // size:   2^128
    ASSERT_TRUE(algo::math::isPrimeMillerRabin({"340282366920938463463374607431768211507"}, 50));
    ASSERT_FALSE(algo::math::isPrimeMillerRabin({"340282366920938463463374607431768211509"}, 50));
}

TEST(GenPrime, g1) {
    BigInt num = BigInt{2};
    num = algo::math::pow(num, 128);
    auto prime = algo::math::GenNextPrime(num);
    std::cout << prime.to_string() << std::endl;
}

TEST(IsPrime, 1) {
    const int MAX = 1'000'000;
    vector<int> pr, lp;
    algo::math::sieve(MAX, pr, lp);
    for (int i = MAX - 100; i < MAX; i++) {
        ASSERT_EQ(lp[i] == i, algo::math::isPrime(i)) << " num:" << i;
    }
}

TEST(logMod, 1) {
    BigInt base = 3, ans = 13, mod = 17;
    ASSERT_EQ(ans, algo::math::powMod(base, algo::math::logMod(base, ans, mod), mod));
}

TEST(logMod, 2) {
    BigInt base = 5, ans = 34, mod = 91;
    ASSERT_EQ(ans, algo::math::powMod(base, algo::math::logMod(base, ans, mod), mod));
}
