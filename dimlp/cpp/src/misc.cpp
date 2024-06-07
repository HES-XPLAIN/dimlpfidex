#include "../../../common/cpp/src/stringI.h"
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////

/**
 * @brief Compares two float values.
 *
 * @param x Pointer to the first float value.
 * @param y Pointer to the second float value.
 * @return 1 if the first value is greater, -1 if the first value is smaller, 0 if they are equal.
 */
int Compare(const void *x, const void *y)

{
  auto *a = (const float *)x;
  auto *b = (const float *)y;

  if (*a > *b)
    return 1;

  if (*a < *b)
    return -1;

  return 0;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if all patterns in the list belong to the same class.
 *
 * @param listPat Shared pointer to the list of patterns.
 * @param classPatNet Vector of class labels for the patterns.
 * @return 1 if all patterns belong to the same class, 0 otherwise.
 */
int AreSameClass(std::shared_ptr<StringInt> listPat, std::vector<int> classPatNet)

{
  int p;
  int nbPat = listPat->GetNbEl();

  listPat->GoToBeg();
  int cl = classPatNet[listPat->GetVal()];

  for (p = 1, listPat->GoToNext(); p < nbPat; p++, listPat->GoToNext())
    if (cl != classPatNet[listPat->GetVal()])
      break;

  if (p == nbPat)
    return 1;

  return 0;
}

////////////////////////////////////////////////////////////////////////
