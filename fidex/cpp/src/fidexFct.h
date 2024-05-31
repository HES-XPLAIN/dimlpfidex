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

/**
 * @brief Displays the parameters for fidex.
 */
void showFidexParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidex.
 */
void checkFidexParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Fidex algorithm with specified parameters to extract an explanation rule for one or several given samples.
 */
int fidex(const std::string &command = "");

#endif // FIDEXFCT_H
