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

/**
 * @brief Displays the parameters for fidexGlo.
 */
void showFidexGloParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidexGlo.
 */
void checkParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Fidex algorithm to extract an explanation rule for a given sample.
 */
void executeFidex(std::vector<std::string> &lines, DataSetFid &trainDataset, Parameters &p, Hyperspace &hyperspace, std::vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass, const std::vector<std::string> &attributeNames, const std::vector<std::string> &classNames);

/**
 * @brief Executes the FidexGlo algorithm with specified parameters to extract explanation rules for each test sample.
 */
int fidexGlo(const std::string &command = "");

#endif // FIDEXGLOFCT_H
