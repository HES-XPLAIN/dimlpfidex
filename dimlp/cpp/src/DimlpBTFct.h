#ifndef DIMLPBTFCT_H
#define DIMLPBTFCT_H

#include "../../../fidexCommon/cpp/src/parameters.h"
#include "bagDimlp.h"
#include "checkFun.h"
#include "realHyp2.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

void showDimlpBTParams();
void checkDimlpBTParametersLogicValues(Parameters &p);
int dimlpBT(const std::string &command);

#endif
