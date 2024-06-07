#ifndef MISC_H
#define MISC_H
#include "../../../common/cpp/src/stringI.h"
#include <vector>

/**
 * @brief Compares two float values.
 */
int Compare(const void *x, const void *y);

/**
 * @brief Checks if all patterns in the list belong to the same class.
 */
int AreSameClass(std::shared_ptr<StringInt> listPat, std::vector<int> classPatNet);

#endif // MISC_H
