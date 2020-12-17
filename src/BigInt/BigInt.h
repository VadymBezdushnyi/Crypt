//
// Created by heimdall on 18.10.18.
//

#ifndef BIGINT_BIGINT_H
#define BIGINT_BIGINT_H

#include <vector>
#include <string>
#include <algorithm>
#include <ostream>
#include <iterator>
#include <iostream>
#include "BigUint.h"


class BigInt{
    friend class BigIntTest;

public:
    static const BigInt ZERO;
    static const BigInt ONE;


    int sign;
    BigUint mag;

    BigInt() : BigInt(0) {}

    BigInt(const std::string &s);

    BigInt(int num) : BigInt(std::to_string(num)) {}

    BigInt(long num) : BigInt(std::to_string(num)) {}

    BigInt(long long num) : BigInt(std::to_string(num)) {}

    bool isZero() const;

    void removeZeros();

    std::string to_string() const;
    explicit operator int() const{
        return sign * int(mag);
    }

    void addOrSub(const BigInt &lhs, const BigInt &rhs, bool substract);

    int len(){return mag.len();}

public:
    int compareTo(const BigInt &other) const;

    BigInt operator-() const{
        BigInt res = *this;
        if(!res.isZero())
            res.sign *= -1;
        return res;

    }
    friend BigInt operator+(const BigInt &lhs, const BigInt &rhs);

    friend BigInt operator-(const BigInt &lhs, const BigInt &rhs);

    friend BigInt operator*(const BigInt &lhs, const BigInt &rhs);

    friend BigInt operator/(const BigInt &lhs, const BigInt &rhs);

    friend BigInt operator%(const BigInt &lhs, const BigInt &rhs);

    friend bool operator==(const BigInt &lhs, const BigInt &rhs);

    friend bool operator<(const BigInt &lhs, const BigInt &rhs);

    friend bool operator>(const BigInt &lhs, const BigInt &rhs);

    friend bool operator<=(const BigInt &lhs, const BigInt &rhs);

    friend bool operator>=(const BigInt &lhs, const BigInt &rhs);

    friend bool operator!=(const BigInt &lhs, const BigInt &rhs);

    BigInt& operator%=(const BigInt &rhs);

    BigInt &operator<<=(int size);

    BigInt &operator>>=(int size);

    void setSign(int new_sign);

    int getSign();

    friend void PrintTo(const BigInt &num, std::ostream *os);
    friend std::istream& operator>>(std::istream &is, BigInt &num);
    friend std::ostream& operator<<(std::ostream &os, const BigInt &num);
};

#endif //LABS_BIGINT_H
