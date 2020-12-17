//
// Created by heimdall on 31.10.18.
//

#include <algorithm>
#include <iterator>
#include "BigUint.h"
#include <iostream>

const BigUint BigUint::ZERO("0");
const BigUint BigUint::ONE("1");

BigUint::BigUint(const std::string &s) {
    string temp(base_len, '0');
    for (auto ch:s)
        if (isdigit(ch))
            temp.push_back(ch);
    for(int i = temp.size()-base_len; i >= 0; i -= base_len){
        data.push_back(std::stoi(temp.substr(i,base_len)));
    }
    removeZeros();
}


void BigUint::removeZeros() {
    if (data.empty()) {
        data.push_back(0); // TODO
    }
    while (data.size() > 1 && data.back() == 0)
        data.pop_back();
}

std::string BigUint::to_string() const {
    std::string result;
    result.append(std::to_string(data.back()));
    for(int i = data.size() - 2; i >= 0; i--) {
        std::string limb = std::to_string(data[i]);
        result.append(std::string(base_len - limb.length(), '0').append(limb));
    }
    return result;
}

bool operator==(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) == 0;
}

bool operator<(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) < 0;
}

bool operator>(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) > 0;
}

bool operator<=(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) <= 0;
}

bool operator>=(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) >= 0;
}

bool operator!=(const BigUint &lhs, const BigUint &rhs) {
    return lhs.compareTo(rhs) != 0;
}


BigUint &BigUint::operator<<=(int size) {
    size_t initial_size = data.size();
    data.resize(initial_size + size);
    std::copy(data.begin(), data.begin() + initial_size, data.begin() + size);
    std::fill(data.begin(), data.begin() + size, 0);
    return *this;
}

BigUint &BigUint::operator>>=(int size) {
    std::copy(data.begin() + size, data.end(), data.begin());
    data.resize(data.size() - size);
    removeZeros();
    return *this;
}


int BigUint::compareTo(const BigUint &other) const{
    if (data.size() != other.data.size()) {
        return (data.size() > other.data.size() ? 1 : -1);
    }

    for (auto lhs_iter = data.rbegin(), rhs_iter = other.data.rbegin();
         lhs_iter != data.rend() && rhs_iter != other.data.rend();
         lhs_iter++, rhs_iter++) {
        if (*lhs_iter != *rhs_iter) {
            return (*lhs_iter > *rhs_iter ? 1 : -1);
        }
    }
    return 0;
}





void divMod(BigUint lhs, BigUint rhs, BigUint &div, BigUint &mod) {
    if(rhs == 0){
        throw exception();
    }
    BigUint z = 1;
    int k = 0;
    while(lhs >= rhs){
        rhs <<= 1;
        z <<= 1;
        k++;
    }

    while(k > 0){
        rhs >>= 1;
        z >>= 1;
        k--;

        limb_t l = 0, r = BigUint::base, x = 0;
        while(l <= r){
            limb_t m = l + (r-l)/2;
            if(lhs >= rhs*m){
                x = m;
                l = m + 1;
            }else{
                r = m - 1;
            }
        }
        lhs = lhs - rhs * x;
        div = div + z * x;
    }
    mod = lhs;
}

BigUint operator/(const BigUint &lhs, const BigUint &rhs) {
    BigUint div, mod;
    divMod(lhs, rhs, div, mod);
    return div;
}



BigUint &BigUint::operator%=(BigUint rhs) {
    if(rhs.isZero()){
        throw exception();
    }
    BigUint z = 1;
    int k = 0;
    while(*this >= rhs){
        rhs <<= 1;
        z <<= 1;
        k++;
    }

    while(k > 0){
        rhs >>= 1;
        z >>= 1;
        k--;

        limb_t l = 0, r = BigUint::base, x = 0;
        while(l <= r){
            limb_t m = l + (r-l)/2;
            if(*this >= rhs*m){
                x = m;
                l = m + 1;
            }else{
                r = m - 1;
            }
        }
        *this = *this - rhs * x;
    }
    return *this;
}

BigUint operator%(const BigUint &lhs, const BigUint &rhs) {
    BigUint div, mod;
    divMod(lhs, rhs, div, mod);
    return mod;
}

void BigUint::multiply(const BigUint &lhs, const BigUint &rhs) {
    data.resize(lhs.data.size() + rhs.data.size());
    for (std::size_t i = 0; i < lhs.data.size(); i++) {
        limb_t carry = 0;
        for (std::size_t j = 0; carry || j < rhs.data.size(); j++) {
            limb_ll cur = data[i + j] + carry + limb_ll(lhs.data[i]) * (j < rhs.data.size() ? rhs.data[j] : 0);
            data[i + j] = limb_t (cur % BigUint::base);
            carry = limb_t(cur / BigUint::base);
        }
    }
    removeZeros();
}

BigUint operator*(const BigUint &lhs, const BigUint &rhs) {
    BigUint ans;
    ans.multiply(lhs, rhs);
    return ans;
}

BigUint operator+(const BigUint &lhs, const BigUint &rhs) {
    BigUint ans(lhs);
    int carry = 0;
    for (size_t i=0; i<max(ans.data.size(),rhs.data.size()) || carry; ++i) {
        if (i == ans.data.size())
            ans.data.push_back (0);
        ans.data[i] += carry + (i < rhs.data.size() ? rhs.data[i] : 0);
        carry = ans.data[i] >= BigUint::base;
        if (carry)
            ans.data[i] -= BigUint::base;
    }
    return ans;
}

BigUint operator-(const BigUint &lhs, const BigUint &rhs) {
    BigUint ans(lhs);
    limb_t carry = 0;
    for (size_t i=0, sz = rhs.data.size(); i<sz || carry; ++i) {
        ans.data[i] += BigUint::base;
        ans.data[i] -= carry + (i < sz ? rhs.data[i] : 0);
        carry = ans.data[i] < BigUint::base;
        if (!carry)
            ans.data[i] -= BigUint::base;
    }
    ans.removeZeros();
    return ans;
}

std::ostream &operator<<(std::ostream &os, const BigUint &num) {
    std::copy(num.data.rbegin(), num.data.rend(), std::ostream_iterator<limb_t>(os, "*"));
}

bool BigUint::isZero() const {
    return data.size() == 1 && data.front() == 0;
}


