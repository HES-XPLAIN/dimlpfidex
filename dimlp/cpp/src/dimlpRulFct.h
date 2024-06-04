#ifndef DIMLPRULFCT_H
#define DIMLPRULFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "dimlp.h"
#include "realHyp2.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unordered_map>
#include <vector>

/**
 * @brief Displays the parameters for dimlpRul.
 */
void showDimlpRulParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpRul.
 */
void checkDimlpRulParametersLogicValues(Parameters &p);
int dimlpRul(const std::string &command = "");

#endif
