#pragma once

#include <AES/Aes.h>
#include <utils.h>

namespace algo::stream {

namespace rc4 {

void init_state(const uint8_t *key, size_t key_len, uint8_t *s) {
    constexpr size_t mod = 256;
    for(size_t i = 0; i < mod; i++) {
        s[i] = i;
    }

    uint8_t j = 0;
    for(size_t i = 0; i < mod; i++) {
        j = (j + s[i] + key[i % key_len]) % mod;
        std::swap(s[i], s[j]);
    }
}

void apply(const uint8_t *text, size_t text_len,
           const uint8_t *key, size_t key_len,
           uint8_t *out) {
    constexpr size_t mod = 256;
    uint8_t state[mod];
    init_state(key, key_len, state);

    uint8_t i = 0, j = 0, t;
    for(size_t it = 0; it < text_len; it++) {
        i = (i + 1) % mod;
        j = (j + state[i]) % mod;

        std::swap(state[i], state[j]);
        t = (state[i] + state[j]) % mod;
        const uint8_t value = state[t];

        out[it] = text[it] ^ value;
    }
}

}

namespace salsa20 {

using utils::FourCC;

inline uint32_t rotate(uint32_t x, uint32_t k) {
    // transforms (0x1a2b3c4d,8) into 0x2b3c4d1a
    return (x << k) | (x >> (32 - k));
}

inline void qr(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    b ^= rotate(a + d, 7);
    c ^= rotate(b + a, 9);
    d ^= rotate(c + b, 13);
    a ^= rotate(d + c, 18);
}

void apply(const uint8_t *in, size_t len,
                  const uint8_t *key,
                  const uint64_t nonce,
                  uint8_t *out) {
    uint32_t last_state[16];
    uint8_t* last_state8 = reinterpret_cast<uint8_t*>(last_state);
    uint32_t nonce_high_bits = nonce >> 32;
    uint32_t nonce_low_bits = nonce & ((1ull << 32) - 1);

    for(size_t i = 0; i < len; i++) {
        if(i % 64 == 0) {
            // recalculate state for every 64-byte block
            uint32_t pos_high_bits = i >> 32;
            uint32_t pos_low_bits = i & ((1ull << 32) - 1);

            uint32_t s[] = {
                FourCC("expa"), key[0],         key[1],         key[2],
                key[3],         FourCC("nd 3"), pos_high_bits,  pos_low_bits,
                nonce_high_bits,nonce_low_bits, FourCC("2-by"), key[4],
                key[5],         key[6],         key[7],         FourCC("te k")
            };

            constexpr size_t kNumRounds = 20 / 2;
            for(size_t i = 0; i < kNumRounds; i++) {
                // Odd round for columns
                qr(s[0],  s[4],  s[8],  s[12]);
                qr(s[5],  s[9],  s[13], s[1]);
                qr(s[10], s[14], s[2],  s[6]);
                qr(s[15], s[3],  s[7],  s[11]);

                // Even round for rows
                qr(s[0],  s[1],  s[2],  s[3]);
                qr(s[5],  s[6],  s[7],  s[4]);
                qr(s[10], s[11], s[8],  s[9]);
                qr(s[15], s[12], s[13], s[14]);
            }

            memcpy(last_state, s, sizeof(s));
        }
        out[i] = in[i] ^ last_state8[i % 64];
    }
}
}
}