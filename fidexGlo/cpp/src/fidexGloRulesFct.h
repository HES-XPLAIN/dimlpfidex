#ifndef FIDEXGLORULESFCT_H
#define FIDEXGLORULESFCT_H

#include "../../../fidexCommon/cpp/src/checkFun.h"
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperLocus.h"
#include "../../../fidexCommon/cpp/src/parameters.h"
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
void checkParameters(Parameters p);
int fidexGloRules(const string &command);
tuple<double, double> writeRulesFile(string filename, const vector<Rule> &rules, const vector<string> &attributes, const vector<string> &classes);

#endif
