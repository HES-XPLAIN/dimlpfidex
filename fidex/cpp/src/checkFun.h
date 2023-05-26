#ifndef CHECKFUNFid_H
#define CHECKFUNFid_H

#include <algorithm>
#include <iostream>
#include <string>

/**
 * @brief Check if the given character string represents a positive integer.
 *
 * @param str The character string to be checked.
 * @return int Returns 1 if the string represents a positive integer, 0 otherwise.
 */
int CheckPositiveInt(const char *str);

/**
 * @brief Check if the given character string represents a floating-point number.
 *
 * @param str The character string to be checked.
 * @return int Returns 1 if the string represents a floating-point number, 0 otherwise.
 */
int CheckFloatFid(const char *str);

/**
 * @brief Check if the given string is empty or consists only of whitespace characters.
 *
 * @param line The string to be checked.
 * @return bool Returns true if the string is empty or consists only of whitespace characters, false otherwise.
 */
bool checkStringEmpty(std::string line);

#endif
