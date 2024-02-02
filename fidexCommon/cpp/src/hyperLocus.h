#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "checkFun.h"
#include "dataSet.h"
#include "errorHandler.h"
#include <fstream>
#include <iomanip>
#include <regex>
#include <set>
#include <sstream>
#include <string.h>

std::vector<std::vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot, bool display = true);
std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, const int nbAttributes, const std::vector<std::string> &attributeNames = {});

#endif
