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

/**
 * @brief Checks if a given string represents a valid integer.
 */
bool checkInt(const std::string &input);

/**
 * @brief Checks if a given string represents a valid floating-point number.
 */
bool checkFloat(const std::string &input);

/**
 * @brief Checks if a given string is a valid representation of a boolean value.
 */
bool checkBool(const std::string &input);

/**
 * @brief Checks if a given string is in the format of a list of floating-point numbers.
 */
bool checkList(const std::string &input);

/**
 * @brief Checks if a given string is empty or contains only whitespace characters.
 */
bool checkStringEmpty(const std::string &line);

/**
 * @brief Converts a double value to a string and formats it by removing trailing zeros and decimal point if necessary.
 */
std::string formattingDoubleToString(double number);

/**
 * @brief Splits a given string into a vector of substrings based on a specified delimiter.
 */
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter);

/**
 * @brief Parses a string representing a list of integers and returns them as a vector.
 */
std::vector<int> getIntVectorFromString(std::string str);

/**
 * @brief Parses a string representing a list of doubles and returns them as a vector.
 */
std::vector<double> getDoubleVectorFromString(std::string str);

/**
 * @brief Parses a normalization file to extract statistical data.
 */
std::tuple<std::vector<int>, bool, std::vector<double>, std::vector<double>> parseNormalizationStats(const std::string &normalizationFile, int nbAttributes, const std::vector<std::string> &attributes = std::vector<std::string>());

/**
 * @brief Checks if a string contains a space between words.
 */
bool hasSpaceBetweenWords(const std::string &str);

/**
 * @brief Checks if a given file or directory is valid.
 */
bool exists(const std::string &path);

/**
 * @brief Gets the OS's default filesystem separator character.
 */
std::string getOSSeparator();

/**
 * @brief Prints an option and its description aligned at the specified width.
 */
void printOptionDescription(const std::string &option, const std::string &description, int width = 30);

/**
 * @brief Counts the number of networks in the weights file.
 */
int countNetworksInFile(const std::string &weightsFile);

/**
 * @brief Parses a line from a file and converts it into a vector of double values.
 */
std::vector<double> parseFileLine(std::string str, const std::string &fileName);

#endif // CHECKFUNFid_H
