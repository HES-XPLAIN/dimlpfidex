#ifndef FIDEXGLOFCT_H
#define FIDEXGLOFCT_H

#include "../../../fidex/cpp/src/fidexFct.h"
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperLocus.h"
#include "../../../fidexCommon/cpp/src/parameters.h"
#include "../../../fidexCommon/cpp/src/rule.h"
#include "fidexAlgo.h"
#include "getRulesFun.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

void showParams();
void checkParametersLogicValues(Parameters *p);
void launchFidex(std::vector<std::string> &lines, DataSetFid &dataset, Parameters &p, Hyperspace &hyperspace, vector<double> *mainSampleValues, int mainSamplePred, const vector<string> &attributeNames, const vector<string> &classNames);
int fidexGlo(const std::string &command);

#endif
