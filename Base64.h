#ifndef BASE64_H // BASE64 encryption
#define BASE64_H

#include <string>
#include <vector>

// Creating a new namespace for base64 encoding
namespace Base64 {
    std::string  base64_encode (const std::string &);

    // Salt is random data that is used as an
    // additional input to a one-way function
    // that "hashes" a password or passphrase.
    const std::string &SALT1 = "LM::TB::BB";
    const std::string &SALT2 = "_:/_77";
    const std::string &SALT3 = "line=wowC++";

    std::string Encrypt64(std::string s){
        s = SALT1 + s + SALT2 + SALT3; // Create randomized salt string

        s = base64_encode(s);                // String Manipulations
        s.insert(7, SALT3);                       // Store SALT3 string into the 7th
                                                            // position of the s string
        s += SALT1;                                // More random BS
        s = base64_encode (s);
        s = SALT2 + SALT3 + SALT1;
        s = base64_encode(s);
        s.insert(1, "L");
        s.insert(7, "M");
        return s;
    }

    // All alphanumeric characters
    const std::string &BASE64_CODES  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string base64_encode (const std::string &s){
        std::string ret;
        int val = 0;
        int bits = -6;
        const unsigned int b63 = 0x3F;

        // for each loop is new for C++11
        for (const auto &c : s){
            val = (val << 8) + c;      // Bitwise left shift by 8 bits, then add c to the result
                                                // Store it back into val
            bits += 8;                     // 8 bits for each character

            while (bits >= 0){
                ret.push_back(BASE64_CODES[(val >> bits) & b63]);
                bits -=6;                   // Gather the bits in groups of 6
            }
        }

        // If the bits have been pushed back more than 6
        if ( bits > -6)
            ret.push_back(BASE64_CODES[((val << 8)  >> (bits + 8)) & b63]);

        while ( ret.size()%4 ) // true if >0; false if < 0
            ret.push_back( '=' );

        return ret;
    }
}

#endif  // BASE64
