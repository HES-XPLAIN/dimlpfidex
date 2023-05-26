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
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

/**
 * Displays the Fidex parameters.
 */
void showFidexParams();

/**
 * Executes the Fidex algorithm based on the specified command.
 *
 * @param command The Fidex command to execute.
 * @return The exit code of the Fidex algorithm.
 */
int fidex(const std::string &command);

#endif
