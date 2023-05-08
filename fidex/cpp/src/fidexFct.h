#ifndef FIDEXFCT_H
#define FIDEXFCT_H

#include "checkFun.h"
#include "dataSet.h"
#include "hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

static void showParams();
int fidex(string command);
bool checkStringEmpty(string line);

#endif
