#ifndef FIDEXGLOSTATSFCT_H
#define FIDEXGLOSTATSFCT_H

#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "getRulesFun.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

void showStatsParams();
int fidexGloStats(const std::string &command);

#endif
