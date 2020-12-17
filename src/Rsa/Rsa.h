//
// Created by Vadym on 12/17/2020.
//

#pragma once

#include <BigInt/BigInt.h>
#include <Algorithms/BigIntMath.h>
#include <random>

namespace algo::rsa {

class Rsa {
    std::mt19937_64 rng;
public:

    struct PrivateKey{
        BigInt p;
        BigInt q;
        BigInt d;
    };

    struct PublicKey{
        BigInt n;
        BigInt e;
    };

    BigInt GenRandomPrime(size_t bit_key_size){
        std::uniform_int_distribution<int> distribution(0, 1);
        BigInt num = 0;
        for(size_t i = 0; i < bit_key_size; i++) {
            auto random = static_cast<int>(distribution(rng));
            num = num * 2 + BigInt{random};
        }
        return math::GenNextPrime(num);
    }

    void GenerateKeys(uint64_t key_size, PrivateKey& pv_key, PublicKey& pb_key) {
        rng.seed(0);
        BigInt p = GenRandomPrime(key_size / 2 - 3);
        BigInt q = GenRandomPrime((key_size + 1) / 2 + 3);
        auto n = p * q;
        auto phi = (p - 1) * (q - 1);
        BigInt e = 104729;// some number
        while(algo::math::gcd(e, phi) != 1) {
            e = e + 1;
        }
        auto d = math::inverseMod(e, phi);

        assert(e * d % phi == 1);

        pv_key.p = p;
        pv_key.q = q;
        pv_key.d = d;

        pb_key.e = e;
        pb_key.n = n;
    }

    BigInt Encrypt(BigInt m, const PublicKey& key) {
        return math::powMod(m, key.e, key.n);
    }

    BigInt Decrypt(BigInt c, const PrivateKey& key) {
        return math::powMod(c, key.d, key.p * key.q);
    }
};

}