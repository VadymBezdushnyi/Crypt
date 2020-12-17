#pragma once

#include <Aes/Aes.h>
#include <optional>

namespace algo::stream {
using aes::Aes;

enum class CipherMode {
    kEcb,
    kCbc,
    kCfb,
    kOfb,
    kCtr,
};

inline std::string_view GetCipherModeName(CipherMode mode) {
    constexpr std::string_view modes[] = {"Ecb", "Cbc", "Cfb", "Ofb", "Ctr"};
    return modes[static_cast<size_t>(mode)];
}

class Stream {
public:
    Stream(const std::vector<uint8_t>& key);

    void Encrypt(CipherMode mode, std::vector<uint8_t> &data);
    void Decrypt(CipherMode mode, std::vector<uint8_t> &data);

private:
    const std::vector<uint8_t>& _key;
    std::vector<uint8_t> _expanded_key;
    std::optional<Aes> _aes;

    void EncryptEcb(std::vector<uint8_t>& data);
    void DecryptEcb(std::vector<uint8_t>& data);

    void EncryptCbc(std::vector<uint8_t>& data);
    void DecryptCbc(std::vector<uint8_t>& data);

    void EncryptCfb(std::vector<uint8_t>& data);
    void DecryptCfb(std::vector<uint8_t>& data);

    void EncryptOfb(std::vector<uint8_t>& data);
    void DecryptOfb(std::vector<uint8_t>& data);

    void EncryptCtr(std::vector<uint8_t>& data);
    void DecryptCtr(std::vector<uint8_t>& data);
};

}
