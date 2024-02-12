#ifndef DENSCLSFCT_H
#define DENSCLSFCT_H

#include "../../../fidexCommon/cpp/src/parameters.h"
#include "bagDimlp.h"
#include "realHyp2.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

void showDensClsParams();
void checkDensClsParametersLogicValues(Parameters &p);
int densCls(const std::string &command);

#endif
