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
  std::string datasetName;
  std::vector<std::vector<double>> datas;
  std::vector<int> trueClasses;
  std::vector<int> predictions;
  std::vector<std::vector<double>> outputValuesPredictions;
  std::vector<std::vector<double>> weights;
  bool hasDatas = false;
  bool hasPreds = false;
  bool hasClasses = false;
  bool hasWeights = false;

  int nbClasses = -1;
  int nbPreds = -1;
  int nbAttributes = -1;
  int nbSamples = -1;
  int nbClassData = -1;
  int nbPredData = -1;

  void setDataLine(const std::string &line, const char *dataFile);
  void setPredLine(const std::string &line, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *dataFile);
  void setClassLine(const std::string &line, const char *dataFile);

  void checkDatas() const;

  std::vector<std::string> attributeNames;
  std::vector<std::string> classNames;
  bool hasAttributes = false;
  bool hasClassNames = false;

public:
  explicit DataSetFid(const std::string &name) : datasetName(name){};
  DataSetFid(const std::string &name, const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile = nullptr);
  DataSetFid(const std::string &name, const char *dataFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass); // dataFile with data, predictions and maybe classes
  explicit DataSetFid(const std::string &name, const char *weightFile);

  void setDataFromFile(const char *dataFile);
  void setPredFromFile(bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *predFile);
  void setClassFromFile(const char *classFile);

  std::vector<std::vector<double>> *getDatas();
  std::vector<int> *getClasses();
  bool getHasClasses() const;
  std::vector<int> *getPredictions();
  std::vector<std::vector<double>> *getOutputValuesPredictions();
  int getNbClasses() const;
  int getNbAttributes() const;
  int getNbSamples() const;

  void setAttribute(const char *attributFile);
  std::vector<std::string> *getAttributeNames();
  std::vector<std::string> *getClassNames();
  bool getHasClassNames() const;

  std::vector<std::vector<double>> getWeights() const;
  std::vector<double> getInBiais() const;
  std::vector<double> getInWeights() const;
};

#endif
