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

private:
  std::vector<std::vector<double>> datas;
  std::vector<int> trueClasses;
  std::vector<int> predictions;
  std::vector<std::vector<double>> outputValuesPredictions;
  std::vector<std::vector<double>> weights;
  bool hasDatas = false;
  bool hasClassesAttr = false;
  bool hasWeights = false;
  bool everyPredIsBool = true; // If every prediction is boolean, then there is no interest in computing confidence, it will always be 1

  void getDataLine(const std::string &line, const char *dataFile);
  void getPredLine(const std::string &line, std::vector<double> &valuesPred, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *dataFile);
  void getClassLine(const std::string &line, const char *dataFile, int &nbClasses);

public:
  DataSetFid();
  DataSetFid(const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile = nullptr);
  DataSetFid(const char *dataFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass); // dataFile with data, predictions and maybe classes
  explicit DataSetFid(const char *weightFile);

  std::vector<std::vector<double>> *getDatas();
  std::vector<int> *getTrueClasses();
  bool hasClasses() const;
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
