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

using namespace std;

vector<vector<double>> calcHypLocus(const char *dataFileWeights, int nbQuantLevels, double hiKnot, bool display = true);
vector<vector<double>> calcHypLocus(const char *rulesFile, const size_t nbAttributes, const vector<string> &attributeNames = {});

#endif
