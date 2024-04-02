#ifndef FIDEXFCT_H
#define FIDEXFCT_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/parameters.h"
#include "fidexAlgo.h"
#include "hyperLocus.h"
#include "hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

void showFidexParams();
void launchFidex(Fidex &fidex, Parameters &params, Rule &rule, std::vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass);
void checkFidexParametersLogicValues(Parameters &p);
int fidex(const std::string &command);
void writeResultsFile(std::string const &filename, std::vector<std::tuple<int, Rule>> &sampleRuleDict, std::vector<std::string> attributesNames, std::vector<std::string> classNames);
void writeStatsFile(std::string const &filename, int nbTestSamples, double meanCovSize, double meanNbAntecedentsPerRule, double meanFidelity, double meanAccuracy, double meanConfidence);

#endif