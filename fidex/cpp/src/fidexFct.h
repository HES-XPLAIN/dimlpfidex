#ifndef FIDEXFCT_H
#define FIDEXFCT_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/hyperLocus.h"
#include "../../../common/cpp/src/parameters.h"
#include "../../../fidexGlo/cpp/src/fidexAlgo.h"
#include "../../../fidexGlo/cpp/src/hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

void showFidexParams();
void checkFidexParametersLogicValues(Parameters &p);
int fidex(const std::string &command);

#endif
