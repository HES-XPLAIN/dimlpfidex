#ifndef DIMLPBTFCT_H
#define DIMLPBTFCT_H

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
 * @brief Displays the parameters for dimlpBT.
 */
void showDimlpBTParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpBT.
 */
void checkDimlpBTParametersLogicValues(Parameters &p);

/**
 * @brief Executes the Dimlp Bagging Training (dimlpBT) process with specified parameters and optionally performs rule extraction with the Dimlp algorithm.
 */
int dimlpBT(const std::string &command = "");

#endif // DIMLPBTFCT_H
