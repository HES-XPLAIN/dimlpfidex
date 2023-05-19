#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "checkFun.h"
#include "dataSet.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <time.h>

void showParams();
std::vector<std::vector<double>> calcHypLocus(int nbQuantLevels, double lowKnot, double hiKnot, std::vector<double> vectBias, std::vector<double> vectWeights);
int hyperLocus(const std::string &command);

#endif
