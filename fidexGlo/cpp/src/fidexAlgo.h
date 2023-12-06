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

  bool fidex(Rule &rule,
             vector<vector<double>> *trainData,
             vector<int> *trainPreds,
             bool hasConfidence,
             vector<vector<double>> *trainOutputValuesPredictions,
             vector<int> *trainTrueClass,
             vector<double> *mainSampleValues,
             int mainSamplePred,
             Hyperspace *hyperspace,
             int nbIn,
             int nbAttributs,
             int itMax,
             int minNbCover,
             bool dropoutDim,
             double dropoutDimParam,
             bool dropoutHyp,
             double dropoutHypParam,
             mt19937 g) const;
};

#endif
