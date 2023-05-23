#include "dataSet.h"
using namespace std;

DataSetFid::DataSetFid(const char *dataFile, const char *predFile, const char *trueClassFile) : hasDatas(true) {

  int i; // iterator
  string line;

  // Get data

  fstream fileDta;

  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw std::runtime_error("Error : file " + std::string(dataFile) + " not found");
  }

  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (line.length() != 0) {
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
      throw std::runtime_error("Error : file " + std::string(trueClassFile) + " not found");
    }

    while (!fileCl.eof()) {
      i = 0;
      getline(fileCl, line);
      if (line.length() != 0) {
        std::stringstream myLine(line);
        int value;
        while (myLine >> value) {
          if (value == 1) {
            trueClasses.push_back(i);
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
    throw std::runtime_error("Error : file " + std::string(predFile) + " not found");
  }

  while (!filePrd.eof()) {
    getline(filePrd, line);
    if (line.length() != 0) {
      std::stringstream myLine(line);
      double value;
      vector<double> values;
      while (myLine >> value) {
        values.push_back(value);
      }
      outputValuesPredictions.push_back(values);
      predictions.push_back(static_cast<int>(std::max_element(values.begin(), values.end()) - values.begin()));
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
    throw std::runtime_error("Error : file " + std::string(weightFile) + " not found");
  }

  while (!fileWts.eof()) {
    getline(fileWts, line);
    if (line.length() != 0) {
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
    throw std::runtime_error("Error : data file not specified for this DataSetFid");
  }
}

vector<int> *DataSetFid::getTrueClasses() {
  if (hasClasses) {
    return &trueClasses;
  } else {
    throw std::runtime_error("Error : dataClass file not specified for this DataSetFid");
  }
}

vector<int> *DataSetFid::getPredictions() {
  if (hasDatas) {
    return &predictions;
  } else {
    throw std::runtime_error("Error : prediction file not specified for this DataSetFid");
  }
}

vector<vector<double>> *DataSetFid::getOutputValuesPredictions() {
  if (hasDatas) {
    return &outputValuesPredictions;
  } else {
    throw std::runtime_error("Error : prediction file not specified for this DataSetFid");
  }
}

vector<vector<double>> DataSetFid::getWeights() const {
  if (hasWeights) {
    return weights;
  } else {
    throw std::runtime_error("Error : weight file not specified for this DataSetFid");
  }
}

vector<double> DataSetFid::getInBiais() const {
  if (hasWeights) {
    return weights[0];
  } else {
    throw std::runtime_error("Error : weight file not specified for this DataSetFid");
  }
}

vector<double> DataSetFid::getInWeights() const {
  if (hasWeights) {
    return weights[1];
  } else {
    throw std::runtime_error("Error : weight file not specified for this DataSetFid");
  }
}

Attribute::Attribute(const char *attributeFile) {

  // Get attributes
  fstream fileAttr;

  string line;

  fileAttr.open(attributeFile, ios::in); // Read weight file
  if (fileAttr.fail()) {
    throw std::runtime_error("Error : file " + std::string(attributeFile) + " not found");
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
      attributes.push_back(attr);
    }
  }
  fileAttr.close(); // close file
}

vector<string> *Attribute::getAttributes() {
  if (hasAttributes) {
    return &attributes;
  } else {
    throw std::runtime_error("Error : attribute file not specified for this dataset");
  }
}
