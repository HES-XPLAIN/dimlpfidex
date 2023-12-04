#include "dataSet.h"
using namespace std;

DataSetFid::DataSetFid(const char *dataFile, const char *predFile, bool hasDecisionThreshold, double decisionThreshold, int indexPositiveClass, const char *trueClassFile) : hasDatas(true) {
  int i; // iterator
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
      std::stringstream myLine(line);
      double value;
      vector<double> tempVect;
      while (myLine >> value) {
        tempVect.push_back(value);
      }
      datas.push_back(tempVect);
    }
  }

  fileDta.close(); // close data file

  // Get data class

  if (trueClassFile != nullptr) {

    hasClasses = true;
    fstream fileCl;

    fileCl.open(trueClassFile, ios::in); // read true dataclass file
    if (fileCl.fail()) {
      throw FileNotFoundError("Error : file " + std::string(trueClassFile) + " not found");
    }

    while (!fileCl.eof()) {
      i = 0;
      getline(fileCl, line);
      if (!checkStringEmpty(line)) {
        std::stringstream myLine(line);
        float value;
        while (myLine >> value) {
          if (value == 1.0f) {
            trueClasses.push_back(i);
            hasTrueClasses.push_back(true);
            break;
          } else if (value == -1.0f) {
            trueClasses.push_back(-1);
            hasTrueClasses.push_back(false);
            break;
          }
          i++;
        }
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
      std::stringstream myLine(line);
      double value;
      vector<double> values;
      while (myLine >> value) {
        if (value != 1 && value != 0) {
          everyPredIsBool = false;
        }
        values.push_back(value);
      }
      outputValuesPredictions.push_back(values);

      if (indexPositiveClass >= static_cast<int>(values.size())) {
        throw CommandArgumentException("Error : parameter positive_index(-x) has to be a positive integer smaller than " + to_string(values.size()));
      }

      if (hasDecisionThreshold && values[indexPositiveClass] >= decisionThreshold) {
        predictions.push_back(indexPositiveClass);
      } else {
        predictions.push_back(static_cast<int>(std::max_element(values.begin(), values.end()) - values.begin()));
      }
    }
  }

  filePrd.close(); // close file
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

vector<vector<double>> *DataSetFid::getDatas() {
  if (hasDatas) {
    return &datas;
  } else {
    throw CommandArgumentException("Error : data file not specified for this dataset");
  }
}

vector<int> *DataSetFid::getTrueClasses() {
  if (hasClasses) {
    return &trueClasses;
  } else {
    throw CommandArgumentException("Error : dataClass file not specified for this dataset");
  }
}

vector<bool> *DataSetFid::getHasTrueClasses() {
  if (hasClasses) {
    return &hasTrueClasses;
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
  return outputValuesPredictions[0].size();
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
