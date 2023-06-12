#ifndef FIDEXGLORULESFCT_H
#define FIDEXGLORULESFCT_H

#include "../../../fidexCommon/cpp/src/checkFun.h"
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperLocus.h"
#include "fidexAlgo.h"
#include <algorithm>
#include <chrono>
#include <limits.h>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <time.h>

void showRulesParams();
int fidexGloRules(const std::string &command);

#endif
