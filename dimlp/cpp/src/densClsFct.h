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
int densCls(const std::string &command = "");

#endif // DENSCLSFCT_H
