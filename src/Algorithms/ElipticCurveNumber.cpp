//
// Created by heimdall on 27.11.18.
//

#include "ElipticCurveNumber.h"
#include "BigIntMath.h"


ElipticCurveNumber::ElipticCurveNumber(const ElipticCurve &curve, const BigInt &x, const BigInt &y)
        : curve(curve), x(x), y(y) {

}

ElipticCurveNumber operator+(const ElipticCurveNumber &a, const ElipticCurveNumber &b) {
    ElipticCurveNumber c(a.curve);
    if (a.x == b.x && a.y != b.y) {
        return c;
    }
    if (a.x.isZero() && a.y.isZero()) {
        return b;
    }
    if (b.x.isZero() && b.y.isZero()) {
        return a;
    }
    BigInt slope = (
            a == b ?
            (3 * a.x * a.x + a.curve.a) * algo::math::inverseMod(2 * a.y, a.curve.p) % a.curve.p :
            (a.y + a.curve.p - b.y) * algo::math::inverseMod(a.x + a.curve.p - b.x, a.curve.p) % a.curve.p
    );
    c.x = (slope * slope + ((2 * a.curve.p - a.x) - b.x)) % a.curve.p;
    c.y = (slope * (a.x + a.curve.p - c.x) + a.curve.p - a.y) % a.curve.p;
    return c;
}

std::ostream &operator<<(std::ostream &cout, const ElipticCurveNumber &number) {
    return cout << "(" << number.x << "; " << number.y << ")";
}

bool operator==(const ElipticCurveNumber &a, const ElipticCurveNumber &b) {
    return a.x == b.x && a.y == b.y;
}

ElipticCurveNumber operator*(const ElipticCurveNumber &a, int b) {
    if (b == 1) {
        return a;
    }
    if (b == 0) {
        return ElipticCurveNumber(a.curve);
    }
    ElipticCurveNumber c = (a*(b>>1));
    if (b & 1) {
        return c + c + a;
    }
    return c + c;
}

ElipticCurveNumber operator*(const ElipticCurveNumber &a, BigInt b) {
    if (b == BigInt::ONE) {
        return a;
    }
    if (b == BigInt::ZERO) {
        return ElipticCurveNumber(a.curve);
    }
    ElipticCurveNumber c(a.curve);
    ElipticCurveNumber A = a;
    while(!b.isZero()) {
        if (b % 2 == 1) {
            c = (c+A);
        }
        A = (A+A);
        b = b / 2;
    }
    return c;
}

ElipticCurveNumber ElipticCurveNumber::inverse() const {
    return ElipticCurveNumber(curve, x, (curve.p-y)%curve.p);
}

bool operator<(const ElipticCurveNumber &a, const ElipticCurveNumber &b) {
    return a.x < b.x;
}