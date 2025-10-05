#include "util.hpp"

std::ostream& operator<< (std::ostream& out, const Bits& bits) {
    std::string bits_str{ };

    for (Bit bit : bits) {
        bits_str.push_back(bit ? '1' : '0');
    }

    out << bits_str;

    return out;
}

/**
 * PRIVATE HELPER FUNCTIONS
 */
namespace {
    Bits char_to_bits(uint8_t ch) {
        Bits bits{ };

        uint8_t mask{ 0b10000000 };

        for (int i{ }; i < 8; ++i) {
            bits.push_back((ch & (mask >> i)) ? 1 : 0);
        }

        return std::move(bits);
    }

    uint8_t bits_to_char(const Bits& bits) {
        uint8_t ch{ };

        for (int i{ 7 }; i >= 0; --i) {
            if (bits[i]) {
                ch += static_cast<uint8_t>(std::pow(2, 8 - i - 1));
            }
        }
        return ch;
    }

    Bits rotate_left(const Bits& bits, int count) {
        Bits result{ };

        count = count % bits.size();

        for (int i{ }; i < bits.size(); ++i) {
            result.push_back(bits[(i + count) % bits.size()]);
        }

        return std::move(result);
    }

    Bits exec_s_box(const Bits& bits_6, const int s_box[4][16]) {
        std::string row{ };
        std::string col{ };

        row.push_back(bits_6[0] ? '1' : '0');
        row.push_back(bits_6[5] ? '1' : '0');

        col.push_back(bits_6[1] ? '1' : '0');
        col.push_back(bits_6[2] ? '1' : '0');
        col.push_back(bits_6[3] ? '1' : '0');
        col.push_back(bits_6[4] ? '1' : '0');

        std::unordered_map<std::string, int> table1{
            { "00", 0 },
            { "01", 1 },
            { "10", 2 },
            { "11", 3 },

            { "0000", 0 },
            { "0001", 1 },
            { "0010", 2 },
            { "0011", 3 },
            { "0100", 4 },
            { "0101", 5 },
            { "0110", 6 },
            { "0111", 7 },
            { "1000", 8 },
            { "1001", 9 },
            { "1010", 10 },
            { "1011", 11 },
            { "1100", 12 },
            { "1101", 13 },
            { "1110", 14 },
            { "1111", 15 },
        };

        std::unordered_map<int, Bits> table2{
            { 0 ,{ 0, 0, 0, 0 } },
            { 1 ,{ 0, 0, 0, 1 } },
            { 2 ,{ 0, 0, 1, 0 } },
            { 3 ,{ 0, 0, 1, 1 } },
            { 4 ,{ 0, 1, 0, 0 } },
            { 5 ,{ 0, 1, 0, 1 } },
            { 6 ,{ 0, 1, 1, 0 } },
            { 7 ,{ 0, 1, 1, 1 } },
            { 8 ,{ 1, 0, 0, 0 } },
            { 9 ,{ 1, 0, 0, 1 } },
            { 10 ,{ 1, 0, 1, 0 } },
            { 11 ,{ 1, 0, 1, 1 } },
            { 12 ,{ 1, 1, 0, 0 } },
            { 13 ,{ 1, 1, 0, 1 } },
            { 14 ,{ 1, 1, 1, 0 } },
            { 15 ,{ 1, 1, 1, 1 } },
        };

        return table2[s_box[table1[row]][table1[col]]];
    }
}

/**
 * PUBLIC FUNCTIONS
 */
Bits bits_join(const Bits& bits1, const Bits& bits2) {
    Bits bits(bits1.begin(), bits1.end());
    bits.append_range(bits2);

    return std::move(bits);
}

Bits permute(const Bits& bits, const int* permutation, int n) {
    Bits result{ };

    for (int i{ }; i < n; ++i) {
        result.push_back(bits[permutation[i] - 1]);
    }

    return std::move(result);
}

std::string txt_preprocess(std::string_view txt) {
    int32_t num{ static_cast<int32_t>(8 - (txt.length() % 8)) };
    
    std::string result{ txt };

    if (num < 8) {
        for (uint8_t i{ }; i < num; ++i) {
            result.push_back('.');
        }
    }

    return std::move(result);
}

Bits txt_to_bits(std::string_view txt) {
    Bits bits{ };

    for (uint8_t ch : txt) {
        bits.append_range(char_to_bits(ch));
    }

    return std::move(bits);
}

std::string bits_to_txt(const Bits& bits) {
    if (bits.size() % 8) {
        std::cerr << "Number of bits should be multiple of 8\n";
        std::exit(1);
    }

    std::string txt{ };

    for (int i{ }; i < bits.size(); i += 8) {
        txt.push_back(bits_to_char(fetch_bits(bits, i, i + 8)));
    }

    return std::move(txt);
}

Bits fetch_bits(const Bits& bits, int from, int to) {
    Bits result{ };

    for (int i{ from }; i < to; ++i) {
        result.push_back(bits[i]);
    }

    return std::move(result);
}

std::vector<Bits> generate_round_keys(const Bits& key) {
    std::vector<Bits> round_keys{ };

    Bits key_56{ permute(key, pc1_table, 56) };

    Bits left_28{ fetch_bits(key_56, 0, 28) };
    Bits right_28{ fetch_bits(key_56, 28, 56) };

    for (int i{ 1 }; i <= 16; ++i) {
        left_28 = rotate_left(left_28, (i == 1 || i == 2 || i == 9 || i == 16) ? 1 : 2);
        right_28 = rotate_left(right_28, (i == 1 || i == 2 || i == 9 || i == 16) ? 1 : 2);

        key_56 = bits_join(left_28, right_28);

        round_keys.push_back(permute(key_56, pc2_table, 48));
    }

    return std::move(round_keys);
}

Bits exec_xor(const Bits& bits1, const Bits& bits2, int n) {
    Bits result{ };

    for (int i{ }; i < n; ++i) {
        result.push_back((bits1[i] == bits2[i]) ? 0 : 1);
    }

    return std::move(result);
}

Bits exec_mangler(const Bits& bits, const Bits& key_48) {
    Bits bits_48{ permute(bits, e_box, 48) };

    bits_48 = exec_xor(bits_48, key_48, 48);

    Bits bits_32{ };

    for (int i{ }; i < 8; ++i) {
        Bits bits_6{ fetch_bits(bits_48, (6 * i), (6 * i) + 6) };

        bits_32.append_range(exec_s_box(bits_6, s_boxes[i]));
    }

    bits_32 = permute(bits_32, p_box, 32);

    return std::move(bits_32);
}