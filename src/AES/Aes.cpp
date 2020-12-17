#include "Aes.h"
#include <iostream>

namespace algo::aes {

Aes::Aes(size_t key_size) : Nk(GetNk(key_size)), Nr(GetNr(key_size)), ExpandedKeySize(GetExpandedKeySize(GetNr(key_size))) {
    if(!(key_size == 128 || key_size == 192 || key_size == 256)) {
        throw std::logic_error("Invalid key size " + std::to_string(key_size));
    }
}

size_t Aes::GetPaddedLen(size_t length) {
    return (length + BlockSize - 1) / BlockSize * BlockSize;
}

void Aes::KeyExpansion(const uint8_t *key, uint8_t *result) {
    uint32_t temp;

    uint32_t i = 0;
    auto key_32 = (uint32_t *) key;

    auto result_32 = reinterpret_cast<uint32_t*>(result);
    for(i = 0; i < Nk; i++) {
        result_32[i] = __builtin_bswap32(key_32[i]);
    }
    for(; i < ExpandedKeySize / 4; i++) {
        temp = result_32[i - 1];
        if(i % Nk == 0) {
            temp = RotWord(temp);
            SubBytes((uint8_t *) &temp, 4);
            temp ^= kRcon[i / Nk];
        } else if(Nk > 6 && i % Nk == 4) {
            std::cout << "Wtf";
            SubBytes((uint8_t *) &temp, 4);
        }
        result_32[i] = result_32[i - Nk] ^ temp;
    }

    for(uint32_t i = 0; i < ExpandedKeySize / 4; i++) {
        uint32_t value = __builtin_bswap32(result_32[i]);
        memcpy(result + 4*i, &value, 4);
    }
}

void Aes::Cipher(const uint8_t *in, uint8_t *out, uint8_t *w) {
    uint8_t state[4 * Nb];

    InitStateFromInput(state, in);
    AddRoundKey(state, w);

    for(uint32_t round = 1; round <= Nr - 1; round++) {
        SubBytes(state, BlockSize);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, w + round * BlockSize);
    }

    SubBytes(state, BlockSize);
    ShiftRows(state);
    AddRoundKey(state, w + Nr * BlockSize);

    InitOutputFromState(out, (const uint8_t *) state);
}

void Aes::InvCipher(const uint8_t *in, uint8_t *out, uint8_t *w) {
    uint8_t state[4 * Nb];

    InitStateFromInput(state, in);
    AddRoundKey(state, w + Nr * BlockSize);

    for(uint32_t round = Nr - 1; round >= 1; round--) {
        InvShiftRows(state);
        InvSubBytes(state, BlockSize);
        AddRoundKey(state, w + round * BlockSize);
        InvMixColumns(state);
    }

    InvShiftRows(state);
    InvSubBytes(state, BlockSize);
    AddRoundKey(state, w);

    InitOutputFromState(out, const_cast<const uint8_t *>(state));
}

void Aes::InitStateFromInput(uint8_t *state, const uint8_t *input) {
    for(uint32_t i = 0; i < 4; i++) {
        for(uint32_t j = 0; j < Nb; j++) {
            state[4 * i + j] = input[i + 4 * j];
        }
    }
}

void Aes::InitOutputFromState(uint8_t *out, const uint8_t *state) {
    for(uint32_t i = 0; i < 4; i++) {
        for(uint32_t j = 0; j < Nb; j++) {
            out[i + 4 * j] = state[i * Nb + j];
        }
    }
}

void Aes::AddRoundKey(uint8_t *state, const uint8_t *w) {
    for(uint32_t i = 0; i < 4; i++) {
        for(uint32_t j = 0; j < Nb; j++) {
            state[4 * i + j] ^= w[i + 4 * j];
        }
    }
}

void Aes::SubBytes(uint8_t *state, uint32_t size) {
    for(uint32_t i = 0; i < size; i++) {
        state[i] = kSBox[state[i]];
    }
}

void Aes::InvSubBytes(uint8_t *state, uint32_t size) {
    for(uint32_t i = 0; i < size; i++) {
        state[i] = kInvSBox[state[i]];
    }
}

uint32_t Aes::RotWord(uint32_t word) {
    return (word << 8) + (word >> 24);
}

void Aes::ShiftRow(uint8_t *row, uint32_t k) {
    // k positions to right
    uint8_t temp[Nb];
    memcpy(temp, row, Nb);

    for(uint32_t i = 0; i < Nb; i++) {
        row[i] = temp[(i + k) % Nb];
    }
}

void Aes::ShiftRows(uint8_t *state) {
    ShiftRow(state + Nb, 1);
    ShiftRow(state + 2 * Nb, 2);
    ShiftRow(state + 3 * Nb, 3);
}

void Aes::InvShiftRows(uint8_t *state) {
    ShiftRow(state + Nb, Nb - 1);
    ShiftRow(state + 2 * Nb, Nb - 2);
    ShiftRow(state + 3 * Nb, Nb - 3);
}

void Aes::MixColumns(uint8_t *state) {
    uint8_t temp[4];
    for(uint32_t j = 0; j < Nb; j++) {
        for(uint32_t i = 0; i < 4; i++) {
            temp[i] = state[i * Nb + j];
        }
        MixColumn(temp);
        for(uint32_t i = 0; i < 4; i++) {
            state[i * Nb + j] = temp[i];
        }
    }
}

void Aes::InvMixColumns(uint8_t *state) {
    uint8_t temp[4];
    for(uint32_t j = 0; j < Nb; j++) {
        for(uint32_t i = 0; i < 4; i++) {
            temp[i] = state[i * Nb + j];
        }
        InvMixColumn(temp);
        for(uint32_t i = 0; i < 4; i++) {
            state[i * Nb + j] = temp[i];
        }
    }
}

uint8_t Aes::dbl(uint8_t a) {
    return (a << 1) ^ (0x11b & -(a >> 7)); // bit-hacks hehe
}

void Aes::MixColumn(uint8_t *cols) {
    uint8_t a = cols[0], b = cols[1], c = cols[2], d = cols[3];
    cols[0] = dbl(a ^ b) ^ b ^ c ^ d;  /* 2a + 3b + c + d */
    cols[1] = dbl(b ^ c) ^ c ^ d ^ a;  /* 2b + 3c + d + a */
    cols[2] = dbl(c ^ d) ^ d ^ a ^ b;  /* 2c + 3d + a + b */
    cols[3] = dbl(d ^ a) ^ a ^ b ^ c;  /* 2d + 3a + b + c */
}

void Aes::InvMixColumn(uint8_t *cols) {
    uint8_t a = cols[0], b = cols[1], c = cols[2], d = cols[3];
    uint8_t x = dbl(a ^ b ^ c ^ d), y = dbl(x ^ a ^ c), z = dbl(x ^ b ^ d);
    cols[0] = dbl(y ^ a ^ b) ^ b ^ c ^ d;  /* 14a + 11b + 13c + 9d */
    cols[1] = dbl(z ^ b ^ c) ^ c ^ d ^ a;  /* 14b + 11c + 13d + 9a */
    cols[2] = dbl(y ^ c ^ d) ^ d ^ a ^ b;  /* 14c + 11d + 13a + 9b */
    cols[3] = dbl(z ^ d ^ a) ^ a ^ b ^ c;  /* 14d + 11a + 13b + 9c */
}

constexpr uint32_t Aes::GetNk(uint32_t key_size) {
    switch(key_size) {
        case 128:
            return 4;
        case 192:
            return 6;
        case 256:
            return 8;
        default:
            return 0;
    }
}

constexpr uint32_t Aes::GetNr(uint32_t key_size) {
    switch(key_size) {
        case 128:
            return 10;
        case 192:
            return 12;
        case 256:
            return 14;
        default:
            return 0;
    }
}

constexpr uint32_t Aes::GetExpandedKeySize(uint32_t num_rounds) {
    return Nb*(num_rounds + 1)*4;
}


}