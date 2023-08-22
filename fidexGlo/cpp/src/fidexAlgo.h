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

class FidexAlgo {
public:
  //  Contructeur:
  FidexAlgo();

  //  Méthodes:

  bool fidex(std::tuple<std::vector<std::tuple<int, bool, double>>, std::vector<int>, int, double, double> &rule, std::vector<std::vector<double>> *trainData, std::vector<int> *trainPreds, bool hasConfidence, std::vector<std::vector<double>> *trainOutputValuesPredictions, std::vector<int> *trainTrueClass, std::vector<double> *mainSampleValues, int mainSamplePred, FidexGloNameSpace::Hyperspace *hyperspace, const int nbIn, const int nbAttributs, int itMax, int minNbCover, bool dropoutDim, double dropoutDimParam, bool dropoutHyp, double dropoutHypParam, std::mt19937 g) const;
};

#endif
