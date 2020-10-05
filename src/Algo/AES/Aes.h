#pragma once

#include <array>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <omp.h>
#include <cstring>

namespace algo::aes {
template<uint32_t KeySize>
class Aes {
public:
    void Encrypt(uint8_t *input, size_t input_len, uint8_t key[KeySize / 8], uint8_t *output, size_t &output_len) {
        uint32_t expanded_key[ExpandedKeySize];
        KeyExpansion(key, expanded_key);
        for(uint32_t i = 0; i < ExpandedKeySize; i++) {
            expanded_key[i] = __builtin_bswap32(expanded_key[i]);
        }

        size_t block_count = input_len / BlockSize;

        auto expanded_key8 = (uint8_t *) expanded_key;
//#pragma omp parallel for
        for(size_t i = 0; i < block_count; i++) {
            Cipher(input + BlockSize * i, output + BlockSize * i, expanded_key8);
        }
        output_len = block_count * BlockSize;

        size_t remaining_data = input_len % BlockSize;
        if(remaining_data > 0) {
            uint8_t padded_input[BlockSize] = {0};
            memcpy(padded_input, input + BlockSize * block_count, remaining_data);
            Cipher(padded_input, output + BlockSize * block_count, expanded_key8);
            output_len += BlockSize;
        }
    }

    void Decrypt(uint8_t *input, uint8_t key[KeySize / 8], uint8_t *output, size_t output_len) {
        uint32_t expanded_key[Nb * (Nr + 1)];
        KeyExpansion(key, expanded_key);
        for(uint32_t i = 0; i < Nb * (Nr + 1); i++) {
            expanded_key[i] = __builtin_bswap32(expanded_key[i]);
        }

        constexpr size_t block_size = 16;
        size_t block_count = output_len / block_size;

        auto expanded_key8 = (uint8_t *) expanded_key;
//#pragma omp parallel for
        for(size_t i = 0; i < block_count; i++) {
            InvCipher(input + block_size * i, output + block_size * i, expanded_key8);
        }
        size_t remaining_data = output_len % block_size;
        if(remaining_data > 0) {
            uint8_t padded_input[block_size] = {0};
            memset(padded_input, 0, sizeof(padded_input));
            InvCipher(input + block_size * block_count, padded_input, expanded_key8);
            memcpy(output + block_size * block_count, padded_input, remaining_data);
        }
    }

    void KeyExpansion(const uint8_t *key, uint32_t *result) {
        uint32_t temp;

        uint32_t i = 0;
        auto key_32 = (uint32_t *) key;
        for(i = 0; i < Nk; i++) {
            result[i] = __builtin_bswap32(key_32[i]);
        }
        for(; i < Nb * (Nr + 1); i++) {
            temp = result[i - 1];
            if(i % Nk == 0) {
                temp = RotWord(temp);
                SubBytes((uint8_t *) &temp, 4);
                temp ^= kRcon[i / Nk];
            } else if(Nk > 6 && i % Nk == 4) {
                SubBytes((uint8_t *) &temp, 4);
            }
            result[i] = result[i - Nk] ^ temp;
        }
    }

    void Cipher(const uint8_t *in, uint8_t *out, uint8_t *w) {
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

    void InvCipher(const uint8_t *in, uint8_t *out, uint8_t *w) {
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

    void InitStateFromInput(uint8_t *state, const uint8_t *input) {
        for(uint32_t i = 0; i < 4; i++) {
            for(uint32_t j = 0; j < Nb; j++) {
                state[4 * i + j] = input[i + 4 * j];
            }
        }
    }

    void InitOutputFromState(uint8_t *out, const uint8_t *state) {
        for(uint32_t i = 0; i < 4; i++) {
            for(uint32_t j = 0; j < Nb; j++) {
                out[i + 4 * j] = state[i * Nb + j];
            }
        }
    }

    void AddRoundKey(uint8_t *state, const uint8_t *w) {
        for(uint32_t i = 0; i < 4; i++) {
            for(uint32_t j = 0; j < Nb; j++) {
                state[4 * i + j] ^= w[i + 4 * j];
            }
        }
    }

    void SubBytes(uint8_t *state, uint32_t size) {
        for(uint32_t i = 0; i < size; i++) {
            state[i] = kSBox[state[i]];
        }
    }

    void InvSubBytes(uint8_t *state, uint32_t size) {
        for(uint32_t i = 0; i < size; i++) {
            state[i] = kInvSBox[state[i]];
        }
    }

    uint32_t RotWord(uint32_t word) {
        return (word << 8) + (word >> 24);
    }

    void ShiftRow(uint8_t *row, uint32_t k) {
        // k positions to right
        uint8_t temp[Nb];
        memcpy(temp, row, Nb);

        for(uint32_t i = 0; i < Nb; i++) {
            row[i] = temp[(i + k) % Nb];
        }
    }

    void ShiftRows(uint8_t *state) {
        ShiftRow(state + Nb, 1);
        ShiftRow(state + 2 * Nb, 2);
        ShiftRow(state + 3 * Nb, 3);
    }

    void InvShiftRows(uint8_t *state) {
        ShiftRow(state + Nb, Nb - 1);
        ShiftRow(state + 2 * Nb, Nb - 2);
        ShiftRow(state + 3 * Nb, Nb - 3);
    }

    void MixColumns(uint8_t *state) {
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

    void InvMixColumns(uint8_t *state) {
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

    static inline uint8_t dbl(uint8_t a) {
        return (a << 1) ^ (0x11b & -(a >> 7)); // bit-hacks hehe
    }

    void MixColumn(uint8_t *cols) {
        uint8_t a = cols[0], b = cols[1], c = cols[2], d = cols[3];
        cols[0] = dbl(a ^ b) ^ b ^ c ^ d;  /* 2a + 3b + c + d */
        cols[1] = dbl(b ^ c) ^ c ^ d ^ a;  /* 2b + 3c + d + a */
        cols[2] = dbl(c ^ d) ^ d ^ a ^ b;  /* 2c + 3d + a + b */
        cols[3] = dbl(d ^ a) ^ a ^ b ^ c;  /* 2d + 3a + b + c */
    }

    void InvMixColumn(uint8_t *cols) {
        uint8_t a = cols[0], b = cols[1], c = cols[2], d = cols[3];
        uint8_t x = dbl(a ^ b ^ c ^ d), y = dbl(x ^ a ^ c), z = dbl(x ^ b ^ d);
        cols[0] = dbl(y ^ a ^ b) ^ b ^ c ^ d;  /* 14a + 11b + 13c + 9d */
        cols[1] = dbl(z ^ b ^ c) ^ c ^ d ^ a;  /* 14b + 11c + 13d + 9a */
        cols[2] = dbl(y ^ c ^ d) ^ d ^ a ^ b;  /* 14c + 11d + 13a + 9b */
        cols[3] = dbl(z ^ d ^ a) ^ a ^ b ^ c;  /* 14d + 11a + 13b + 9c */
    }

    static constexpr uint32_t GetNk() {
        switch(KeySize) {
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

    static constexpr uint32_t GetNr() {
        switch(KeySize) {
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

    static size_t GetPaddedLen(size_t length) {
        return (length + BlockSize - 1) / BlockSize * BlockSize;
    }

public:
    Aes() {
        static_assert(KeySize == 128 || KeySize == 192 || KeySize == 256, "Invalid key size");
    }

    static constexpr uint32_t Nb = 4;
    static constexpr uint32_t Nk = GetNk();
    static constexpr uint32_t Nr = GetNr();
    static constexpr uint32_t BlockSize = 4 * Nb;
    static constexpr uint32_t ExpandedKeySize = Nb * (Nr + 1);
    static constexpr uint32_t KeySizeBytes = KeySize/8;


    inline static const uint32_t kRcon[] = {
            0x00000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000,
            0x80000000, 0x1b000000, 0x36000000, 0x6c000000, 0xd8000000, 0xab000000, 0xed000000, 0x9a000000
    };

    inline static const uint8_t kSBox[] = {
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
            0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
            0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
            0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
            0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
            0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed,
            0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
            0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
            0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
            0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
            0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
            0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
            0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
            0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
            0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
            0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
            0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
            0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
            0xb0, 0x54, 0xbb, 0x16};

    inline static const uint8_t kInvSBox[] = {
            0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e,
            0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
            0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32,
            0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
            0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49,
            0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
            0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50,
            0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
            0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05,
            0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
            0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41,
            0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
            0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8,
            0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
            0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b,
            0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
            0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59,
            0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
            0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d,
            0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
            0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63,
            0x55, 0x21, 0x0c, 0x7d};
};

} // namespace algo::aes