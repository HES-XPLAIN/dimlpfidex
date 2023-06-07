#ifndef DATASETFID_H
#define DATASETFID_H

#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "checkFun.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class DataSetFid {

  std::vector<std::vector<double>> datas;
  std::vector<int> trueClasses;
  std::vector<int> predictions;
  std::vector<std::vector<double>> outputValuesPredictions;
  std::vector<std::vector<double>> weights;
  bool hasDatas = false;
  bool hasClasses = false;
  bool hasWeights = false;

public:
  DataSetFid();
  DataSetFid(const char *dataFile, const char *predFile, const char *trueClassFile = nullptr);
  explicit DataSetFid(const char *weightFile);

  std::vector<std::vector<double>> *getDatas();
  std::vector<int> *getTrueClasses();
  std::vector<int> *getPredictions();
  std::vector<std::vector<double>> *getOutputValuesPredictions();
  std::vector<std::vector<double>> getWeights() const;
  std::vector<double> getInBiais() const;
  std::vector<double> getInWeights() const;
};

class Attribute {

  std::vector<std::string> attributes;
  bool hasAttributes = true;

public:
  Attribute();
  explicit Attribute(const char *attributFile);

  std::vector<std::string> *getAttributes();
};

#endif
