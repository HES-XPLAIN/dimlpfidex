#ifndef FIDEXGLORULESFCT_H
#define FIDEXGLORULESFCT_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/parameters.h"
#include "../../../common/cpp/src/rule.h"
#include "fidexAlgo.h"
#include "hyperLocus.h"
#include <algorithm>
#include <chrono>
#include <limits.h>
#include <numeric>
#include <omp.h>
#include <random>
#include <sstream>
#include <string>
#include <time.h>

void showRulesParams();
void checkRulesParametersLogicValues(Parameters &p);
int fidexGloRules(const std::string &command = "");

#endif
