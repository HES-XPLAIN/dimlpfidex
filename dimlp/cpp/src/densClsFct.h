#ifndef DENSCLSFCT_H
#define DENSCLSFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "bagDimlp.h"
#include "realHyp2.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

/**
 * @brief Displays the parameters for densCls.
 */
void showDensClsParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of densCls.
 */
void checkDensClsParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Dense Classification (densCls) process with specified parameters. This function obtains train and, optionally, test predictions and accuracy for a model trained with dimlpBT. It can also perform rule extraction with the Dimlp algorithm.
 */
int densCls(const std::string &command = "");

#endif // DENSCLSFCT_H
