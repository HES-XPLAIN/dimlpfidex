#ifndef DIMLPPREDFCT_H
#define DIMLPPREDFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "dimlp.h"
#include "dimlpCommonFun.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

/**
 * @brief Displays the parameters for dimlpPred.
 */
void showDimlpPredParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpPred.
 */
void checkDimlpPredParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Dimlp prediction process on test set with specified parameters for a model trained with dimlpTrn.
 */
int dimlpPred(const std::string &command = "");

#endif // DIMLPPREDFCT_H
