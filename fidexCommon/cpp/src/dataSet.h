#ifndef DATASETFID_H
#define DATASETFID_H

#include "checkFun.h"
#include "errorHandler.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class DataSetFid {

  vector<vector<double>> datas;
  vector<int> trueClasses;
  vector<int> predictions;
  vector<vector<double>> outputValuesPredictions;
  vector<vector<double>> weights;
  bool hasDatas = false;
  bool hasClasses = false;
  bool hasWeights = false;
  bool everyPredIsBool = true; // If every prediction is boolean, then there is no interest in computing confidence, it will always be 1

public:
  DataSetFid() = default;
  DataSetFid(const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile = nullptr);
  explicit DataSetFid(const char *weightFile);

  vector<vector<double>> *getDatas();
  vector<int> *getTrueClasses();
  vector<int> *getPredictions();
  vector<vector<double>> *getOutputValuesPredictions();
  bool hasConfidence() const;
  int getNbClasses() const;
  vector<vector<double>> getWeights() const;
  vector<double> getInBiais() const;
  vector<double> getInWeights() const;
};

class Attribute {

  vector<string> attributes;
  bool hasAttributes = true;

public:
  Attribute();
  explicit Attribute(const char *attributFile);

  vector<string> *getAttributes();
};

#endif
