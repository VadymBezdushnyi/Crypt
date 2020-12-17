//
// Created by heimdall on 08.11.18.
//

#include <cmath>
#include <set>
#include <map>
#include <Algorithms/BigIntMath.h>

namespace algo::math{

BigInt sqrt(BigInt num) {
    if (num < 0) throw exception();
    if (num == 0) return BigInt(0);

    BigInt curr = BigInt(1), next;
    curr <<= (num.len() + 1 / 2);// TODO: FIX
    int k = 0;
    while (true) {
        next = (curr + num / curr) / 2;
        if (next >= curr) {
            return curr;
        }
        curr = next;
    }
}

BigInt pow(BigInt base, BigInt power) {
    BigInt res = 1;
    while (power > 0) {
        if (power % 2 == 1) {
            res = res * base;
        }
        base = base * base;
        power = power / 2;
    }
    return res;
}

BigInt chineese(vector<pair<BigInt, BigInt>> nums) {
    size_t n = nums.size();

    vector<vector<BigInt>> rev(n, vector<BigInt>(n, BigInt(0)));
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            rev[i][j] = inverseMod(nums[i].second, nums[j].second);
        }
    }
    vector<BigInt> x(n);
    BigInt ans;
    BigInt coef = 1;
    for (int i = 0; i < n; i++) {
        x[i] = nums[i].first;
        for (int j = 0; j < i; j++) {
            x[i] = rev[j][i] * (x[i] - x[j]);
            x[i] = normMod(x[i], nums[i].second);
        }
    }
    for (int i = 0; i < n; i++) {
        ans = ans + x[i] * coef;
        coef = coef * nums[i].second;
    }
    ans = normMod(ans, coef);
    return ans;
}

BigInt gcdex(BigInt a, BigInt b, BigInt &x, BigInt &y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    BigInt x1, y1, d;
    d = gcdex(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

void sieve(int num, std::vector<int> &primes, std::vector<int> &least_prime) {
    /*
     * least_prime[i] - least prime divisor of i
     * primes[i] - i-th prime number
     */

    least_prime.assign(num + 1, 0);
    primes.clear();

    for (int i = 2; i <= num; i++) {
        if (least_prime[i] == 0) {
            primes.push_back(i);
            least_prime[i] = i;
        }
        for (int j = 0; j < (int) primes.size() && primes[j] <= least_prime[i] && i * primes[j] <= num; j++) {
            least_prime[i * primes[j]] = primes[j];
        }
    }
}

vector<int> getFirstKprimes(int k){
    vector<int> pr, lp;
    sieve(static_cast<int>(2*k * (log(k) + 1)), pr, lp);
    pr.resize(k);
    return pr;
}

BigInt abs(BigInt num) {
    return ((num > 0) ? num : -num);
}

int Jacobi_symbol(BigInt a, BigInt b) {
    if (a <= 0 || b % 2 == 0) return 0;
    int ans = 1;
    if (a < 0) {
        a = -a;
        if (b % 4 == 3) {
            ans *= -1;
        }
    }
    do {
        do {
            a = a / 2;
            if (abs(b % 8 - 4) == 1)
                ans *= -1;
        } while (a % 2 == 0);

        std::swap(a, b);
        if (a % 4 == 3 && b % 4 == 3) ans *= -1;
        a = a % b;
    } while (a != 0);
    if (b == 1)
        return ans;
    else
        return 0;
}

int legendre_symbol(BigInt a, BigInt p) {
    int ans = int(powMod(a, (p - 1) / 2, p));
    if(ans > 1) // ans == p - 1
        return -1;
    else
        return ans;
}


BigInt pollardRhoIteration(BigInt num, BigInt step) {
    /*
     * returns dividor of num
     */
    BigInt div = 1, one = 1;
    BigInt x_fixed = 1,  x = 2;
    int cycle = 2, count = 0;
    std::cerr << "Pollard Rho iteration: ";
    while (gcd(abs(x - x_fixed), num) == BigInt::ONE) {
        if(count == cycle){
            x_fixed = x;
            cycle *= 2;
            std::cerr << cycle  << " ";
        }
        x = normMod(x*x + step, num);
        count++;
    }
    std::cerr << std::endl;
    return gcd(num, abs(x - x_fixed));
}

BigInt pollardRho(BigInt num) {
    /*
     * returns prime dividor of num
     */
    for(int i = 1; i <= 7; i+=2){
        BigInt div = pollardRhoIteration(num, i);
        if(div != 1 && div != num)
            return div;
    }
    throw;
}




bool isPrimeDummy(BigInt num, int num_primes) {
    vector<int> primes, least_prime;

    for(int prime_num = 2; prime_num < num_primes && prime_num * prime_num <= num; prime_num++)
        if(num % prime_num == 0)
            return false;

    return true;
}


bool isPrimeFermat(BigInt num, int num_rounds) {
    vector<int> primes = getFirstKprimes(num_rounds);
    for(auto witness:primes){
        if(witness + 2 > num) break;
        if(powMod(witness, num - 1, num) != 1){
            return false;
        }
    }
    return true;
}

bool isPrimeMillerRabin(BigInt num, int num_rounds){
    int r = 0;
    BigInt d = num;
    while(d % 2 == 0){
        d = d / 2;
        r++;
    }
    vector<int> primes = getFirstKprimes(num_rounds);
    for(auto witness:primes){
        if(witness + 2 > num) break;
        BigInt x = powMod(witness, num-1, num);
        if(x == 1 || x == num - 1)
            continue;

        for(int i = 1; i < r; i++){
            x = mulMod(x,x,num);
            if(x + 1 == num)
                continue;
        }
        return false;
    }
    return true;
}



std::map<BigInt, int> factorize(BigInt num, bool dummy) {
    BigInt margin = sqrt(num);
    std::map<BigInt, int> ans;
    if(dummy) {
        for(int prime_num = 2; prime_num < 1000 && prime_num * prime_num <= num; prime_num++){
            if (num % prime_num == 0) {
                int cnt = 0;
                while (num % prime_num == 0) {
                    num = num / prime_num;
                    cnt++;
                }
                ans[prime_num] = ans[prime_num] + cnt;
            }
        }
    }
    if(num != 1) {
        if (isPrime(num)) {
            ans[num] = ans[num] + 1;
        } else {
            BigInt div = pollardRho(num);
            auto res1 = factorize(div, false);
            auto res2 = factorize(num / div, false);
            for (auto it:res1) ans[it.first] = ans[it.first] + it.second;
            for (auto it:res2) ans[it.first] = ans[it.first] + it.second;
        }
    }
    return ans;
}

bool isPrime(BigInt num) {
    int num_rounds = num.len() + 3;
    if(num.len() <= 4) return isPrimeDummy(num);
    if(!isPrimeDummy(num, num_rounds)) return false;
    if(!isPrimeMillerRabin(num, num_rounds)) return false;
    return true;
}


BigInt gcd(BigInt a, BigInt b) {
    while (a != BigInt::ZERO && b != BigInt::ZERO)
        if(a >= b)
            a %= b;
        else
            b %= a;
    return a + b;
}

BigInt inverseMod(BigInt num, BigInt mod) {
    BigInt x, y, g;
    g = gcdex(num, mod, x, y);
    if(g != 1){
        throw;
    }
    return normMod(x, mod);
}

BigInt powMod(BigInt base, BigInt power, BigInt mod) {
    BigInt res = 1;
    while(power > 0){
        if(power % 2 == 1){
            res = mulMod(res, base, mod);
        }
        base = mulMod(base, base, mod);
        power = power / 2;
    }
    return res;
}
BigInt logMod(BigInt base, BigInt ans, BigInt mod) {
    // Baby step giant step

    if(gcd(base, mod) != 1) return -1;

    BigInt block_size = sqrt(mod) + 1;
    BigInt giant_step = powMod(base, block_size, mod);

    // Giant step
    std::map<BigInt, BigInt> giant_steps;
    BigInt curr = giant_step;
    for(BigInt i = 1; i <= block_size; i = i + 1){
        if(!giant_steps.count(curr))
            giant_steps[curr] = i;
        curr = mulMod(curr, giant_step, mod);
    }

    // Small step
    curr = ans;
    for(BigInt i = 0; i <= block_size; i = i + 1){
        if(giant_steps.count(curr)){
            BigInt res = giant_steps[curr] * block_size - i;
            if(res < mod)
                return res;
        }
        curr = mulMod(curr, base, mod);
    }
    return -1;
}

BigInt normMod(BigInt num, BigInt mod) {
    num %= mod;
    if(num >= 0){
        return num;
    }else{
        num = num + mod;
        return num % mod;
    }
}

BigInt addMod(BigInt lhs, BigInt rhs, BigInt mod) {
    return normMod(normMod(lhs, mod) + normMod(rhs, mod), mod);
}

BigInt subMod(BigInt lhs, BigInt rhs, BigInt mod) {
    return normMod(lhs - rhs, mod);
}

BigInt mulMod(BigInt lhs, BigInt rhs, BigInt mod) {
    return normMod(lhs * rhs, mod);
}

BigInt divMod(BigInt lhs, BigInt rhs, BigInt mod) {
    return normMod(normMod(lhs, mod) * inverseMod(normMod(rhs, mod), mod), mod);
}

BigInt euler(BigInt num) {
    std::map<BigInt, int> f = factorize(num);
    for(auto it:f){
        num = num - num / it.first;
    }
    return num;
}

int mobius(BigInt num) {
    std::map<BigInt, int> f = factorize(num);
    int res = (f.size() % 2 ? -1 : 1);
    for(auto it:f){
        if(it.second > 1)
            res = 0;
    }
    return res;
}

BigInt GenNextPrime(BigInt num) {
    for(int i = 0; i < 10000; i++) {
        num = num + BigInt{1};
        if(isPrime(num)) {
            return num;
        }
    }
    throw std::runtime_error("Couldn't find next prime");
}

}