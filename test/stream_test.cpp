
#include <AES/Aes.h>
#include <StreamCiphers/StreamCiphers.h>
#include <array>
#include <chrono>
#include <iostream>
#include <StreamCiphers/Aes.h>

#include "gtest/gtest.h"
#include "utils.h"

namespace algo::stream {
using namespace utils;
class AesStreamTest : public testing::Test {
protected:
    static constexpr auto kAesKeySize = 128;
    static constexpr auto kAesBlockSize = 16;

    inline static const auto kAesKey = HexToVec("000102030405060708090a0b0c0d0e0f");

    void TestStreamAes(const std::string& input, CipherMode mode) {
        std::cout << std::setw(20) << "Input: " << input << std::endl;

        std::vector<uint8_t> data = ToVec(input);
        AddZeroPadding(data, kAesBlockSize);
        Stream stream(kAesKey);

        stream.Encrypt(mode, data);
        std::cout << std::setw(20) << "Encrypted: " << ToHex(data) << std::endl;
        stream.Decrypt(mode, data);

        auto output_str = ToStr(data);
        output_str.resize(input.size());
        std::cout << std::setw(20) << "Decrypted: " << output_str << std::endl;

        ASSERT_EQ(input, output_str);
    }

    void BenchmarkStreamAes(size_t test_size, CipherMode mode) {
        Stream stream(kAesKey);

        test_size = CeilNumber(test_size, kAesBlockSize);
        std::vector<uint8_t> input(test_size);
        for(size_t i = 0; i < input.size(); i++) {
            input[i] = i; // pseudo random_data
            if(i > 1) input[i] += input[i-1];
            if(i > 13) input[i] += input[i-13];
        }

        auto data = input;

        auto t1 = std::chrono::high_resolution_clock::now();
        stream.Encrypt(mode, data);
        stream.Decrypt(mode, data);
        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration<double>( t2 - t1 ).count();
        std::cout << "AES-" << kAesKeySize << ", mode: " << GetCipherModeName(mode) << ", size: " << (double)test_size << ", time: " << duration << "s" << std::endl;
        ASSERT_EQ(input, data);
    }
};

const CipherMode kCipherModes[] = {CipherMode::kEcb, CipherMode::kCbc, CipherMode::kCfb, CipherMode::kOfb, CipherMode::kCtr};
class AesStreamParametrizedModeTest : public AesStreamTest, public testing::WithParamInterface<CipherMode> {};
INSTANTIATE_TEST_SUITE_P(AnotherInstantiationName, AesStreamParametrizedModeTest, testing::ValuesIn(kCipherModes));

TEST_P(AesStreamParametrizedModeTest, ParametrizedModeTest) {
    std::string input_string = "Cybernetics Faculty of Taras Shevchenko National University of Kyiv was founded on May 6, 1969. Academician of NAS of Ukraine V.M. Glushkov and dean of the Faculty of Mechanics and Mathematics, Professor I.I. Liashko were its founders. Almost at once after the establishment, Faculty of Cybernetics became a major center of training specialists in the area of applied mathematics and informatics in the Soviet Union.";
    TestStreamAes(input_string, GetParam());
}

TEST_P(AesStreamParametrizedModeTest, Benchmark) {
    BenchmarkStreamAes(1'000'000, GetParam());
}

TEST(StreamCiphersTest, Rc4Test) {
    const std::vector<uint8_t> key = ToVec("secret rc4 key");
    const std::vector<uint8_t> text = ToVec("RC4 (Rivest Cipher 4 also known as ARC4 or ARCFOUR meaning Alleged RC4)");

    std::vector<uint8_t> encrypted(text.size());
    std::vector<uint8_t> decrypted(text.size());

    std::cout << std::setw(20) << "Input text: " << ToStr(text) << std::endl;
    std::cout << std::setw(20) << "Input key: " << ToStr(key) << std::endl;

    rc4::apply(text.data(), text.size(), key.data(), key.size(), encrypted.data());
    std::cout << std::setw(20) << "Encrypted text: " << ToHex(encrypted) << std::endl;

    rc4::apply(encrypted.data(), encrypted.size(), key.data(), key.size(), decrypted.data());
    std::cout << std::setw(20) << "Decrypted text: " << ToStr(decrypted) << std::endl;

    ASSERT_EQ(text, decrypted);
}

TEST(StreamCiphersTest, Salsa20Test) {
    const std::vector<uint8_t> key = ToVec("secret salsa key");
    const uint64_t nonce = 0x12979712;
    const std::vector<uint8_t> text = ToVec("Salsa20 and the closely related ChaCha are stream ciphers developed by Daniel J. Bernstein");

    std::vector<uint8_t> encrypted(text.size());
    std::vector<uint8_t> decrypted(text.size());

    std::cout << std::setw(20) << "Input text: " << ToStr(text) << std::endl;
    std::cout << std::setw(20) << "Input key: " << ToStr(key) << std::endl;

    salsa20::apply(text.data(), text.size(), key.data(), nonce, encrypted.data());
    std::cout << std::setw(20) << "Encrypted text: " << ToHex(encrypted) << std::endl;

    salsa20::apply(encrypted.data(), encrypted.size(), key.data(), nonce, decrypted.data());
    std::cout << std::setw(20) << "Decrypted text: " << ToStr(decrypted) << std::endl;

    ASSERT_EQ(text, decrypted);
}

}