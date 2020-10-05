//
// Created by Vadym on 10/4/2020.
//

#include "gtest/gtest.h"
#include "../../src/Algo/AES/Aes.h"
#include <array>
#include <chrono>

namespace algo::aes {
class AesTest: public testing::Test{
public:
    inline static const uint8_t kKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf,0x4f, 0x3c};

    template<size_t KeySize>
    void runBenchmark(size_t test_size) {
        Aes<KeySize> aes;

        std::array<uint8_t, KeySize/8> key;
        for(size_t i = 0; i < 16; i++) {
            key[i] = rand();
        }

        std::vector<uint8_t> input(test_size); //  = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
        for(size_t i = 0; i < input.size(); i++) {
            input[i] = input[i-1] + i; // pseudo random_data
        }
        size_t output_size;

        std::vector<uint8_t> encrypted(test_size + 16);
        std::vector<uint8_t> decrypted(test_size);
        auto t1 = std::chrono::high_resolution_clock::now();

        aes.Encrypt(input.data(), input.size(), key.data(), encrypted.data(), output_size);
        aes.Decrypt(encrypted.data(), key.data(), decrypted.data(), decrypted.size());

        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration<double>( t2 - t1 ).count();
        std::cout << "AES-" << KeySize << ", size: " << (double)test_size << ", time: " << duration << "s" << std::endl;

        ASSERT_EQ(input, decrypted);
    }
};

TEST_F(AesTest, KeyExpansionTest) {
    Aes<128> aes;
    std::array<uint32_t, 44> expected_expanded_key = {
            0x2b7e1516,
            0x28aed2a6,
            0xabf71588,
            0x09cf4f3c,

            0xa0fafe17,
            0x88542cb1,
            0x23a33939,
            0x2a6c7605,

            0xf2c295f2,
            0x7a96b943,
            0x5935807a,
            0x7359f67f,

            0x3d80477d,
            0x4716fe3e,
            0x1e237e44,
            0x6d7a883b,

            0xef44a541,
            0xa8525b7f,
            0xb671253b,
            0xdb0bad00,

            0xd4d1c6f8,
            0x7c839d87,
            0xcaf2b8bc,
            0x11f915bc,

            0x6d88a37a,
            0x110b3efd,
            0xdbf98641,
            0xca0093fd,

            0x4e54f70e,
            0x5f5fc9f3,
            0x84a64fb2,
            0x4ea6dc4f,

            0xead27321,
            0xb58dbad2,
            0x312bf560,
            0x7f8d292f,

            0xac7766f3,
            0x19fadc21,
            0x28d12941,
            0x575c006e,

            0xd014f9a8,
            0xc9ee2589,
            0xe13f0cc8,
            0xb6630ca6};

    std::array<uint32_t, 44> expanded_key;


    aes.KeyExpansion(kKey128, expanded_key.data());
    for(uint32_t i = 0; i < aes.Nb * (aes.Nr + 1); i++) {
        std::cout << std::setw(2) << i << " " << std::hex << expanded_key[i] << std::endl;
        EXPECT_EQ(expanded_key[i], expected_expanded_key[i]);
    }
}

TEST_F(AesTest,  ManualCipherTest) {
    Aes<128> aes;
    std::array<uint32_t, 44> expanded_key;
    aes.KeyExpansion(kKey128, expanded_key.data());

    std::array<uint8_t, 16> input = {0x32,0x43,0xf6,0xa8,
                                     0x88,0x5a,0x30,0x8d,
                                     0x31,0x31,0x98,0xa2,
                                     0xe0,0x37,0x07,0x34};
    std::array<uint8_t, 16> output;
    std::array<uint8_t, 16> expected_output = {
0x39,0x25,0x84,0x1d,
0x02,0xdc,0x09,0xfb,
0xdc,0x11,0x85,0x97,
0x19,0x6a,0x0b,0x32
    };
    for(uint32_t i = 0; i < expanded_key.size(); i++) {
        expanded_key[i] = __builtin_bswap32(expanded_key[i]);
    }
    aes.Cipher(input.data(), output.data(), (uint8_t*)expanded_key.data());

    ASSERT_EQ(output, expected_output);
}

TEST_F(AesTest, CipherTestFromString) {
    uint8_t input[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    uint8_t key[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    Aes<128> aes;
    std::array<uint8_t, sizeof(input)> output;
    size_t output_size;
    aes.Encrypt(input, sizeof(input), key, output.data(), output_size);

    for(size_t i = 0; i < output_size; i++) {
        std::cout << std::hex << (uint32_t)output[i];
    }
    std::cout << std::endl;
    std::array<uint8_t, 16> expected_cipher = {0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a};
    ASSERT_EQ(output, expected_cipher);
}


TEST_F(AesTest, DecipherTestFromString) {
    uint8_t input[] = {0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a};
    uint8_t key[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    Aes<128> aes;
    std::array<uint8_t, sizeof(input)> output;
    aes.Decrypt(input, key, output.data(), output.size());

    for(size_t i = 0; i < output.size(); i++) {
        std::cout << std::hex << (uint32_t)output[i];
    }
    std::cout << std::endl;
    std::array<uint8_t, 16> expected_cipher = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    ASSERT_EQ(output, expected_cipher);
}


TEST_F(AesTest, CipherDecipher) {
    std::vector<size_t> sizes = {100000, 10000000, 1000000000};
    for(auto size:sizes) {
        runBenchmark<128>(size);
        runBenchmark<192>(size);
        runBenchmark<256>(size);
    }

    // 1Gb synthetic data
    // Sequential - 35s
    // Parallel(8 cores) - 7s

}
}

