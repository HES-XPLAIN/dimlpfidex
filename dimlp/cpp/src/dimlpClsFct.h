#ifndef DIMLPCLSFCT_H
#define DIMLPCLSFCT_H

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
 * @brief Displays the parameters for dimlpCls.
 */
void showDimlpClsParams();

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpCls.
 */
void checkDimlpClsParametersLogicValues(Parameters &p);

void SaveFirstHid(DataSet &data, Dimlp *net, int nbHid, const std::string &outfile, const std::string &firsthidFile);
int dimlpCls(const std::string &command = "");

#endif
