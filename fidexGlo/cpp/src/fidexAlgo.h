#ifndef FIDEXALGO_H
#define FIDEXALGO_H
#include "hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;
using namespace FidexGloNameSpace;

class FidexAlgo {
public:
  //  Contructeur:
  FidexAlgo();

  //  Méthodes:

  bool fidex(Rule &rule, std::vector<std::vector<double>> *trainData, std::vector<int> *trainPreds, std::vector<std::vector<double>> *trainOutputValuesPredictions, std::vector<int> *trainTrueClass, std::vector<double> *mainSampleValues, int mainSamplePred, FidexGloNameSpace::Hyperspace *hyperspace, const int nbIn, const int nbAttributs, int itMax, int minNbCover, double minFidelity, bool dropoutDim, double dropoutDimParam, bool dropoutHyp, double dropoutHypParam, std::mt19937 g) const;
};

#endif
