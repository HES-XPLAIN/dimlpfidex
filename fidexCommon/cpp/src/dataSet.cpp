#include "dataSet.h"
using namespace std;

/**
 * @brief Construct a new DataSetFid object using three separate datafiles : datas, predictions and maybe classes(not mendatory)
 *
 * @param name string containing the name of the dataSet
 * @param dataFile const char* data file name
 * @param predFile const char* predicion file name
 * @param hasDecisionThreshold boolean that checks if a decision threshold is given
 * @param decisionThreshold double indicating the decision threshold, useful when choosing the decision
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold
 * @param trueClassFile const char* class file name
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

/**
 * @brief Construct a new DataSetFid object using a datafile containing datas, predictions and maybe classes(not mendatory)
 *
 * @param name string containing the name of the dataSet
 * @param dataFile const char* data file name containing datas, predictions and maybe classes(not mendatory)
 * @param hasDecisionThreshold boolean that checks if a decision threshold is given
 * @param decisionThreshold double indicating the decision threshold, useful when choosing the decision
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold
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

/**
 * @brief Construct a new DataSetFid object using a weights file
 *
 * @param name string containing the name of the dataSet
 * @param weightFile const char* weight file name
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

/**
 * @brief Add datas from dataFile in the dataset
 *
 * @param dataFile const char* data file name
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

/**
 * @brief Add predictions in the dataset using a prediction file
 *
 * @param hasDecisionThreshold boolean that checks if a decision threshold is given
 * @param decisionThreshold double indicating the decision threshold, useful when choosing the decision
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold
 * @param predFile const char* prediction file name
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

/**
 * @brief Add classes from classFile in the dataset
 *
 * @param classFile const char* file name containing classes in one hot format
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

/**
 * @brief Read data line from data file and save it in datas
 *
 * @param line string containing one line of the data file
 * @param dataFile const char* data file name
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

/**
 * @brief Read prediction line from prediction file and save it in predictions
 *
 * @param line string containing one line of the prediction file
 * @param hasDecisionThreshold boolean that checks if a decision threshold is given
 * @param decisionThreshold double indicating the decision threshold, useful when choosing the decision
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold
 * @param predFile const char* prediction file name
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

/**
 * @brief Read class line from classFile and save it in trueClasses
 *
 * @param line string containing one line of the class file
 * @param classFile const char* class file name containing classes in one hot format
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

/**
 * @brief Return the samples' data
 *
 * @return vector<vector<double>>*
 */
vector<vector<double>> *DataSetFid::getDatas() {
  if (hasDatas) {
    return &datas;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/**
 * @brief Return the classes of the samples
 *
 * @return vector<int>*
 */
vector<int> *DataSetFid::getClasses() {
  if (hasClasses) {
    return &trueClasses;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : dataClass file not specified for this dataset");
  }
}

/**
 * @brief Return whether the dataset contains classes
 *
 * @return true
 * @return false
 */
bool DataSetFid::getHasClasses() const {
  return hasClasses;
}

/**
 * @brief Return the predictions of the samples
 *
 * @return vector<int>*
 */
vector<int> *DataSetFid::getPredictions() {
  if (hasPreds) {
    return &predictions;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset");
  }
}

/**
 * @brief Return the prediction output values of the samples
 *
 * @return vector<vector<double>>*
 */
vector<vector<double>> *DataSetFid::getOutputValuesPredictions() {
  if (hasPreds) {
    return &outputValuesPredictions;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset");
  }
}

/**
 * @brief Return the number of classes in the dataset
 *
 * @return int
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

/**
 * @brief Return the number of attributes in the dataset
 *
 * @return int
 */
int DataSetFid::getNbAttributes() const {
  if (hasDatas) {
    return nbAttributes;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/**
 * @brief Return the number of samples in the dataset
 *
 * @return int
 */
int DataSetFid::getNbSamples() const {
  if (hasDatas) {
    return nbSamples;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset");
  }
}

/**
 * @brief Return the weights
 *
 * @return vector<vector<double>>
 */
vector<vector<double>> DataSetFid::getWeights() const {
  if (hasWeights) {
    return weights;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/**
 * @brief Return the biais of first layer
 *
 * @return vector<double>
 */
vector<double> DataSetFid::getInBiais() const {
  if (hasWeights) {
    return weights[0];
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/**
 * @brief Return the weights of first layer
 *
 * @return vector<double>
 */
vector<double> DataSetFid::getInWeights() const {
  if (hasWeights) {
    return weights[1];
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset");
  }
}

/**
 * @brief Check for errors in the dataset
 *
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

/**
 * @brief Add attributes and eventually classes from attribute file in the dataset
 *
 * @param attributeFile const char* attribute file name
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

/**
 * @brief Return attribute names
 *
 * @return vector<string>*
 */
vector<string> *DataSetFid::getAttributeNames() {
  if (hasAttributes) {
    return &attributeNames;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : attribute file not specified for this dataset");
  }
}

/**
 * @brief Return class names
 *
 * @return vector<string>*
 */
vector<string> *DataSetFid::getClassNames() {
  if (hasClassNames) {
    return &classNames;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : classNames not present in attribute file or attribute file not specified");
  }
}

/**
 * @brief Return whether the dataset contains classNames
 *
 * @return true
 * @return false
 */
bool DataSetFid::getHasClassNames() const {
  return hasClassNames;
}
