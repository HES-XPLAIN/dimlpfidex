#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "checkFun.h"
#include "dataSet.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <time.h>

static void showParams();
static vector<vector<double>> calcHypLocus(int nbQuantLevels, double lowKnot, double hiKnot, vector<double> vectBias, vector<double> vectWeights);
int hyperLocus(string command);

#endif
