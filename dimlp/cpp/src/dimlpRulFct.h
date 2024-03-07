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

void showDimlpRulParams();
void checkDimlpRulParametersLogicValues(Parameters &p);
int dimlpRul(const std::string &command = "");

#endif
