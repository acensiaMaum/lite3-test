#include "hex_parser.hpp"

uint32_t hexStringToUint32(const std::string& str) {
    std::string hex = str;
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')){
        hex = hex.substr(2);
    }

    if(!hex.empty() && (hex.back() == 'u' || hex.back() == 'U')){
        hex.pop_back();
    }

    uint32_t result = std::stoul(hex, nullptr, 16);

    return result;
}