#ifndef FIDEXGLOSTATSFCT_H
#define FIDEXGLOSTATSFCT_H

#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/parameters.h"
#include "../../../common/cpp/src/rule.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

void showStatsParams();
void checkStatsParametersLogicValues(Parameters &p);
int fidexGloStats(const std::string &command = "");

#endif
