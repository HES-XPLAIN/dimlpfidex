#ifndef CHECKFUNFid_H
#define CHECKFUNFid_H

#include "errorHandler.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int CheckPositiveInt(const char *str);
int CheckFloatFid(const char *str);
bool checkBool(std::string input);
bool checkStringEmpty(std::string line);
std::string formattingDoubleToString(double number);
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter);
std::vector<int> getIntVectorFromString(const std::string &str);
std::vector<float> getFloatVectorFromString(const std::string &str);
#endif
