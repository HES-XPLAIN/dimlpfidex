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

/**
 * @brief Displays the parameters for fidexGloRules.
 */
void showRulesParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidexGloRules.
 */
void checkRulesParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Fidex algorithm on all provided samples.
 */
void generateRules(std::vector<Rule> &rules, std::vector<int> &notCoveredSamples, DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus);

/**
 * @brief Generates a list of rules covering all training samples using the best and slowest heuristic.
 */
std::vector<Rule> heuristic_1(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus);

/**
 * @brief Generates a list of rules covering all training samples using a faster but less optimal heuristic.
 */
std::vector<Rule> heuristic_2(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus);

/**
 * @brief Generates a list of rules covering all training samples. This is the fastest heuristic but cannot be parallelized.
 */
std::vector<Rule> heuristic_3(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus);

/**
 * @brief Executes the FidexGloRules algorithm with specified parameters to extract a ruleset from a dataset of training samples.
 */
int fidexGloRules(const std::string &command = "");

#endif // FIDEXGLORULESFCT_H
