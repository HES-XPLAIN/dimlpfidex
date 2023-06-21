#ifndef MISC_H
#define MISC_H
#include "stringI.h"
#include <vector>

int Compare(const void *x, const void *y);
int AreSameClass(std::shared_ptr<StringInt> listPat, std::vector<int> classPatNet);

#endif
