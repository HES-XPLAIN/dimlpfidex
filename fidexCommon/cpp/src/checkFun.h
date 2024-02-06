#ifndef CHECKFUNFid_H
#define CHECKFUNFid_H

#include "errorHandler.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

bool checkInt(const char *str);
bool checkFloatFid(const char *str);
bool checkBool(const std::string &input);
bool checkList(const std::string &input);
bool checkStringEmpty(const std::string &line);
std::string formattingDoubleToString(double number);
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter);
std::vector<int> getIntVectorFromString(const std::string &str);
std::vector<double> getDoubleVectorFromString(const std::string &str);
std::tuple<std::vector<int>, bool, std::vector<double>, std::vector<double>> parseNormalizationStats(const std::string &normalizationFile, int nbAttributes, const std::vector<std::string> &attributes = std::vector<std::string>());
bool hasSpaceBetweenWords(const std::string &str);
bool exists(const std::string &path);
#endif
