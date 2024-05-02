#ifndef FIDEXGLOFCT_H
#define FIDEXGLOFCT_H

#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/parameters.h"
#include "../../../common/cpp/src/rule.h"
#include "fidexAlgo.h"
#include "hyperLocus.h"
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
void checkParametersLogicValues(Parameters &p);
void executeFidex(std::vector<std::string> &lines, DataSetFid &dataset, Parameters &p, Hyperspace &hyperspace, std::vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass, const std::vector<std::string> &attributeNames, const std::vector<std::string> &classNames, int nbFidexRules);
int fidexGlo(const std::string &command = "");

#endif
