#ifndef CHECKFUNFid_H
#define CHECKFUNFid_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

int CheckPositiveInt(const char *str);
int CheckFloatFid(const char *str);
bool checkBool(std::string input);
bool checkStringEmpty(std::string line);
std::string formattingDoubleToString(double number);

#endif
