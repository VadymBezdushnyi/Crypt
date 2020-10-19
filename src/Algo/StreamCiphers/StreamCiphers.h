#pragma once
#include "../AES/Aes.h"

namespace algo::stream{
using algo::aes::Aes;

void init_cr4_state(const uint8_t* key, size_t key_len, uint8_t *s) {
    uint8_t j = 0;
    for(size_t i = 0; i < 256; i++) {
        j += s[i] + key[i % key_len];
        std::swap(s[i], s[j]);
    }
}

void apply_rc4(const uint8_t* text, size_t text_len,
               const uint8_t* key, size_t key_len,
               uint8_t* out) {
    uint8_t state[256];
    init_cr4_state(key, key_len, state);

    uint8_t i = 0, j = 0, t;
    for(size_t it = 0; it < text_len; it++) {
        i++;
        j += state[i];

        std::swap(state[i], state[j]);
        t = state[i] + state[j];
        const uint8_t value = state[t];

        out[it] = text[it] ^ value;
    }
}

constexpr uint32_t FourCC(const char p[5]) {
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

void init_salsa20(uint32_t key[4], uint32_t non[2], uint32_t pos[2]) {
    uint32_t state[16] = {
            FourCC("expa"), key[0], key[1], key[2],
            key[3], FourCC("nd 3"), non[0], non[1],
            pos[0], pos[1], FourCC("2-by"), key[4],
            key[5], key[6], key[7], FourCC("te k")
    };
    return;
}

inline uint32_t rotate(uint32_t x, uint32_t k) {
    return (x << k) | (x >> (32-k));
}

inline void salsa_qr(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    b ^= _rotl(a + d, 7);
    c ^= _rotl(b + a, 9);
    d ^= _rotl(c + b, 13);
    a ^= _rotl(d + c, 18);
}

uint32_t apply_salsa(const uint32_t in[16], uint32_t out[16]) {
    uint32_t x[16];
    memcpy(x, in, sizeof(in));

    constexpr size_t kNumRounds = 20 / 2;
    for(size_t i = 0; i < kNumRounds; i++) {
        // Odd round
        salsa_qr(x[0], x[4], x[8], x[12]);
        salsa_qr(x[5], x[9], x[13], x[1]);
        salsa_qr(x[10], x[14], x[2], x[6]);
        salsa_qr(x[15], x[3], x[7], x[11]);

        // Even round
        salsa_qr(x[0], x[1], x[2], x[3]);
        salsa_qr(x[5], x[6], x[7], x[4]);
        salsa_qr(x[10], x[11], x[8], x[9]);
        salsa_qr(x[15], x[12], x[13], x[14]);
    }

    for(int i = 0; i < 16; i++) {
        out[i] = in[i] ^ x[i];
    }

}

}