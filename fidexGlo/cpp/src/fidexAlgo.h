#ifndef FIDEXALGO_H
#define FIDEXALGO_H
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/parameters.h"
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
             DataSetFid *dataset,
             Parameters *p,
             vector<vector<double>> *hyperlocus,
             int idSample,
             mt19937 gen) const;
};

#endif
