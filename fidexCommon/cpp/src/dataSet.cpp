#include "dataSet.h"
using namespace std;

/*
Creates dataset using 3 separate datafiles : data, predictions and classes(not mendatory)
*/
DataSetFid::DataSetFid(const string &name, const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile) : datasetName(name) {

  // Get data
  setDataFromFile(dataFile);
  // Get data class
  if (trueClassFile != nullptr) {
    setClassFromFile(trueClassFile);
  }
  // Get predictions
  setPredFromFile(hasDecisionThreshold, decisionThreshold, indexPositiveClass, predFile);

  checkDatas();
}

/*
Creates dataset using 1 datafiles containing datas, predictions and classes(not mendatory)
*/
DataSetFid::DataSetFid(const string &name, const char *dataFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass) : datasetName(name), hasDatas(true), hasPreds(true) {
  fstream fileDta;
  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " not found");
  }
  string line;
  bool firstLine = true;
  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getDataLine(line, dataFile);
    } else if (firstLine) {
      throw FileFormatError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", first line is empty");
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " is not on good format, there are more than one empty line between 2 samples");
        }
      }
      break; // There are just empty lines at the end of the file
    }
    if (fileDta.eof()) {
      throw FileContentError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " has not enough prediction data");
    }
    // Get predictions
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getPredLine(line, hasDecisionThreshold, decisionThreshold, indexPositiveClass, dataFile);
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " is not on good format, there is empty lines inbetween data");
        }
      }
      throw FileContentError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " has not enough prediction data");
    }
    bool endOfFile = false;
    if (fileDta.eof()) {
      endOfFile = true;
    }
    // Get classes
    getline(fileDta, line);
    if (!endOfFile && !checkStringEmpty(line)) {
      getClassLine(line, dataFile);

      if (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", you need to have empty lines between samples. You have chosen to give data, predictions and classes in one file. If you want to separate them, use -p and -c");
        }
      }
    }
    firstLine = false;
  }

  if (!trueClasses.empty()) {
    hasClasses = true;
    nbClassData = static_cast<int>(trueClasses.size());
  }

  fileDta.close(); // close data file

  nbSamples = static_cast<int>(datas.size());
  nbPredData = static_cast<int>(predictions.size());
  checkDatas();
}

/*
Creates dataset using 1 a weights file
*/
DataSetFid::DataSetFid(const std::string &name, const char *weightFile) : datasetName(name), hasWeights(true) {

  // Get weights
  fstream fileWts;

  string line;

  fileWts.open(weightFile, ios::in); // Read weight file
  if (fileWts.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(weightFile) + " not found");
  }

  while (!fileWts.eof()) {
    getline(fileWts, line);
    if (!checkStringEmpty(line)) {
      std::stringstream myLine(line);
      double value;
      vector<double> tempVect;
      while (myLine >> value) {
        tempVect.push_back(value);
      }
      weights.push_back(tempVect);
    }
  }
  fileWts.close(); // close file
}

/*
Add datas from dataFile in the dataset
*/
void DataSetFid::setDataFromFile(const char *dataFile) {
  hasDatas = true;
  string line;
  fstream fileDta;

  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " not found");
  }

  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getDataLine(line, dataFile);
    }
  }

  fileDta.close(); // close data file
  nbSamples = static_cast<int>(datas.size());
  checkDatas();
}

/*
Add predictions from predFile in the dataset
*/
void DataSetFid::setPredFromFile(bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *predFile) {
  hasPreds = true;
  string line;
  fstream filePrd;

  filePrd.open(predFile, ios::in); // read predictions data file
  if (filePrd.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(predFile) + " not found");
  }

  while (!filePrd.eof()) {
    getline(filePrd, line);
    if (!checkStringEmpty(line)) {
      getPredLine(line, hasDecisionThreshold, decisionThreshold, indexPositiveClass, predFile);
    }
  }

  filePrd.close(); // close file
  nbPredData = static_cast<int>(predictions.size());
  checkDatas();
}

/*
Add classes from classFile in the dataset
*/
void DataSetFid::setClassFromFile(const char *classFile) {
  hasClasses = true;
  string line;
  fstream fileCl;

  fileCl.open(classFile, ios::in); // read true dataclass file
  if (fileCl.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(classFile) + " not found");
  }

  while (!fileCl.eof()) {
    getline(fileCl, line);
    if (!checkStringEmpty(line)) {
      getClassLine(line, classFile);
    }
  }

  fileCl.close(); // close file
  nbClassData = static_cast<int>(trueClasses.size());
  checkDatas();
}

/*
Read data line from dataFile and save it in datas
*/
void DataSetFid::getDataLine(const string &line, const char *dataFile) {
  std::stringstream myLine(line);
  double valueData;
  vector<double> valuesData;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valueData;
    if (myLine.fail()) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", a number is required");
    }
    valuesData.push_back(valueData);
  }

  if (nbAttributes == -1) {
    nbAttributes = static_cast<int>(valuesData.size());
  }
  if (nbAttributes != static_cast<int>(valuesData.size())) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", there are not the same number of attributes for each sample.");
  }

  datas.push_back(valuesData);
}

/*
Read prediction line from predFile and save it in predictions
*/
void DataSetFid::getPredLine(const string &line, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *predFile) {

  std::stringstream myLine(line);
  double valuePred;
  vector<double> valuesPred;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valuePred;
    if (myLine.fail()) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(predFile) + ", a number is required");
    }
    valuesPred.push_back(valuePred);
  }
  outputValuesPredictions.push_back(valuesPred);
  if (nbPreds == -1) {
    nbPreds = static_cast<int>(valuesPred.size());
  }

  if (static_cast<int>(valuesPred.size()) != nbPreds) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(predFile) + ", there are not the same number of predictions for each sample.");
  }

  if (hasDecisionThreshold && indexPositiveClass >= nbPreds) {
    throw CommandArgumentException("Error in dataset " + datasetName + " : The index of positive class cannot be greater or equal to the number of classes (" + to_string(nbPreds) + ")");
  }

  if (hasDecisionThreshold && valuesPred[indexPositiveClass] >= decisionThreshold) {
    predictions.push_back(indexPositiveClass);
  } else {
    predictions.push_back(static_cast<int>(std::max_element(valuesPred.begin(), valuesPred.end()) - valuesPred.begin()));
  }
}

/*
Read class line from classFile and save it in trueClasses
*/
void DataSetFid::getClassLine(const string &line, const char *classFile) {
  std::stringstream myLine(line);
  float valueClass;
  int nbClassesLine = 0;
  bool classFound = false;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valueClass;
    if (myLine.fail()) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", true classes need to be 0, 1, 0.0 or 1.0");
    }
    if (valueClass == 1.0f) {
      trueClasses.push_back(nbClassesLine);
      if (!classFound) {
        classFound = true;
      } else {
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", there is multiple 1's in a class line");
      }
    } else if (valueClass != 0.0f) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", true classes need to be 0, 1, 0.0 or 1.0");
    }
    nbClassesLine++;
  }
  if (!classFound) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", a 1 or 1.0 is required to express the true class");
  }
  if (nbClasses == -1) {
    nbClasses = nbClassesLine;
  }
  if (nbClasses != nbClassesLine) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", there are not the same number of classes for each sample.");
  }
}

/*
Return datas
*/
vector<vector<double>> *DataSetFid::getDatas() {
  if (hasDatas) {
    return &datas;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/*
Return classes
*/
vector<int> *DataSetFid::getClasses() {
  if (hasClasses) {
    return &trueClasses;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : dataClass file not specified for this dataset");
  }
}

/*
Return whether the dataset contains classes
*/
bool DataSetFid::getHasClasses() const {
  return hasClasses;
}

/*
Return predictions
*/
vector<int> *DataSetFid::getPredictions() {
  if (hasPreds) {
    return &predictions;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset");
  }
}

/*
Return prediction output values
*/
vector<vector<double>> *DataSetFid::getOutputValuesPredictions() {
  if (hasPreds) {
    return &outputValuesPredictions;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset");
  }
}

/*
Return the number of classes in the dataset
*/
int DataSetFid::getNbClasses() const {
  if (hasClasses) {
    return nbClasses;
  } else if (hasPreds) {
    return nbPreds;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : classes not specified for this dataset");
  }
}

/*
Return the number of attributes in the dataset
*/
int DataSetFid::getNbAttributes() const {
  if (hasDatas) {
    return nbAttributes;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/*
Return the number of samples in the dataset
*/
int DataSetFid::getNbSamples() const {
  if (hasDatas) {
    return nbSamples;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/*
Return weights
*/
vector<vector<double>> DataSetFid::getWeights() const {
  if (hasWeights) {
    return weights;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/*
Return biais of first layer
*/
vector<double> DataSetFid::getInBiais() const {
  if (hasWeights) {
    return weights[0];
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/*
Return weights of first layer
*/
vector<double> DataSetFid::getInWeights() const {
  if (hasWeights) {
    return weights[1];
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/*
Check for errors in the dataset
*/
void DataSetFid::checkDatas() const {
  if (hasDatas && nbSamples < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There is no data samples.");
  }
  if (hasPreds && nbPredData < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There is no predictions.");
  }
  if (hasClasses && nbClassData < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There is no classes.");
  }

  if (hasDatas && hasPreds && nbSamples != nbPredData) {
    throw FileContentError("Error in dataset " + datasetName + " : The number of prediction data does not match the number of samples.");
  }
  if (hasClasses) {
    if (hasPreds && nbClasses != nbPreds) {
      throw FileContentError("Error in dataset " + datasetName + " : the number of class data does not match the number of prediction data for a given sample.");
    }
    if (hasDatas && nbSamples != nbClassData) {
      throw FileContentError("Error in dataset " + datasetName + " : The number of class data does not match the number of samples.");
    }
    if (hasPreds && nbPredData != nbClassData) {
      throw FileContentError("Error in dataset " + datasetName + " : The number of class data does not match the number of prediction data.");
    }
  }

  if (hasAttributes && hasDatas) {
    if (attributeNames.size() < nbAttributes) {
      throw FileContentError("Error in dataset " + datasetName + " : there is not enough attribute names");
    } else if (hasClasses && attributeNames.size() != nbAttributes && attributeNames.size() != nbAttributes + nbClasses) {
      throw FileContentError("Error in dataset " + datasetName + " :  there is not the good amount of attribute and class names");
    }
  }
}

/*
Add attributes from attributeFile in the dataset
*/
void DataSetFid::setAttribute(const char *attributeFile) {
  hasAttributes = true;
  // Get attributes
  fstream fileAttr;
  string line;

  fileAttr.open(attributeFile, ios::in); // Read attribute file
  if (fileAttr.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(attributeFile) + " not found");
  }
  while (!fileAttr.eof()) {
    getline(fileAttr, line);
    // Remove invisible characters at the end if exist
    for (int i = static_cast<int>(line.length()) - 1; i >= 0; i--) {
      if (!std::isspace(line[i])) {
        line.erase(i + 1);
        break;
      }
    }
    if (!checkStringEmpty(line)) {
      std::stringstream myLine(line);
      string attr = myLine.str();
      attributeNames.push_back(attr);
    }
  }

  hasClassNames = false;

  if (!hasDatas) {
    std::cout << "WARNING in dataset " + datasetName + " there is no data, so no verification is done on attributes." << std::endl;
  }

  if (!hasClasses) {
    std::cout << "WARNING in dataset " + datasetName + " there is no classes, so no verification is done on classNames in attribute file." << std::endl;
  }

  if (hasDatas && attributeNames.size() < nbAttributes) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(attributeFile) + ", there is not enough attribute names");
  } else if (hasDatas && hasClasses) {
    if (attributeNames.size() == nbAttributes) {
      hasClassNames = false;
    } else if (attributeNames.size() != nbAttributes + nbClasses) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(attributeFile) + ", there is not the good amount of attribute and class names");
    } else {
      hasClassNames = true;
      auto firstEl = attributeNames.end() - nbClasses;
      auto lastEl = attributeNames.end();
      classNames.insert(classNames.end(), firstEl, lastEl);
      attributeNames.erase(firstEl, lastEl);
    }
  }

  fileAttr.close(); // close file
}

/*
Return attribute names
*/
vector<string> *DataSetFid::getAttributeNames() {
  if (hasAttributes) {
    return &attributeNames;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : attribute file not specified for this dataset");
  }
}

/*
Return class names
*/
vector<string> *DataSetFid::getClassNames() {
  if (hasClassNames) {
    return &classNames;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : classNames not present in attribute file or attribute file not specified");
  }
}

/*
Return whether the dataset contains classNames
*/
bool DataSetFid::getHasClassNames() const {
  return hasClassNames;
}
