//
// Created by Vadym on 10/4/2020.
//

#include "gtest/gtest.h"
#include <AES/Aes.h>
#include <array>
#include <chrono>

namespace algo::aes {
class AesTest: public testing::Test{
public:
    inline static const uint8_t kKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf,0x4f, 0x3c};

    void print_hex(uint8_t* p, size_t size){
        for(size_t i = 0; i < size; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)p[i];
        }
        std::cout << std::setfill(' ');
        std::cout << std::endl;
    }
};

TEST_F(AesTest, KeyExpansionTest) {
    Aes aes(128);
    std::vector<uint32_t> expected_expanded_key = {
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
    std::vector<uint8_t> expanded_key(aes.ExpandedKeySize);

    std::cout << aes.ExpandedKeySize << std::endl;

    aes.KeyExpansion(kKey128, expanded_key.data());
    auto expanded_key32 = reinterpret_cast<uint32_t*>(expanded_key.data());
    for(uint32_t i = 0; i < expected_expanded_key.size(); i++) {
        std::cout << std::setw(2) << i << " " << std::hex << (uint32_t)expanded_key[i] << std::dec << std::endl;
        EXPECT_EQ(expanded_key32[i], __builtin_bswap32(expected_expanded_key[i]));
    }
}

TEST_F(AesTest,  ManualCipherTest) {
    Aes aes(128);
    std::vector<uint8_t> expanded_key(aes.ExpandedKeySize);
    aes.KeyExpansion(kKey128, expanded_key.data());

    std::vector<uint8_t> input = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    std::vector<uint8_t> output(16);
    std::vector<uint8_t> expected_output = {
            0x39, 0x25, 0x84, 0x1d,
            0x02, 0xdc, 0x09, 0xfb,
            0xdc, 0x11, 0x85, 0x97,
            0x19, 0x6a, 0x0b, 0x32
    };
    aes.Cipher(input.data(), output.data(), expanded_key.data());

    ASSERT_EQ(output, expected_output);
}

TEST_F(AesTest, CipherTestFromString) {
    Aes aes(128);
    std::vector<uint8_t> input = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    std::vector<uint8_t> key = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

    std::vector<uint8_t> expanded_key(aes.ExpandedKeySize);
    aes.KeyExpansion(key.data(), expanded_key.data());

    std::vector<uint8_t> output(input.size());
    aes.Cipher(input.data(), output.data(), expanded_key.data());

    std::vector<uint8_t> expected_cipher = {0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a};
    ASSERT_EQ(output, expected_cipher);
}


TEST_F(AesTest, DecipherTestFromString) {
    std::vector<uint8_t> input = {0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a};
    std::vector<uint8_t> key = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    Aes aes(128);

    std::vector<uint8_t> expanded_key(aes.ExpandedKeySize);
    aes.KeyExpansion(key.data(), expanded_key.data());

    std::vector<uint8_t> output(input.size());
    aes.InvCipher(input.data(), output.data(), expanded_key.data());

    std::vector<uint8_t> expected_output = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    ASSERT_EQ(output, expected_output);
}


//TEST_F(AesTest, CipherDecipher) {
//    std::vector<size_t> sizes = {100000, 10000000};
//    for(auto size:sizes) {
//        runBenchmark<128>(size);
//        runBenchmark<192>(size);
//        runBenchmark<256>(size);
//    }
//
//    // 1Gb synthetic data
//    // Sequential - 35s
//    // Parallel(8 cores) - 7s
//}
//
//
//TEST_F(AesTest, CipherDecipherVisual) {
//    Aes<256> aes;
//
//    std::array<uint8_t, aes.KeySizeBytes> key;
//    for(size_t i = 0; i < key.size(); i++) {
//        key[i] = rand();
//    }
//    std::cout << std::setw(12) << "Key: ";
//    print_hex(key.data(), key.size());
//
//    std::string input = "Cybernetics Faculty of Taras Shevchenko National University of Kyiv was founded on May 6, 1969. Academician of NAS of Ukraine V.M. Glushkov and dean of the Faculty of Mechanics and Mathematics, Professor I.I. Liashko were its founders. Almost at once after the establishment, Faculty of Cybernetics became a major center of training specialists in the area of applied mathematics and informatics in the Soviet Union."; //  = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//    std::string input = "Cybernetics";
//    size_t output_size;
//
//    std::vector<uint8_t> encrypted(aes.GetPaddedLen(input.size()));
//    std::string decrypted(input.size(), '0');
//
//    aes.EncryptEcb(reinterpret_cast<uint8_t *>(input.data()), input.size(), key.data(), encrypted.data(), output_size);
//    aes.DecryptEcb(encrypted.data(), key.data(), reinterpret_cast<uint8_t *>(decrypted.data()), decrypted.size());
//
//
//
//    std::cout << std::setw(12) << "Input: " << input << std::endl;
//
//    std::cout << std::setw(12) << "Encrypted: ";
//    print_hex(encrypted.data(), encrypted.size());
//    std::cout << std::setw(12) << "Output: " <<  decrypted << std::endl;
//
//    ASSERT_EQ(input, decrypted);
//}

}

