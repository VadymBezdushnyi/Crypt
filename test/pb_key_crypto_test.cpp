#include "gtest/gtest.h"
#include <Rsa/Rsa.h>
#include <chrono>


namespace algo::rsa {
class RsaTest : public testing::Test {
protected:
    void Benchmark(size_t key_size) {
        Rsa rsa;
        Rsa::PrivateKey pv_key;
        Rsa::PublicKey pb_key;


        BigInt message{"12345678901234567890"};
        BigInt expected_message = message;

        auto t1 = std::chrono::high_resolution_clock::now();
        rsa.GenerateKeys(key_size, pv_key, pb_key);
        std::cout << "p: " << pv_key.p << ", q:" << pv_key.q << ", " << pv_key.d << std::endl;
        std::cout << "n: " << pb_key.n << ", e:" << pb_key.e << std::endl;

        ASSERT_EQ(pv_key.p * pv_key.q, pb_key.n);



        auto t2 = std::chrono::high_resolution_clock::now();
        message = rsa.Encrypt(message, pb_key);
        auto t3 = std::chrono::high_resolution_clock::now();

        std::cout << "encrypted: " << message << std::endl;


        message = rsa.Decrypt(message, pv_key);
        auto t4 = std::chrono::high_resolution_clock::now();

        ASSERT_EQ(message, expected_message);

        std::cout << "decrypted: " << message << std::endl;
        auto duration_gen = std::chrono::duration<double>( t2 - t1 ).count();
        auto duration_enc = std::chrono::duration<double>( t3 - t2 ).count();
        auto duration_dec = std::chrono::duration<double>( t4 - t3 ).count();
        std::cout << "RSA-" << key_size << std::endl;
        std::cout << "Key_gen: " << duration_gen << "s" << std::endl;
        std::cout << "Enc    : " << duration_enc << "s" << std::endl;
        std::cout << "Dec    : " << duration_dec << "s" << std::endl;


    }
};

TEST_F(RsaTest, bench128) {
    Benchmark(128);
}

TEST_F(RsaTest, bench256) {
    Benchmark(256);
}

TEST_F(RsaTest, DISABLED_bench512) {
    // 32 sec
    Benchmark(512);
}

}