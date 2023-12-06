#include "dataSet.h"
using namespace std;

DataSetFid::DataSetFid(const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile) : hasDatas(true) {
  string line;

  // Get data

  fstream fileDta;

  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error : file " + std::string(dataFile) + " not found");
  }

  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getDataLine(line, dataFile);
    }
  }

  fileDta.close(); // close data file

  // Get data class

  if (trueClassFile != nullptr) {

    hasClassesAttr = true;
    fstream fileCl;

    fileCl.open(trueClassFile, ios::in); // read true dataclass file
    if (fileCl.fail()) {
      throw FileNotFoundError("Error : file " + std::string(trueClassFile) + " not found");
    }

    while (!fileCl.eof()) {
      int nbClasses = 0;

      getline(fileCl, line);
      if (!checkStringEmpty(line)) {
        getClassLine(line, dataFile, nbClasses);
      }
    }

    fileCl.close(); // close file
  }

  // Get predictions

  fstream filePrd;

  filePrd.open(predFile, ios::in); // read predictions data file
  if (filePrd.fail()) {
    throw FileNotFoundError("Error : file " + std::string(predFile) + " not found");
  }

  while (!filePrd.eof()) {
    getline(filePrd, line);
    if (!checkStringEmpty(line)) {
      vector<double> valuesPred;
      getPredLine(line, valuesPred, hasDecisionThreshold, decisionThreshold, indexPositiveClass, dataFile);
    }
  }

  filePrd.close(); // close file
}

// One dataFile with data, predictions and (maybe) classes
DataSetFid::DataSetFid(const char *dataFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass) : hasDatas(true) {
  fstream fileDta;
  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error : file " + std::string(dataFile) + " not found");
  }
  string line;
  vector<double> valuesPred;
  bool firstLine = true;
  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getDataLine(line, dataFile);
    } else if (firstLine) {
      throw FileFormatError("Error : in file " + std::string(dataFile) + ", first line is empty");
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error : file " + std::string(dataFile) + " is not on good format, there are more than one empty line between 2 samples");
        }
      }
      break; // There are just empty lines at the end of the file
    }
    if (fileDta.eof()) {
      throw FileContentError("Error : file " + std::string(dataFile) + " has not enough prediction data");
    }
    // Get predictions
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      getPredLine(line, valuesPred, hasDecisionThreshold, decisionThreshold, indexPositiveClass, dataFile);
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error : file " + std::string(dataFile) + " is not on good format, there is empty lines inbetween data");
        }
      }
      throw FileContentError("Error : file " + std::string(dataFile) + " has not enough prediction data");
    }
    bool endOfFile = false;
    if (fileDta.eof()) {
      endOfFile = true;
    }
    // Get classes
    getline(fileDta, line);
    if (endOfFile || checkStringEmpty(line)) {
      hasTrueClassesVect.push_back(false);
      trueClasses.push_back(-1);
    } else {

      int nbClasses = 0;
      getClassLine(line, dataFile, nbClasses);
      if (nbClasses != valuesPred.size() && trueClasses[0] != -1) {
        std::cout << trueClasses[0] << std::endl;
        throw FileContentError("Error : in file " + std::string(dataFile) + ", true classes and predictions need to have the same amount of data, or classes need to be -1 to express that there is no class.");
      }

      if (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error : in file " + std::string(dataFile) + ", you need to have empty lines between samples. You have chosen to give data, predictions and classes in one file. If you want to separate them, use -p and -c");
        }
      }
    }
    firstLine = false;
  }
  bool noElement = std::all_of(trueClasses.begin(), trueClasses.end(), [](int element) {
    return element == -1;
  });

  if (!noElement) {
    hasClassesAttr = true;
  }

  fileDta.close(); // close data file
}

DataSetFid::DataSetFid(const char *weightFile) : hasWeights(true) {

  // Get weights
  fstream fileWts;

  string line;

  fileWts.open(weightFile, ios::in); // Read weight file
  if (fileWts.fail()) {
    throw FileNotFoundError("Error : file " + std::string(weightFile) + " not found");
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
      throw FileContentError("Error : in file " + std::string(dataFile) + ", a number is required");
    }
    valuesData.push_back(valueData);
  }
  datas.push_back(valuesData);
}

void DataSetFid::getPredLine(const string &line, vector<double> &valuesPred, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *dataFile) {

  std::stringstream myLine(line);
  double valuePred;
  valuesPred.clear();
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valuePred;
    if (myLine.fail()) {
      throw FileContentError("Error : in file " + std::string(dataFile) + ", a number is required");
    }
    if (valuePred != 1 && valuePred != 0) {
      everyPredIsBool = false;
    }
    valuesPred.push_back(valuePred);
  }
  outputValuesPredictions.push_back(valuesPred);

  if (hasDecisionThreshold && indexPositiveClass >= static_cast<int>(valuesPred.size())) {
    throw CommandArgumentException("Error : parameter positive_index(-x) has to be a positive integer smaller than " + to_string(valuesPred.size()));
  }

  if (hasDecisionThreshold && valuesPred[indexPositiveClass] >= decisionThreshold) {
    predictions.push_back(indexPositiveClass);
  } else {
    predictions.push_back(static_cast<int>(std::max_element(valuesPred.begin(), valuesPred.end()) - valuesPred.begin()));
  }
}

void DataSetFid::getClassLine(const string &line, const char *dataFile, int &nbClasses) {
  std::stringstream myLine(line);
  float valueClass;
  nbClasses = 0;
  bool classFound = false;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valueClass;
    if (myLine.fail()) {
      throw FileContentError("Error : in file " + std::string(dataFile) + ", true classes need to be 0, 1, 0.0, 1.0 or -1 if we haven't the class");
    }
    if (valueClass == 1.0f) {
      trueClasses.push_back(nbClasses);
      hasTrueClassesVect.push_back(true);
      if (!classFound) {
        classFound = true;
      } else {
        throw FileContentError("Error : in file " + std::string(dataFile) + ", there is multiple 1's and -1's in a class line");
      }
    } else if (valueClass == -1.0f) {
      trueClasses.push_back(-1);
      hasTrueClassesVect.push_back(false);
      if (!classFound) {
        classFound = true;
      } else {
        throw FileContentError("Error : in file " + std::string(dataFile) + ", there is multiple 1's and -1's in a class line");
      }
    } else if (valueClass != 0.0f) {
      throw FileContentError("Error : in file " + std::string(dataFile) + ", true classes need to be 0, 1, 0.0, 1.0 or -1 if we haven't the class");
    }
    nbClasses++;
  }
  if (!classFound) {
    throw FileContentError("Error : in file " + std::string(dataFile) + ", a 1 or 1.0 is required to express the true class, or -1 if we haven't the class");
  }
  if ((trueClasses.back() == -1) && (nbClasses != 1)) {
    throw FileContentError("Error : in file " + std::string(dataFile) + ", -1 is given for class but other numbers are given, write only -1 if we don't know the class");
  }
}

vector<vector<double>> *DataSetFid::getDatas() {
  if (hasDatas) {
    return &datas;
  } else {
    throw CommandArgumentException("Error : data file not specified for this dataset");
  }
}

vector<int> *DataSetFid::getTrueClasses() {
  if (hasClassesAttr) {
    return &trueClasses;
  } else {
    throw CommandArgumentException("Error : dataClass file not specified for this dataset");
  }
}

bool DataSetFid::hasClasses() const {
  return hasClassesAttr;
}

vector<bool> *DataSetFid::gethasTrueClassesVect() {
  if (hasClassesAttr) {
    return &hasTrueClassesVect;
  } else {
    throw CommandArgumentException("Error : dataClass file not specified for this dataset");
  }
}

vector<int> *DataSetFid::getPredictions() {
  if (hasDatas) {
    return &predictions;
  } else {
    throw CommandArgumentException("Error : prediction file not specified for this dataset");
  }
}

vector<vector<double>> *DataSetFid::getOutputValuesPredictions() {
  if (hasDatas) {
    return &outputValuesPredictions;
  } else {
    throw CommandArgumentException("Error : prediction file not specified for this dataset");
  }
}

bool DataSetFid::hasConfidence() const {
  if (everyPredIsBool) {
    return false;
  } else {
    return true;
  }
}

int DataSetFid::getNbClasses() const {
  return static_cast<int>(outputValuesPredictions[0].size());
}

vector<vector<double>> DataSetFid::getWeights() const {
  if (hasWeights) {
    return weights;
  } else {
    throw CommandArgumentException("Error : weight file not specified for this dataset");
  }
}

vector<double> DataSetFid::getInBiais() const {
  if (hasWeights) {
    return weights[0];
  } else {
    throw CommandArgumentException("Error : weight file not specified for this dataset");
  }
}

vector<double> DataSetFid::getInWeights() const {
  if (hasWeights) {
    return weights[1];
  } else {
    throw CommandArgumentException("Error : weight file not specified for this dataset");
  }
}

Attribute::Attribute(const char *attributeFile, int nbAttributs, int nbClass) {

  // Get attributes
  fstream fileAttr;

  string line;

  fileAttr.open(attributeFile, ios::in); // Read weight file
  if (fileAttr.fail()) {
    throw FileNotFoundError("Error : file " + std::string(attributeFile) + " not found");
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
  if (attributeNames.size() < nbAttributs) {
    throw FileContentError("Error : in file " + std::string(attributeFile) + ", there is not enough attribute names");
  } else if (attributeNames.size() == nbAttributs) {
    hasClassNames = false;
  } else if (attributeNames.size() != nbAttributs + nbClass) {
    throw FileContentError("Error : in file " + std::string(attributeFile) + ", there is not the good amount of attribute and class names");
  } else {
    hasClassNames = true;
    auto firstEl = attributeNames.end() - nbClass;
    auto lastEl = attributeNames.end();
    classNames.insert(classNames.end(), firstEl, lastEl);
    attributeNames.erase(firstEl, lastEl);
  }

  fileAttr.close(); // close file
}

vector<string> *Attribute::getAttributeNames() {
  if (hasAttributes) {
    return &attributeNames;
  } else {
    throw CommandArgumentException("Error : attribute file not specified for this dataset");
  }
}

vector<string> *Attribute::getClassNames() {
  return &classNames;
}
