
#include <vector>
#include <string>
#include <ios>
#include <iostream>
#include <iomanip>

namespace utils{


template<typename T = uint8_t>
std::vector<T> ToVec(const std::string& s) {
    std::vector<T> vec(s.begin(), s.end());
    return vec;
}

uint8_t HexCharToNum(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if(c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if(c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

template<typename T = uint8_t>
std::vector<T> HexToVec(const std::string& s) {
    if(s.length() % 2 != 0) {
        throw std::logic_error("String length must be even");
    }
    std::vector<T> vec;
    for(size_t i = 0; i < s.size();i+=2) {
        vec.push_back(HexCharToNum(s[i]) * 16);
        vec.back() += HexCharToNum(s[i+1]);
    }
    return vec;
}

std::string ToStr(const std::vector<uint8_t>& data) {
    return std::string(data.begin(), data.end());
}

std::string ToHex(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    for(size_t i = 0; i < data.size(); i++) {
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint32_t>(data[i]);
    }
    return ss.str();
}

void AddZeroPadding(std::vector<uint8_t>& vec, size_t block_size) {
    while(vec.size() % block_size != 0) {
        vec.push_back(0);
    }
}

size_t CeilNumber(size_t number, size_t div) {
    return ((number + div - 1) / div) * div;
}

}