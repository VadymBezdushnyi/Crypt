//
// Created by heimdall on 31.10.18.
//

#ifndef BigUint_PROJECT_BIGUNSIGNED_H
#define BigUint_PROJECT_BIGUNSIGNED_H

#include <vector>
#include <string>

typedef unsigned limb_t;
typedef unsigned long long limb_ll;

using namespace std;
class BigUint {
public:
    static const BigUint ZERO;
    static const BigUint ONE;

    static const limb_t base = 1'000'000'000;
    static const limb_t base_len = 9;

    std::vector<limb_t> data;

    BigUint() : BigUint(0) {}

    BigUint(const std::string &s);

    BigUint(int num) : BigUint(std::to_string(num)) {}

    BigUint(limb_t num) : BigUint(std::to_string(num)) {}

    BigUint(long long num) : BigUint(std::to_string(num)) {}

    bool isZero() const;

    void removeZeros();

    int len() const{
        return data.size() * base_len;
    }


    std::string to_string() const;
    explicit operator int() const{
        return std::stoi(this->to_string());
    }

public:
    void multiply(const BigUint &lhs, const BigUint &rhs);

    int compareTo(const BigUint &other) const;

    friend BigUint operator+(const BigUint &lhs, const BigUint &rhs);

    friend BigUint operator-(const BigUint &lhs, const BigUint &rhs);

    friend BigUint operator*(const BigUint &lhs, const BigUint &rhs);

    friend BigUint operator/(const BigUint &lhs, const BigUint &rhs);

    friend BigUint operator%(const BigUint &lhs, const BigUint &rhs);

    BigUint& operator%=(BigUint rhs);

    friend bool operator==(const BigUint &lhs, const BigUint &rhs);

    friend bool operator<(const BigUint &lhs, const BigUint &rhs);

    friend bool operator>(const BigUint &lhs, const BigUint &rhs);

    friend bool operator<=(const BigUint &lhs, const BigUint &rhs);

    friend bool operator>=(const BigUint &lhs, const BigUint &rhs);

    friend bool operator!=(const BigUint &lhs, const BigUint &rhs);

    BigUint &operator<<=(int size);

    BigUint &operator>>=(int size);

    friend std::ostream& operator<<(std::ostream &os, const BigUint &num);
};


#endif //BigUint_PROJECT_BIGUNSIGNED_H
