#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "checkFun.h"
#include "dataSet.h"
#include "errorHandler.h"
#include <fstream>
#include <set>
#include <sstream>
#include <string.h>

std::vector<std::vector<double>> calcHypLocus(const char *dataFileWeights, int nbQuantLevels, double hiKnot);
std::vector<std::vector<double>> calcHypLocus(const char *rulesFile, int nbAttributes);

#endif
