#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "tables.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cmath>

using Bit  = uint8_t;
using Bits = std::vector<Bit>;

std::ostream& operator<< (std::ostream& out, const Bits& bits);

Bits bits_join(const Bits& bits1, const Bits& bits2);
Bits permute(const Bits& bits, const int* permutation, int n);
std::string txt_preprocess(std::string_view txt);
Bits txt_to_bits(std::string_view txt);
std::string bits_to_txt(const Bits& bits);
Bits fetch_bits(const Bits& bits, int from, int to);
std::vector<Bits> generate_round_keys(const Bits& key);
Bits exec_xor(const Bits& bits1, const Bits& bits2, int n);
Bits exec_mangler(const Bits& bits, const Bits& key_48);

#endif