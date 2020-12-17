//
// Created by heimdall on 18.10.18.
//

#include <cassert>
#include "BigInt.h"

const BigInt BigInt::ZERO("0");
const BigInt BigInt::ONE("1");


BigInt::BigInt(const std::string &s) {
    sign = 1;
    if (s.empty())
        return;

    std::string::const_iterator head = s.begin();
    std::string q = s;
    if (s.front() == '-') {
        sign = -1;
        q = s.substr(1);
    }
    mag = BigUint(q);
    if(mag.isZero()){
        sign = 1;
    }
}


int BigInt::compareTo(const BigInt &other) const{
    /*
     * takes two numbers
     *
     * returns 1 if lhs > rhs
     * returns 0 if lhs == rhs
     * return -1 if lhs < rhs
     *
     */
    if (sign != other.sign) {
        return (sign > other.sign ? 1 : -1);
    }

    return mag.compareTo(other.mag) * sign;
}



void BigInt::addOrSub(const BigInt &lhs, const BigInt &rhs, bool substract) {
    // if substract == true we subtract data
    if ((lhs.sign == rhs.sign) ^ substract) {
        mag = lhs.mag + rhs.mag;
        setSign(lhs.sign);
        return;
    }

    // Why it works ?
    int new_rhs_sign = (substract ? rhs.sign : -rhs.sign);
    int cmp_value = lhs.mag.compareTo(rhs.mag);
    if(cmp_value == 1)
        mag = lhs.mag - rhs.mag;
    else
        mag = rhs.mag - lhs.mag;
    setSign(cmp_value * new_rhs_sign);
}

BigInt operator+(const BigInt &lhs, const BigInt &rhs) {
    BigInt res;
    res.addOrSub(lhs, rhs, false);
    return res;
}

BigInt operator-(const BigInt &lhs, const BigInt &rhs) {
    BigInt res;
    res.addOrSub(lhs, rhs, true);
    return res;
}


std::string BigInt::to_string() const {
    std::string result;
    if (sign == -1) result += "-";
    result.append(mag.to_string());
    return result;
}


void BigInt::setSign(int new_sign) {
    sign = (new_sign >= 0 ? 1 : -1);
}

int BigInt::getSign() {
    return sign;
}

void PrintTo(const BigInt &num, std::ostream *os) {
    *os << num.sign << "*" << num.mag;
}

bool operator==(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) == 0;
}

bool operator<(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) < 0;
}

bool operator>(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) > 0;
}

bool operator<=(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) <= 0;
}

bool operator>=(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) >= 0;
}

bool operator!=(const BigInt &lhs, const BigInt &rhs) {
    return lhs.compareTo(rhs) != 0;
}


BigInt operator*(const BigInt &lhs, const BigInt &rhs) {
    BigInt res;
    res.mag.multiply(lhs.mag, rhs.mag);
    res.setSign((lhs.isZero() || rhs.isZero()) ? 1 : lhs.sign * rhs.sign);
    return res;
}

bool BigInt::isZero() const {
    return sign == 1 && mag.isZero();
}

BigInt &BigInt::operator<<=(int size) {
    mag <<= size;
    return *this;
}

BigInt &BigInt::operator>>=(int size) {
    mag >>= size;
    return *this;
}

BigInt operator/(const BigInt &lhs, const BigInt &rhs) {
    BigInt res;
    res.mag = lhs.mag / rhs.mag;
    res.setSign((lhs.isZero() || rhs.isZero()) ? 1 : lhs.sign * rhs.sign); // TODO:check
    return res;
}

BigInt operator%(const BigInt &lhs, const BigInt &rhs) {
    BigInt res;
    res = lhs - (lhs/rhs)*rhs;
    return res;
}

std::ostream &operator<<(std::ostream &os, const BigInt &num) {
    os << (num.sign==1?"":"-") + num.mag.to_string();
    return os;
}

std::istream &operator>>(std::istream &is, BigInt &num) {
    std::string s;
    std::cin >> s;
    num = BigInt(s);
}

BigInt &BigInt::operator%=(const BigInt &rhs) {
    if(*this <= BigInt::ZERO || rhs <= BigInt::ZERO)
        *this  = *this % rhs;
    this->mag %= rhs.mag;
    return *this;
}