#ifndef _SAURON_UTILS_H_
#define _SAURON_UTILS_H_

#include <string>
#include <vector>

void convertAddress(const std::string &address, std::string &output);
void convertStringIPv6ToBSD(const std::string &address, std::string &output);
void convertStringIPv4ToBSD(std::string address, std::string &output);

#endif
