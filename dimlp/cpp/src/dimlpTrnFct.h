#ifndef DIMLPTRNFCT_H
#define DIMLPTRNFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "dimlp.h"
#include "dimlpCommonFun.h"
#include "realHyp2.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>
#include <vector>

/**
 * @brief Displays the parameters for dimlpTrn.
 */
void showDimlpTrnParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpTrn.
 */
void checkDimlpTrnParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Dimlp training process with specified parameters and optionally performs rule extraction with the Dimlp algorithm.
 */
int dimlpTrn(const std::string &command = "");

#endif // DIMLPTRNFCT_H
