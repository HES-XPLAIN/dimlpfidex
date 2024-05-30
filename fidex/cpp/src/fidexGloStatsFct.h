#ifndef FIDEXGLOSTATSFCT_H
#define FIDEXGLOSTATSFCT_H

#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/parameters.h"
#include "../../../common/cpp/src/rule.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

/**
 * @brief Determines which samples are covered by a given rule.
 */
void getCovering(std::vector<int> &sampleIds, const Rule &rule, std::vector<std::vector<double>> &testValues);

/**
 * @brief Computes the number of true positives, false positives, true negatives, and false negatives based on the model's or rules's decision and the true class.
 */
void computeTFPN(int decision, int positiveClassIndex, int testTrueClass, int &nbTruePositive, int &nbFalsePositive, int &nbTrueNegative, int &nbFalseNegative);

/**
 * @brief Displays the parameters for fidexGloStats.
 */
void showStatsParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidexGloStats.
 */
void checkStatsParametersLogicValues(Parameters &p);

/**
 * @brief Computes the statistics of the global ruleset obtained from fidexGloRules on a test dataset.
 */
int fidexGloStats(const std::string &command = "");

#endif // FIDEXGLOSTATSFCT_H
