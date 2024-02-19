#ifndef FIDEXALGO_H
#define FIDEXALGO_H
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/parameters.h"
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

using namespace std::chrono;

class Fidex {

public:
  // Struct to handle special parameters
  struct currentExecutionSpecs {
    bool showInitialFidelity = false;
    double mainSamplePredValue = -1.0;
    int nbIt = 0;
  };

private:
  DataSetFid *_trainDataset;
  Parameters *_parameters;
  Hyperspace *_hyperspace;
  mt19937 _rnd;
  currentExecutionSpecs specs;

  // utils

  bool tryComputeFidex(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, double mainSamplePredValue, bool verbose, bool detailedVerbose = false, bool foundRule = false);
  int dichotomicSearch(Rule &bestRule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int mainSampleClass, double mainSamplePredValue, int left, int right, bool verbose);
  bool retryComputeFidex(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, double mainSamplePredValue, int maxFailedAttempts, bool hasDropout, bool verbose);

public:
  Fidex() = default;
  Fidex(DataSetFid &_trainDataset, Parameters &parameters, Hyperspace &Hyperspace);

  // execute algo
  bool compute(Rule &rule, bool withTestSample, vector<double> &mainSampleValues, int mainSamplePred, double minFidelity, int minNbCover, int mainSampleClass = -1);

  // Setters
  void setShowInitialFidelity(bool value) {
    specs.showInitialFidelity = value;
  }

  void setMainSamplePredValue(double value) {
    specs.mainSamplePredValue = value;
  }

  void setNbIt(int value) {
    specs.nbIt = value;
  }

  // Getters
  bool getShowInitialFidelity() const {
    return specs.showInitialFidelity;
  }

  double getMainSamplePredValue() const {
    return specs.mainSamplePredValue;
  }

  int getNbIt() const {
    return specs.nbIt;
  }

  void resetExecutionSpecs() {
    specs.showInitialFidelity = false;
    specs.mainSamplePredValue = -1.0;
  }

  // utils
  void launchFidex(Parameters &params, Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass, bool verbose = false);
};

#endif
