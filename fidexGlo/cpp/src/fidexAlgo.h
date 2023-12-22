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
#include <math.h>
#include <numeric>
#include <random>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

using namespace std;
using namespace std::chrono;
using namespace FidexGloNameSpace;

class Fidex {
private:
  DataSetFid *_dataset;
  Parameters *_parameters;
  Hyperspace *_hyperspace;
  mt19937 _rnd;

  mt19937 generateRandom(int seed);

public:
  Fidex() = default;
  Fidex(DataSetFid *dataset, Parameters *parameters, Hyperspace *Hyperspace, int seed);

  // execute algo
  bool compute(Rule *rule, int idSample, float minFidelity);

  //  Méthodes:
  // bool fidex(Rule &rule,
  //            DataSetFid *dataset,
  //            Parameters *p,
  //            Hyperspace *hyperspace,
  //            int idSample,
  //            double minFidelity,
  //            mt19937 gen) const;
};

#endif
