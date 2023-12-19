#ifndef DATASETFID_H
#define DATASETFID_H

#include "checkFun.h"
#include "errorHandler.h"
#include <algorithm>
#include <cmath>
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

  double decisionThreshold = -1;
  int indexPositiveClass = -1;

  int nbClasses = -1;
  int nbAttributes = -1;
  int nbSamples = -1;
  int nbClassData = -1;
  int nbPredData = -1;

  void setDataLine(const std::string &line, const char *dataFile);
  void setPredLine(const std::string &line, const char *dataFile);
  void setClassLine(const std::string &line, const char *dataFile);

  void setNbClass(int nb_classes);
  void setNbClassAndAttr(int _nbClasses, int _nbAttributes);

  void checkDatas();
  void checkThreshold() const;

  std::vector<std::string> attributeNames;
  std::vector<std::string> classNames;
  bool hasAttributes = false;
  bool hasClassNames = false;

public:
  explicit DataSetFid(const std::string &name) : datasetName(name){};
  DataSetFid(const std::string &name, const char *dataFile, const char *predFile, int _nbAttributes, int _nbClasses, double decisionThresh, int indexPositiveCl, const char *trueClassFile = nullptr);
  DataSetFid(const std::string &name, const char *dataFile, int _nbAttributes, int _nbClasses, double decisionThresh, int indexPositiveCl); // dataFile with data, predictions and maybe classes
  explicit DataSetFid(const std::string &name, const char *weightFile);

  void setDataFromFile(const char *dataFile, int _nbAttributes, int _nbClasses);
  void setPredFromFile(const char *predFile, int _nbClasses, double decisionThreshold = -1, int indexPositiveClass = -1);
  void setClassFromFile(const char *classFile, int _nbClasses);

  std::vector<std::vector<double>> *getDatas();
  std::vector<int> *getClasses();
  bool getHasClasses() const;
  std::vector<int> *getPredictions();
  std::vector<std::vector<double>> *getOutputValuesPredictions();
  int getNbClasses() const;
  int getNbAttributes() const;
  int getNbSamples() const;

  void setAttributes(const char *attributFile, int _nbAttributes, int _nbClasses = -1);
  std::vector<std::string> *getAttributeNames();
  std::vector<std::string> *getClassNames();
  bool getHasClassNames() const;

  std::vector<std::vector<double>> getWeights() const;
  std::vector<double> getInBiais() const;
  std::vector<double> getInWeights() const;
};

#endif
