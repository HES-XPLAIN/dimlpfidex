#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/rule.h"
#include <fstream>
#include <iomanip>
#include <regex>
#include <set>
#include <sstream>
#include <string.h>

std::vector<std::vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot, bool verbose = true);
std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, DataSetFid &dataset);

#endif
