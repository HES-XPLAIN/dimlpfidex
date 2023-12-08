#ifndef FIDEXALGO_H
#define FIDEXALGO_H
#include "../../../fidexCommon/cpp/src/dataSet.h"
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
             DataSetFid dataset,
             Hyperspace *hyperspace,
             int idSample,
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
