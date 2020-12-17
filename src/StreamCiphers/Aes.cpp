//
// Created by Vadym on 12/16/2020.
//
#include "Aes.h"
#include <utils.h>

namespace algo::stream {

Stream::Stream(const std::vector<uint8_t> &key) : _key(key), _aes(key.size() * 8) {
    _expanded_key.resize(_aes->ExpandedKeySize);
    _aes->KeyExpansion(_key.data(), _expanded_key.data());
}

void Stream::Encrypt(CipherMode mode, std::vector<uint8_t> &data) {
    switch(mode) {
        case CipherMode::kEcb:
            EncryptEcb(data);
            break;
        case CipherMode::kCbc:
            EncryptCbc(data);
            break;
        case CipherMode::kCfb:
            EncryptCfb(data);
            break;
        case CipherMode::kOfb:
            EncryptOfb(data);
            break;
        case CipherMode::kCtr:
            EncryptCtr(data);
            break;
    }
}

void Stream::Decrypt(CipherMode mode, std::vector<uint8_t> &data) {
    switch(mode) {
        case CipherMode::kEcb:
            DecryptEcb(data);
            break;
        case CipherMode::kCbc:
            DecryptCbc(data);
            break;
        case CipherMode::kCfb:
            DecryptCfb(data);
            break;
        case CipherMode::kOfb:
            DecryptOfb(data);
            break;
        case CipherMode::kCtr:
            DecryptCtr(data);
            break;
    }
}

void Stream::EncryptEcb(std::vector<uint8_t> &data) {
    for(size_t i = 0; i < data.size(); i += _aes->BlockSize) {
        _aes->Cipher(data.data() + i, data.data() + i, _expanded_key.data());
    }
}

void Stream::DecryptEcb(std::vector<uint8_t>& data) {
    for(size_t i = 0; i < data.size(); i += _aes->BlockSize) {
        _aes->InvCipher(data.data() + i, data.data() + i, _expanded_key.data());
    }
}

void Stream::EncryptCbc(std::vector<uint8_t> &data) {
    auto iv = utils::GenerateRandomVec<uint8_t>(_aes->BlockSize);
    // IV can be stored separately but it is also can be just concatenated with input
    data.insert(data.begin(), iv.begin(), iv.end());

    for(size_t i = _aes->BlockSize; i < data.size(); i += _aes->BlockSize) {
        // xor with last encrypted block
        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= data[i - _aes->BlockSize + j];
        }
        _aes->Cipher(data.data() + i, data.data() + i, _expanded_key.data());
    }
}

void Stream::DecryptCbc(std::vector<uint8_t> &data) {
    for(size_t i = data.size() - _aes->BlockSize; i >= _aes->BlockSize; i -= _aes->BlockSize) {
        _aes->InvCipher(data.data() + i, data.data() + i, _expanded_key.data());
        // xor with previous encrypted block
        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= data[i - _aes->BlockSize + j];
        }
    }
    data.erase(data.begin(), data.begin() + _aes->BlockSize);
}

void Stream::EncryptCfb(std::vector<uint8_t> &data) {
    auto iv = utils::GenerateRandomVec(_aes->BlockSize);
    data.insert(data.begin(), iv.begin(), iv.end());
    auto encrypted_buffer = iv;
    // IV will be stored in the beginning

    constexpr size_t offset = 4;
    for(size_t i = _aes->BlockSize; i < data.size(); i += offset) {
        _aes->Cipher(iv.data(), encrypted_buffer.data(), _expanded_key.data());

        for(size_t j = 0; j < offset; j++) {
            data[i+j] ^= encrypted_buffer[j];
        }

        iv.erase(iv.begin(), iv.begin() + offset);
        iv.insert(iv.end(), data.begin() + i, data.begin() + i + offset);
    }
}

void Stream::DecryptCfb(std::vector<uint8_t> &data) {
    std::vector<uint8_t> iv(data.begin(), data.begin() + _aes->BlockSize);
    auto encrypted_buffer = iv;
    // IV will be stored in the beginning

    constexpr size_t offset = 4;
    for(size_t i = _aes->BlockSize; i < data.size(); i += offset) {
        _aes->Cipher(iv.data(), encrypted_buffer.data(), _expanded_key.data());

        iv.erase(iv.begin(), iv.begin() + offset);
        iv.insert(iv.end(), data.begin() + i, data.begin() + i + offset);

        for(size_t j = 0; j < offset; j++) {
            data[i+j] ^= encrypted_buffer[j];
        }
    }
    data.erase(data.begin(), data.begin() + _aes->BlockSize);
}

void Stream::EncryptOfb(std::vector<uint8_t> &data) {
    auto iv = utils::GenerateRandomVec(_aes->BlockSize);
    data.insert(data.begin(), iv.begin(), iv.end());
    // IV will be stored in the beginning
    for(size_t i = _aes->BlockSize; i < data.size(); i += _aes->BlockSize) {
        _aes->Cipher(iv.data(), iv.data(), _expanded_key.data());
        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= iv[j];
        }
    }
}

void Stream::DecryptOfb(std::vector<uint8_t> &data) {
    std::vector<uint8_t> iv(data.begin(), data.begin() + _aes->BlockSize);
    for(size_t i = _aes->BlockSize; i < data.size(); i += _aes->BlockSize) {
        _aes->Cipher(iv.data(), iv.data(), _expanded_key.data());
        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= iv[j];
        }
    }
    data.erase(data.begin(), data.begin() + _aes->BlockSize);
}

void Stream::EncryptCtr(std::vector<uint8_t> &data) {
    auto iv = utils::GenerateRandomVec(_aes->BlockSize);
    auto encrypted_buffer = iv;
    uint64_t counter = 0;

    data.insert(data.begin(), iv.begin(), iv.end());
    auto iv_value = reinterpret_cast<uint64_t*>(iv.data());
    for(size_t i = _aes->BlockSize; i < data.size(); i += _aes->BlockSize) {
        *iv_value ^= counter++;
        _aes->Cipher(iv.data(), encrypted_buffer.data(), _expanded_key.data());

        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= encrypted_buffer[j];
        }
    }
}

void Stream::DecryptCtr(std::vector<uint8_t> &data) {
    std::vector<uint8_t> iv(data.begin(), data.begin() + _aes->BlockSize);
    auto encrypted_buffer = iv;
    uint64_t counter = 0;

    auto iv_value = reinterpret_cast<uint64_t*>(iv.data());
    for(size_t i = _aes->BlockSize; i < data.size(); i += _aes->BlockSize) {
        *iv_value ^= counter++;
        _aes->Cipher(iv.data(), encrypted_buffer.data(), _expanded_key.data());

        for(size_t j = 0; j < _aes->BlockSize; j++) {
            data[i+j] ^= encrypted_buffer[j];
        }
    }
    data.erase(data.begin(), data.begin() + iv.size());
}


}