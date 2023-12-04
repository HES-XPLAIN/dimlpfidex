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

class DataSetFid {

  std::vector<std::vector<double>> datas;
  std::vector<int> trueClasses;
  std::vector<bool> hasTrueClasses; // Maybe sometimes we have a file with samples with classes and samples without classes
  std::vector<int> predictions;
  std::vector<std::vector<double>> outputValuesPredictions;
  std::vector<std::vector<double>> weights;
  bool hasDatas = false;
  bool hasClasses = false;
  bool hasWeights = false;
  bool everyPredIsBool = true; // If every prediction is boolean, then there is no interest in computing confidence, it will always be 1

public:
  DataSetFid();
  DataSetFid(const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile = nullptr);
  explicit DataSetFid(const char *weightFile);

  std::vector<std::vector<double>> *getDatas();
  std::vector<int> *getTrueClasses();
  std::vector<bool> *getHasTrueClasses();
  std::vector<int> *getPredictions();
  std::vector<std::vector<double>> *getOutputValuesPredictions();
  bool hasConfidence() const;
  int getNbClasses() const;
  std::vector<std::vector<double>> getWeights() const;
  std::vector<double> getInBiais() const;
  std::vector<double> getInWeights() const;
};

class Attribute {

  std::vector<std::string> attributeNames;
  std::vector<std::string> classNames;
  bool hasAttributes = true;
  bool hasClassNames = false;

public:
  Attribute();
  explicit Attribute(const char *attributFile, int nbAttrbuts, int nbClass);

  std::vector<std::string> *getAttributeNames();
  std::vector<std::string> *getClassNames();
};

#endif
