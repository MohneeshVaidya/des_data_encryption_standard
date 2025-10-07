/**
 *  DATA ENCRYPTION STANDARD (DES)
 */
#include "util.hpp"

std::string encrypt(const Bits& txt_bits, const std::vector<Bits>& round_keys) {
    Bits txt_bits_64{ permute(txt_bits, ip_table, 64) };

    Bits left_32{ fetch_bits(txt_bits_64, 0, 32) };
    Bits right_32{ fetch_bits(txt_bits_64, 32, 64) };

    for (int i{ }; i < 16; ++i) {
        Bits tmp{ std::move(left_32) };

        left_32 = right_32;

        right_32 = exec_xor(tmp, exec_mangler(right_32, round_keys[i]), 32);
    }

    Bits combined{ bits_join(right_32, left_32) };

    combined = permute(combined, fp_table, 64);

    return bits_to_txt(combined);
}

int main() {
    std::string txt{ "1234567890abcdedAbcde!@#$%^&*()_+=-{}[];:'<>/?" };
    std::string key{ "1234AbcD" };

    txt = txt_preprocess(txt);
    key = txt_preprocess(key);

    std::cout << "The text: " << txt << "\n\n";
    std::cout << "The key : " << key << "\n\n";

    Bits txt_bits{ txt_to_bits(txt) };
    Bits key_bits{ txt_to_bits(key) };

    std::vector<Bits> round_keys{ generate_round_keys(key_bits) };

    std::string encrypted{ }; 

    int i{ };
    while (i < txt_bits.size()) {
        Bits txt_bits_64{ };

        for (int j{ }; j < 64; ++j) {
            txt_bits_64.push_back(txt_bits[i + j]);
        }

        encrypted += encrypt(txt_bits_64, round_keys);

        i += 64;
    }

    std::cout << "The encrypted text: " << encrypted << "\n\n";

    Bits encrypted_bits{ txt_to_bits(encrypted) };

    std::vector<Bits> round_keys_reversed(round_keys.rbegin(), round_keys.rend());

    std::string decrypted{ };
    
    i = 0;
    while (i < encrypted_bits.size()) {
        Bits txt_bits_64{ };

        for (int j{ }; j < 64; ++j) {
            txt_bits_64.push_back(encrypted_bits[i + j]);
        }

        decrypted += encrypt(txt_bits_64, round_keys_reversed);

        i += 64;
    }

    std::cout << "The decrypted text: " << decrypted << '\n';

    return 0;
}