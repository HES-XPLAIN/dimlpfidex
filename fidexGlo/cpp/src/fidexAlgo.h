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
#include <omp.h>
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

public:
  Fidex() = default;
  Fidex(DataSetFid &dataset, Parameters &parameters, Hyperspace &Hyperspace);

  // execute algo
  bool compute(Rule &rule, int idSample, double minFidelity, int minNbCover);
};

#endif
