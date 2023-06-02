#include "stringI.h"
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////

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
