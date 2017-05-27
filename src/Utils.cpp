#include "Utils.h"

void convertStringIPv6ToBSD(const std::string &address, std::string &output) {
    int i = 0;
    int bCnt = 0;

    for (int i = 0; i < address.length(); i += 2) {
        if (address[i] == ':') {
            ++i;
            ++bCnt;
        }

        output += std::stoul(address.substr(i, 2), nullptr, 16);
    }
}

void convertStringIPv4ToBSD(std::string address, std::string &output) {
    address += '.';
    const int ffendPos = 10;
    const int ipNumSize = 4;

    /**
    * Conversion ipv4 -> ipv4 mapped ipv6
    */

    for (int i = 0; i < ffendPos; ++i) {
        output += static_cast<char>('\0');
    }
    output += static_cast<unsigned char>(255);
    output += static_cast<unsigned char>(255);

    std::vector<unsigned char> res;
    std::string s;

    for (int i = 0; i < address.length(); ++i) {
        if (address[i] == '.') {
            res.emplace_back(static_cast<unsigned char>(std::stoi(s)));
            s.clear();
        }
        else {
            s += address[i];
        }
    }

    for (int i = 0; i < ipNumSize; ++i) {
        output += res[i];
    }
}

void convertAddress(const std::string &address, std::string &output) {
    if (address.find(':') != std::string::npos) {
        convertStringIPv6ToBSD(address, output);
    } else {
        convertStringIPv4ToBSD(address, output);
    }
}
