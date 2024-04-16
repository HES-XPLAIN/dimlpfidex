#ifndef CHECKFUNFid_H
#define CHECKFUNFid_H

#include "errorHandler.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <tuple>
#include <vector>

bool checkInt(const std::string &input);
bool checkFloat(const std::string &input);
bool checkBool(const std::string &input);
bool checkList(const std::string &input);
bool checkStringEmpty(const std::string &line);
std::string formattingDoubleToString(double number);
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter);
std::vector<int> getIntVectorFromString(std::string str);
std::vector<double> getDoubleVectorFromString(std::string str);
std::tuple<std::vector<int>, bool, std::vector<double>, std::vector<double>> parseNormalizationStats(const std::string &normalizationFile, int nbAttributes, const std::vector<std::string> &attributes = std::vector<std::string>());
bool hasSpaceBetweenWords(const std::string &str);
bool exists(const std::string &path);
std::string getOSSeparator();
void printOptionDescription(const std::string &option, const std::string &description, int width = 30);
int countNetworksInFile(const std::string &weightsFile);
std::vector<double> parseFileLine(std::string str, const std::string &fileName);
#endif
