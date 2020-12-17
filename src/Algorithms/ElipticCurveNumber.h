//
// Created by heimdall on 27.11.18.
//

#ifndef BIGINT_PROJECT_ELIPTICCURVENUMBER_H
#define BIGINT_PROJECT_ELIPTICCURVENUMBER_H

#include "ElipticCurve.h"
#include <BigInt/BigInt.h>

class ElipticCurveNumber {
    ElipticCurveNumber(const ElipticCurve & curve,
                       const BigInt & x = BigInt::ZERO,
                       const BigInt & y = BigInt::ZERO);
    ElipticCurve curve;
    BigInt x, y;
public:
    friend ElipticCurveNumber operator+(const ElipticCurveNumber & a, const ElipticCurveNumber & b);
    friend ElipticCurveNumber operator*(const ElipticCurveNumber & a, int b);
    friend ElipticCurveNumber operator*(const ElipticCurveNumber & a, BigInt b);

    friend bool operator==(const ElipticCurveNumber & a, const ElipticCurveNumber & b);
    friend bool operator<(const ElipticCurveNumber & a, const ElipticCurveNumber & b);
    friend bool operator!=(const ElipticCurveNumber & a, const ElipticCurveNumber & b);

    friend std::ostream &operator<<(std::ostream & cout, const ElipticCurveNumber & number);

    ElipticCurveNumber inverse() const;
};


#endif //BIGINT_PROJECT_ELIPTICCURVENUMBER_H
