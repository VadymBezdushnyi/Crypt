#pragma once

#include <map>
#include <BigInt/BigInt.h>

namespace algo::math{

BigInt inverseMod(BigInt num, BigInt mod);

BigInt powMod(BigInt base, BigInt power, BigInt mod);

BigInt normMod(BigInt num, BigInt mod);

BigInt addMod(BigInt lhs, BigInt rhs, BigInt mod);

BigInt subMod(BigInt lhs, BigInt rhs, BigInt mod);

BigInt mulMod(BigInt lhs, BigInt rhs, BigInt mod);

BigInt divMod(BigInt lhs, BigInt rhs, BigInt mod);

BigInt sqrt(BigInt num);

BigInt pow(BigInt base, BigInt power);

BigInt chineese(vector<pair<BigInt, BigInt>> nums);

BigInt gcd(BigInt a, BigInt b);

BigInt gcdex(BigInt a, BigInt b, BigInt &x, BigInt &y);

void sieve(int num, std::vector<int> &primes, std::vector<int> &least_prime);

BigInt logMod(BigInt base, BigInt ans, BigInt mod);

BigInt abs(BigInt num);

int legendre_symbol(BigInt a, BigInt p);

int Jacobi_symbol(BigInt a, BigInt b);

bool isPrime(BigInt num);

BigInt GenNextPrime(BigInt num);

bool isPrimeDummy(BigInt num, int num_primes = -1);

bool isPrimeFermat(BigInt num, int num_rounds);

bool isPrimeMillerRabin(BigInt num, int num_rounds);

BigInt _pollardRhoNextNum(BigInt num);

BigInt pollardRhoIteration(BigInt num);

std::map<BigInt, int> factorize(BigInt num, bool dummy = true);

BigInt euler(BigInt num);

int mobius(BigInt num);
}
