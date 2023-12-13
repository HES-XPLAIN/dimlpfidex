#ifndef FIDEXGLORULESFCT_H
#define FIDEXGLORULESFCT_H

#include "../../../fidexCommon/cpp/src/checkFun.h"
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperLocus.h"
#include "../../../fidexCommon/cpp/src/rule.h"
#include "fidexAlgo.h"
#include <algorithm>
#include <chrono>
#include <limits.h>
#include <numeric>
#include <omp.h>
#include <random>
#include <sstream>
#include <string>
#include <time.h>

using namespace std;
using namespace FidexGloNameSpace;
using namespace std::chrono;

void showRulesParams();
int fidexGloRules(const string &command);
tuple<int, int> writeRulesFile(string filename, const vector<Rule> rules, bool hasConfidence, const vector<string> *attributes = NULL, const vector<string> *classes = NULL);

#endif
