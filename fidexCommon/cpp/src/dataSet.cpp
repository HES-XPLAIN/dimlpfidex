#include "dataSet.h"
using namespace std;

/**
 * @brief Construct a new DataSetFid object using three separate data files: one for data, one for predictions, and optionally one for classes.
 *        - The data file should contain the attributes of each sample, and optionally, the class information can also be included in this file.
 *          If class information is included in the data file, there is no need to provide a separate class file.
 *        - The prediction file should contain the prediction scores for each class per sample.
 *        - The class file (optional) should be used only if class information is not included in the data file. It should contain either
 *          a single class ID or a series of integers in a one-hot encoding scheme per sample.
 *
 *        Expected file formats:
 *        - Data file: Each line should contain a series of numerical values representing the attributes of a sample,
 *          optionally followed by the class information (either as a single integer for class ID or a one-hot encoded vector).
 *        - Prediction file: Each line should contain a series of numerical values.
 *        - Class file (optional): Only needed if class information is not included in the data file. Each line should contain either a single class ID
 *          or a series of integers in a one-hot encoding scheme.
 *
 *        Each numbers in a line are separated by a space.
 *        The number of attributes and classes in the dataset are used to validate the format and content of the data and class files.
 *
 * @param name string containing the name of the dataSet
 * @param dataFile const char* data file name
 * @param predFile const char* predicion file name
 * @param _nbAttributes int number of attributes
 * @param _nbClasses int number of classes
 * @param decisionThresh double indicating the decision threshold, useful when choosing the decision (-1 for no threshold)
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold (-1 if no threshold)
 * @param trueClassFile const char* class file name
 */
DataSetFid::DataSetFid(const string &name, const char *dataFile, const char *predFile, int _nbAttributes, int _nbClasses, double decisionThresh, int indexPositiveCl, const char *trueClassFile) : datasetName(name) {

  setNbClassAndAttr(_nbClasses, _nbAttributes);

  if (decisionThresh != -1 && !std::isnan(decisionThresh)) {
    decisionThreshold = decisionThresh;
  }
  if (indexPositiveCl != -1 && !std::isnan(indexPositiveCl)) {
    indexPositiveClass = indexPositiveCl;
  }

  checkThreshold();

  // Get data
  setDataFromFile(dataFile, _nbAttributes, _nbClasses);
  // Get data class
  if (trueClassFile != nullptr) {
    setClassFromFile(trueClassFile, _nbClasses);
  }
  // Get predictions
  setPredFromFile(predFile, _nbClasses);

  checkDatas();
}

/**
 * @brief Construct a new DataSetFid object using a data file. The file contains data samples, predictions,
 *        and optionally classes (not mandatory). The format of each sample in the file is as follows:
 *        - First Line: Contains data attributes. It may be followed by class information (either as an ID or in one-hot format).
 *        - Second Line: Contains prediction values.
 *        - Third Line (optional): Contains class information, only if it was not included in the first line and if present.
 *        There is a blank line between each sample in the file.
 *        Each data in a line is separated by a space
 *
 *        The presence and format of class data (ID or one-hot) are inferred based on the structure of the lines in the file.
 *
 * @param name string containing the name of the dataSet
 * @param dataFile const char* data file name containing datas, predictions and maybe classes(not mendatory)
 * @param _nbAttributes int number of attributes
 * @param _nbClasses int number of classes
 * @param decisionThres double indicating the decision threshold, useful when choosing the decision (-1 for no threshold)
 * @param indexPositiveClass integer corresponding to the index of the positive class for which we have the decision threshold (-1 if no threshold)
 */
DataSetFid::DataSetFid(const string &name, const char *dataFile, int _nbAttributes, int _nbClasses, double decisionThresh, int indexPositiveCl) : datasetName(name), hasDatas(true), hasPreds(true) {

  setNbClassAndAttr(_nbClasses, _nbAttributes);

  if (decisionThresh != -1 && !std::isnan(decisionThresh)) {
    decisionThreshold = decisionThresh;
  }
  if (indexPositiveCl != -1 && !std::isnan(indexPositiveCl)) {
    indexPositiveClass = indexPositiveCl;
  }

  checkThreshold();

  fstream fileDta;
  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " not found.");
  }
  string line;
  bool firstLine = true;
  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      setDataLine(line, dataFile);
    } else if (firstLine) {
      throw FileFormatError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", first line is empty.");
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " is not on good format, there are more than one empty line between 2 samples.");
        }
      }
      break; // There are just empty lines at the end of the file
    }
    if (fileDta.eof()) {
      throw FileContentError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " has not enough prediction data.");
    }
    // Get predictions
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      setPredLine(line, dataFile);
    } else {
      while (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " is not on good format, there is empty lines inbetween data.");
        }
      }
      throw FileContentError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " has not enough prediction data.");
    }
    bool endOfFile = false;
    if (fileDta.eof()) {
      endOfFile = true;
    }
    // Get classes
    getline(fileDta, line);
    if (!endOfFile && !checkStringEmpty(line)) {
      if (classFormat == "one-hot_combined" || classFormat == "id_combined") {
        throw FileContentError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " classes are given two times in the file, with attributes and are after predictions.");
      }
      setClassLine(line, dataFile);

      if (!fileDta.eof()) {
        getline(fileDta, line);
        if (!checkStringEmpty(line)) {
          throw FileFormatError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", you need to have empty lines between samples. You have chosen to give data, predictions and classes in one file. If you want to separate them, use -p and -c.");
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
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(weightFile) + " not found.");
  }

  while (!fileWts.eof()) {
    getline(fileWts, line);
    if (!checkStringEmpty(line)) {
      std::stringstream myLine(line);
      double value;
      vector<double> tempVect;
      while (myLine >> value) {
        if (myLine.fail()) {
          throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(weightFile) + ", a number is required.");
        }
        tempVect.push_back(value);
      }
      weights.push_back(tempVect);
    }
  }
  fileWts.close(); // close file
}

/**
 * @brief Get data from dataFile and save it in datas and trueClasses if it contains class information.
 *
 *            The file should contain one sample per line. Each number in line is separated by a space. Each sample can be in one of the following formats:
 *            1. Attributes only: Each line contains each float attribute.
 *            2. Attributes with Class ID: Each line contains all the float attributes followed by an integer class ID.
 *            3. Attributes with One-Hot Class Encoding: Each line contains all the float attributes followed by a one-hot encoding of the class.
 *               The number of elements in this encoding should match the total number of classes, with exactly one '1' and the rest '0's.
 *
 * @param dataFile const char* representing the name of the data file.. This file should contain data in one of the supported formats.
 * @param _nbAttributes int specifying the number of attributes
 * @param _nbClasses int specifying the number of classes
 */
void DataSetFid::setDataFromFile(const char *dataFile, int _nbAttributes, int _nbClasses) {
  if (hasDatas == true) {
    throw InternalError("Error in dataset " + datasetName + " : datas have been given two times.");
  }
  hasDatas = true;

  setNbClassAndAttr(_nbClasses, _nbAttributes);

  string line;
  fstream fileDta;

  fileDta.open(dataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(dataFile) + " not found.");
  }

  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (!checkStringEmpty(line)) {
      setDataLine(line, dataFile);
    }
  }

  fileDta.close(); // close data file

  if (!trueClasses.empty()) {
    if (hasClasses == true) {
      throw InternalError("Error in dataset " + datasetName + " : classes have been given two times.");
    }
    hasClasses = true;
    nbClassData = static_cast<int>(trueClasses.size());
  }

  nbSamples = static_cast<int>(datas.size());

  checkDatas();
}

/**
 * @brief Add predictions to the dataset using a prediction file.
 *        The prediction file should contain one line per data sample, each line consisting of a series of numerical values separated by spaces
 *        representing the prediction scores scores for each class.
 *        The number of values per line should match the specified number of classes.
 *        If a decision threshold is provided, the function uses it to determine the predicted class based on the threshold.
 *
 * @param predFile const char* prediction file name
 * @param _nbClasses int number of classes
 * @param decisionThresh optional double indicating the decision threshold, useful when choosing the decision (-1 for no threshold)
 * @param indexPositiveClass optional integer corresponding to the index of the positive class for which we have the decision threshold (-1 if no threshold)
 */
void DataSetFid::setPredFromFile(const char *predFile, int _nbClasses, double decisionThresh, int indexPositiveCl) {

  setNbClass(_nbClasses);

  if (decisionThresh != -1 && !std::isnan(decisionThresh)) {
    if (decisionThreshold != -1) {
      decisionThreshold = decisionThresh;
    } else {
      throw InternalError("Error in dataset " + datasetName + " : decision threshold has been given two times.");
    }
  }
  if (indexPositiveCl != -1 && !std::isnan(indexPositiveCl)) {
    if (indexPositiveClass != -1) {
      indexPositiveClass = indexPositiveCl;
    } else {
      throw InternalError("Error in dataset " + datasetName + " : index of positive class has been given two times.");
    }
  }

  checkThreshold();

  if (hasPreds == true) {
    throw InternalError("Error in dataset " + datasetName + " : predictions have been given two times.");
  }
  hasPreds = true;
  string line;
  fstream filePrd;

  filePrd.open(predFile, ios::in); // read predictions data file
  if (filePrd.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(predFile) + " not found.");
  }

  while (!filePrd.eof()) {
    getline(filePrd, line);
    if (!checkStringEmpty(line)) {
      setPredLine(line, predFile);
    }
  }

  filePrd.close(); // close file
  nbPredData = static_cast<int>(predictions.size());
  checkDatas();
}

/**
 * @brief Add classes from a specified file into the dataset.
 *        The class file can contain lines in different formats:
 *        1. Class ID format: Each line contains a single integer representing the class ID.
 *        2. One-hot format: Each line contains a sequence of integers in a one-hot encoding scheme,
 *           where exactly one value is 1 (indicating the class ID) and all others are 0.
 *        Each number in a line is separated by a space
 *
 *        The function determines the format of each line based on the '_nbClasses' parameter and the structure of the data in the line.
 *
 * @param classFile A const char* representing the name of the class file. This file should contain class data
 *                  in one of the supported formats (either class ID or one-hot encoded).
 * @param _nbClasses An int specifying the number of classes.
 */
void DataSetFid::setClassFromFile(const char *classFile, int _nbClasses) {
  if (hasClasses == true) {
    throw InternalError("Error in dataset " + datasetName + " : classes have been given two times.");
  }
  hasClasses = true;

  setNbClass(_nbClasses);

  string line;
  fstream fileCl;

  fileCl.open(classFile, ios::in); // read true dataclass file
  if (fileCl.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(classFile) + " not found.");
  }

  while (!fileCl.eof()) {
    getline(fileCl, line);
    if (!checkStringEmpty(line)) {
      setClassLine(line, classFile);
    }
  }

  fileCl.close(); // close file
  nbClassData = static_cast<int>(trueClasses.size());
  checkDatas();
}

/**
 * @brief Read a data line from a data file and save it in datas and trueClasses if it contains class information.
 *        This function supports different data formats. It can process lines containing:
 *        - Only attributes: A sequence of numerical values.
 *        - Attributes followed by class IDs: A sequence of numerical values followed by an integer representing the class ID.
 *        - Attributes followed by one-hot encoded classes: A sequence of numerical values followed by a one-hot encoding representing the class.
 *          The number of elements in this encoding should match the total number of classes, with exactly one '1' and the rest '0's.
 *        Each data in a line is separated by a space
 *        The format of the class representation (ID or one-hot encoding) is determined based on the structure of the data in the line.
 *
 * @param line A string containing one line of the data file. This line can include attribute values and, optionally, class information.
 * @param dataFile A const char* representing the name of the data file. This file may contain lines of data in any of the supported formats.
 */
void DataSetFid::setDataLine(const string &line, const char *dataFile) {
  std::stringstream myLine(line);
  double valueData;
  vector<double> valuesData;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valueData;
    if (myLine.fail()) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", a number is required.");
    }
    valuesData.push_back(valueData);
  }
  auto lineSize = static_cast<int>(valuesData.size());

  // Identify class format (one-hot, id, one-hot_combined, id_combined)
  if (classFormat.empty()) {
    if (lineSize == nbAttributes + nbClasses) {
      classFormat = "one-hot_combined";
    } else if (lineSize == nbAttributes + 1) {
      classFormat = "id_combined";
    } else if (lineSize != nbAttributes) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", incorrect number of parameters per line, expected the given number of attributes (" + std::to_string(nbAttributes) + ") or this number plus 1 (classes in id format) or plus the number of classes (one-hot format).");
    }
  }

  if (classFormat == "one-hot" || classFormat == "id" || classFormat == "") { // No classes with the attributes
    if (lineSize != nbAttributes) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", the number of attributes for a sample don't match the given number of attributes (" + std::to_string(nbAttributes) + "). No class should be given with the data as there was none beforehand.");
    } else {
      datas.push_back(valuesData);
    }
  } else if (classFormat == "one-hot_combined") { // With classes on one-hot format
    if (lineSize != nbAttributes + nbClasses) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", the number of attributes and classes for a sample don't match the given number of attributes and classes (" + std::to_string(nbAttributes + nbClasses) + "). Classes should be given with datas on one-hot format as they were given like this beforehand.");
    } else {

      int oneHotIndex;
      if (isOneHot(valuesData, nbAttributes, oneHotIndex)) {
        datas.emplace_back(valuesData.begin(), valuesData.begin() + nbAttributes);
        trueClasses.emplace_back(oneHotIndex);
      } else {
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", invalid one-hot format for a given sample.");
      }
    }
  } else if (classFormat == "id_combined") { // With classes on id format
    if (lineSize != nbAttributes + 1) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", the number of attributes for a sample don't match the given number of attributes (" + std::to_string(nbAttributes) + "). The id of the class should be given at the end of the line as they were given like this beforehand.");
    } else {
      double classId = valuesData.back();
      if (classId < 0 || classId >= nbClasses || classId != std::floor(classId)) {
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(dataFile) + ", invalid class ID for a given sample.");
      }
      datas.emplace_back(valuesData.begin(), valuesData.begin() + nbAttributes);
      trueClasses.emplace_back(classId);
    }
  } else {
    throw InternalError("Error in dataset " + datasetName + " : wrong class format.");
  }
}

/**
 * @brief Read a prediction line from the prediction file and save it in predictions.
 *        Each line in the prediction file should contain a sequence of numerical values representing the prediction scores for each class.
 *        The number of values in each line must match the number of classes specified for the dataset.
 *        If a decision threshold is set, the function also determines the predicted class based on this threshold.
 *        Each line should contain numbers separated by spaces.
 *
 * @param line string containing one line of the prediction file
 * @param predFile const char* prediction file name
 */
void DataSetFid::setPredLine(const string &line, const char *predFile) {

  std::stringstream myLine(line);
  double valuePred;
  vector<double> valuesPred;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valuePred;
    if (myLine.fail()) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(predFile) + ", a number is required.");
    }
    valuesPred.push_back(valuePred);
  }
  outputValuesPredictions.push_back(valuesPred);

  if (static_cast<int>(valuesPred.size()) != nbClasses) {
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(predFile) + ", the number of predictions is not equal to the number of classes (" + std::to_string(nbClasses) + ") for each sample.");
  }

  if (decisionThreshold >= 0 && indexPositiveClass >= nbClasses) {
    throw CommandArgumentException("Error in dataset " + datasetName + " : The index of positive class cannot be greater or equal to the number of classes (" + to_string(nbClasses) + ").");
  }

  if (decisionThreshold >= 0 && valuesPred[indexPositiveClass] >= decisionThreshold) {
    predictions.push_back(indexPositiveClass);
  } else {
    predictions.push_back(static_cast<int>(std::max_element(valuesPred.begin(), valuesPred.end()) - valuesPred.begin()));
  }
}

/**
 * @brief Read a class line from the class file and save it in trueClasses.
 *        This function is designed to handle two formats of class representation in the file:
 *        1. Class ID format: The line contains a single integer representing the class ID.
 *        2. One-hot format: The line contains a sequence of integers in a one-hot encoding scheme,
 *           where exactly one value is 1 (indicating the class ID) and all others are 0.
 *        Each data in a line is separated by a space
 *
 *        The function automatically determines the format based on the structure of the data in the line.
 *
 * @param line A string containing one line from the class file. This line can either have a single integer (class ID)
 *             or multiple integers (one-hot encoded vector) representing the class information.
 * @param classFile A const char* representing the name of the class file. This file should contain lines in one of the
 *                  supported formats (either class ID or one-hot encoded).
 */
void DataSetFid::setClassLine(const string &line, const char *classFile) {
  std::stringstream myLine(line);
  double valueData;
  vector<double> valuesData;
  while (myLine >> std::ws) { // Skip leading whitespaces
    if (myLine.peek() == EOF) {
      break; // Reached the end of the stream
    }
    myLine >> valueData;
    if (myLine.fail() || valueData != std::floor(valueData)) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", an integer is required.");
    }
    valuesData.push_back(valueData);
  }
  auto lineSize = static_cast<int>(valuesData.size());

  if (classFormat == "") {
    if (lineSize == nbClasses) {
      classFormat = "one-hot";
    } else if (lineSize == 1) {
      classFormat = "id";
    } else {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", incorrect number of classes for a given sample.");
    }
  }

  if (classFormat == "one-hot") {
    if (lineSize != nbClasses) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", the number of classes for a sample doesn't match the given number of classes (" + std::to_string(nbClasses) + "). Classes should be given with datas on one-hot format as they were given like this beforehand.");
    } else {
      int oneHotIndex;
      if (isOneHot(valuesData, 0, oneHotIndex)) {
        trueClasses.emplace_back(oneHotIndex);
      } else {
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", invalid one-hot format for a given sample.");
      }
    }
  } else if (classFormat == "id") {
    if (lineSize != 1) {
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", only the id of the class should be given on the line as classes were given like this beforehand.");
    } else {
      double classId = valuesData[0];
      if (classId < 0 || classId >= nbClasses || classId != std::floor(classId)) {
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(classFile) + ", invalid class ID for a given sample.");
      }
      trueClasses.emplace_back(classId);
    }
  } else if (classFormat == "one-hot_combined" || classFormat == "id_combined") {
    throw InternalError("Error in dataset " + datasetName + " : classes were already added combined with datas.");
  } else {
    throw InternalError("Error in dataset " + datasetName + " : wrong class format.");
  }
}

/**
 * @brief Set the number of classes for the dataset
 *
 * This function sets the number of classes for the dataset.
 * If the number of classes is already set (not -1), it checks
 * if the new value matches the existing one. If not, an error is thrown.
 * If the number is not set yet, it validates that the new value is strictly positive.
 *
 * @param _nbClasses The number of classes to set.
 */
void DataSetFid::setNbClass(int _nbClasses) {
  if (nbClasses != -1) {
    if (nbClasses != _nbClasses) {
      throw InternalError("Error in dataset " + datasetName + " : the given number of classes (" + to_string(_nbClasses) + ") doesn't match the number given before (" + to_string(nbClasses) + ").");
    }
  } else {
    if (_nbClasses > 0) {
      nbClasses = _nbClasses;
    } else {
      throw InternalError("Error in dataset " + datasetName + " : the number of classes has to be a strictly positive integer.");
    }
  }
}

/**
 * @brief Set the number of attributes and classes for the dataset
 *
 * This function sets the number of attributes and classes for the dataset.
 * If the number of attributes or classes is already set (not -1), it checks
 * if the new value matches the existing one. If not, an error is thrown.
 * If the number is not set yet, it validates that the new value is strictly positive.
 *
 * @param _nbAttributes The number of attributes to set.
 * @param _nbClasses The number of classes to set.
 */
void DataSetFid::setNbClassAndAttr(int _nbClasses, int _nbAttributes) {
  if (nbAttributes != -1) {
    if (nbAttributes != _nbAttributes) {
      throw InternalError("Error in dataset " + datasetName + " : the given number of attributes (" + to_string(_nbAttributes) + ") doesn't match the number given before (" + to_string(nbAttributes) + ").");
    }
  } else {
    if (_nbAttributes > 0) {
      nbAttributes = _nbAttributes;
    } else {
      throw InternalError("Error in dataset " + datasetName + " : the number of attributes has to be a strictly positive integer.");
    }
  }

  setNbClass(_nbClasses);
}

/**
 * @brief Checks if a portion of a std::vector<double> follows a one-hot encoding scheme.
 *        In one-hot encoding, all elements must be either 0.0 or 1.0, with exactly one element being 1.0.
 *
 * @param values The vector of double values to check for one-hot encoding.
 * @param start The starting index in 'values' from where to check for one-hot encoding.
 * @param oneHotIndex Reference to an integer where the index (relative to 'start') of the 1.0 element in one-hot encoding will be stored.
 *                    This is set only if a valid one-hot encoding is found.
 * @return true If the portion of the vector starting from 'start' follows a one-hot encoding.
 * @return false If the portion of the vector does not follow a one-hot encoding,
 *               either because there's more than one 1.0, or because an element other than 0.0 or 1.0 is found.
 */
bool DataSetFid::isOneHot(const std::vector<double> &values, int start, int &oneHotIndex) const {
  bool foundOne = false; // Flag to indicate if 1.0 has been found in the specified range
  oneHotIndex = -1;      // Initialize the one-hot index to -1 (not found)

  for (int i = start; i < values.size(); ++i) {
    if (values[i] == 1.0f) { // Check if the current element is 1.0
      if (foundOne) {
        // More than one '1.0' found, not a one-hot format
        return false;
      }
      foundOne = true;         // Mark that we've found a 1.0
      oneHotIndex = i - start; // Calculate the relative index of the 1.0 element
    } else if (values[i] != 0.0f) {
      // An element other than 0.0 or 1.0 found, not a one-hot format
      return false;
    }
  }

  return foundOne; // Return true if a valid one-hot encoding was found
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : dataClass file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : prediction file not specified for this dataset.");
  }
}

/**
 * @brief Return the number of classes in the dataset
 *
 * @return int
 */
int DataSetFid::getNbClasses() const {
  if (nbClasses != -1) {
    return nbClasses;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : number of classes not specified for this dataset.");
  }
}

/**
 * @brief Return the number of attributes in the dataset
 *
 * @return int
 */
int DataSetFid::getNbAttributes() const {
  if (nbAttributes != -1) {
    return nbAttributes;
  } else {
    throw CommandArgumentException("Error in dataset " + datasetName + " : number of attributes not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : data file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : weight file not specified for this dataset.");
  }
}

/**
 * @brief Check for errors in the dataset
 *
 */
void DataSetFid::checkDatas() const {
  if (hasDatas && nbSamples < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There are no data samples.");
  }
  if (hasPreds && nbPredData < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There are no predictions.");
  }
  if (hasClasses && nbClassData < 1) {
    throw FileContentError("Error in dataset " + datasetName + " : There are no classes.");
  }

  if (hasDatas && hasPreds && nbSamples != nbPredData) {
    throw FileContentError("Error in dataset " + datasetName + " : The number of prediction data does not match the number of samples.");
  }
  if (hasClasses) {
    if (hasDatas && nbSamples != nbClassData) {
      throw FileContentError("Error in dataset " + datasetName + " : The number of class data does not match the number of samples.");
    }
    if (hasPreds && nbPredData != nbClassData) {
      throw FileContentError("Error in dataset " + datasetName + " : The number of class data does not match the number of prediction data.");
    }
  }
}

/**
 * @brief Check for errors when adding a threshold
 *
 */
void DataSetFid::checkThreshold() const {

  if (decisionThreshold != -1 && indexPositiveClass == -1) { // XOR
    throw InternalError("Error in dataset " + datasetName + " : index positive class has to be given when decisionThreshold is given.");
  }

  if (decisionThreshold != -1 && (decisionThreshold < 0 || decisionThreshold > 1)) {
    throw CommandArgumentException("Error in dataset " + datasetName + " : the decision threshold has to be a float included in [0,1].");
  }

  if (indexPositiveClass != -1 && indexPositiveClass < 0) {
    throw CommandArgumentException("Error in dataset " + datasetName + " : the index of positive class has to be a positive integer.");
  }
}

/**
 * @brief Add attributes and eventually classes from attribute file in the dataset
 *
 * @param attributeFile const char* attribute file name
 * @param _nbAttributes int number of attributes
 * @param _nbClasses optional int number of classes
 */
void DataSetFid::setAttributes(const char *attributeFile, int _nbAttributes, int _nbClasses) {
  hasAttributes = true;
  if (_nbClasses != -1) {
    setNbClassAndAttr(_nbClasses, _nbAttributes);
  } else {
    setNbClass(_nbClasses);
  }

  // Get attributes
  fstream fileAttr;
  string line;

  fileAttr.open(attributeFile, ios::in); // Read attribute file
  if (fileAttr.fail()) {
    throw FileNotFoundError("Error in dataset " + datasetName + " : file " + std::string(attributeFile) + " not found.");
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

  if (attributeNames.size() < nbAttributes) { // Not enough attributes
    throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(attributeFile) + ", there is not enough attribute names.");
  } else if (attributeNames.size() == nbAttributes) { // Exact correlation, so there is no class names.
    hasClassNames = false;
  } else {
    if (nbClasses != -1) {
      if (attributeNames.size() != nbAttributes + nbClasses) { // No correlation with the number of attributes and classes
        throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(attributeFile) + ", there is not the good amount of attribute and class names.");
      } else { // There is attributes and classes
        hasClassNames = true;
        auto firstEl = attributeNames.end() - nbClasses;
        auto lastEl = attributeNames.end();
        classNames.insert(classNames.end(), firstEl, lastEl);
        attributeNames.erase(firstEl, lastEl);
      }
    } else { // No class specified and too much attributes
      throw FileContentError("Error in dataset " + datasetName + " : in file " + std::string(attributeFile) + ", there are too much attribute names (no class has been specified, if you have classNames, add the number of classes).");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : attribute file not specified for this dataset.");
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
    throw CommandArgumentException("Error in dataset " + datasetName + " : classNames not present in attribute file or attribute file not specified.");
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
