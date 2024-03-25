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

using namespace std;

class DataSetFid {

private:
  std::string datasetName;
  std::vector<std::vector<double>> datas;
  std::vector<int> trueClasses;
  std::vector<int> predictions;
  std::vector<std::vector<double>> outputValuesPredictions;
  std::vector<std::vector<std::vector<double>>> weights;
  bool hasDatas = false;
  bool hasPreds = false;
  bool hasClasses = false;
  bool hasWeights = false;

  std::string classFormat = ""; // one-hot, id, one-hot_combined or id_combined

  double decisionThreshold = -1;
  int positiveClassIndex = -1;

  int _nbClasses = -1;
  int _nbAttributes = -1;
  int nbSamples = -1;
  int nbClassData = -1;
  int nbPredData = -1;
  int nbNets = 1;

  void setDataLine(const std::string &line, const std::string &dataFile);
  void setPredLine(const std::string &line, const std::string &dataFile);
  void setClassLine(const std::string &line, const std::string &dataFile);

  void setNbClass(int nbClasses);
  void setNbClassAndAttr(int nbClasses, int nbAttributes);

  bool isOneHot(const std::vector<double> &values, int start, int &oneHotIndex) const;

  void checkDatas() const;
  void checkThreshold() const;

  std::vector<std::string> attributeNames;
  std::vector<std::string> classNames;
  bool hasAttributes = false;
  bool hasClassNames = false;

public:
  explicit DataSetFid(const std::string &name) : datasetName(name){};
  DataSetFid(const std::string &name, const std::string &dataFile, const std::string &predFile, int nbAttributes, int nbClasses, double decisionThresh, int indexPositiveCl, const std::string &trueClassFile = "");
  DataSetFid(const std::string &name, const std::string &dataFile, int nbAttributes, int nbClasses, double decisionThresh, int indexPositiveCl); // dataFile with data, predictions and maybe classes
  explicit DataSetFid(const std::string &name, const std::string &weightFile);
  void parseSingleNetwork(fstream &fileWts);
  void parseMultipleNetworks(fstream &fileWts);

  void setDataFromFile(const std::string &dataFile, int nbAttributes, int nbClasses);
  void setPredFromFile(const std::string &predFile, int nbClasses, double decisionThreshold = -1, int positiveClassIndex = -1);
  void setClassFromFile(const std::string &classFile, int nbClasses);

  std::vector<std::vector<double>> &getDatas();
  std::vector<int> &getClasses();
  bool getHasClasses() const;
  std::vector<int> &getPredictions();
  std::vector<std::vector<double>> &getOutputValuesPredictions();
  int getNbClasses() const;
  int getNbAttributes() const;
  int getNbSamples() const;

  void setAttributes(const std::string &attributesFile, int nbAttributes, int nbClasses = -1);
  std::vector<std::string> &getAttributeNames();
  std::vector<std::string> &getClassNames();
  bool getHasAttributeNames() const;
  bool getHasClassNames() const;

  std::vector<std::vector<std::vector<double>>> getWeights() const;
  std::vector<double> getInBiais(int netId) const;
  std::vector<double> getInWeights(int netId) const;
  int getNbNets() const;
};

#endif
