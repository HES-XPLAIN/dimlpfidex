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

/**
 * @brief The DataSetFid class handles the loading, parsing, and storage of datasets including data, predictions, and classes.
 */
class DataSetFid {

private:
  std::string datasetName;                                  ///< Name of the dataset
  std::vector<std::vector<double>> datas;                   ///< Data samples
  std::vector<int> trueClasses;                             ///< True class labels for the data samples
  std::vector<int> predictions;                             ///< Predictions for the data samples
  std::vector<std::vector<double>> outputValuesPredictions; ///< Raw prediction values for the data samples
  std::vector<std::vector<std::vector<double>>> weights;    ///< Weights for the dataset
  bool hasDatas = false;                                    ///< Flag indicating if data is loaded
  bool hasPreds = false;                                    ///< Flag indicating if predictions are loaded
  bool hasClasses = false;                                  ///< Flag indicating if classes are loaded
  bool hasWeights = false;                                  ///< Flag indicating if weights are loaded

  std::string classFormat = ""; ///< Format of the class representation (one-hot, id, one-hot_combined, id_combined)

  double decisionThreshold = -1; ///< Decision threshold for classifying samples
  int positiveClassIndex = -1;   ///< Index of the positive class used for threshold-based classification

  int _nbClasses = -1;    ///< Number of classes in the dataset
  int _nbAttributes = -1; ///< Number of attributes in the dataset
  int nbSamples = -1;     ///< Number of samples in the dataset
  int nbClassData = -1;   ///< Number of class data entries
  int nbPredData = -1;    ///< Number of prediction data entries
  int nbNets = 1;         ///< Number of networks used in training found in the weight file

  std::vector<std::string> attributeNames; ///< Names of the attributes
  std::vector<std::string> classNames;     ///< Names of the classes
  bool hasAttributes = false;              ///< Flag indicating if attribute names are loaded
  bool hasClassNames = false;              ///< Flag indicating if class names are loaded

  void setDataLine(const std::string &line, const std::string &dataFile);
  void setPredLine(const std::string &line, const std::string &dataFile);
  void setClassLine(const std::string &line, const std::string &dataFile);

  void setNbClass(int nbClasses);
  void setNbClassAndAttr(int nbClasses, int nbAttributes);

  bool isOneHot(const std::vector<double> &values, int start, int &oneHotIndex) const;

  void checkDatas() const;
  void checkThreshold() const;

public:
  /**
   * @brief Construct a new DataSetFid object with a name.
   * @param name The name of the dataset.
   */
  explicit DataSetFid(const std::string &name) : datasetName(name){};

  /**
   * @brief Construct a new DataSetFid object using separate data, prediction, and optional class files.
   * @param name The name of the dataset.
   * @param dataFile The name of the data file.
   * @param predFile The name of the prediction file.
   * @param nbAttributes The number of attributes.
   * @param nbClasses The number of classes.
   * @param decisionThresh The decision threshold.
   * @param indexPositiveCl The index of the positive class.
   * @param trueClassFile The name of the class file (optional).
   */
  DataSetFid(const std::string &name, const std::string &dataFile, const std::string &predFile, int nbAttributes, int nbClasses, double decisionThresh, int indexPositiveCl, const std::string &trueClassFile = "");

  /**
   * @brief Construct a new DataSetFid object using a single data file containing data, predictions, and optionally classes.
   * @param name The name of the dataset.
   * @param dataFile The name of the data file.
   * @param nbAttributes The number of attributes.
   * @param nbClasses The number of classes.
   * @param decisionThresh The decision threshold.
   * @param indexPositiveCl The index of the positive class.
   */
  DataSetFid(const std::string &name, const std::string &dataFile, int nbAttributes, int nbClasses, double decisionThresh, int indexPositiveCl); // dataFile with data, predictions and maybe classes

  /**
   * @brief Construct a new DataSetFid object using a weight file.
   * @param name The name of the dataset.
   * @param weightFile The name of the weight file.
   */
  explicit DataSetFid(const std::string &name, const std::string &weightFile);

  /**
   * @brief Parses a weight file containing a single network's weights and stores them in the weights vector.
   * @param fileWts Reference to the file stream opened for reading the weight file.
   */
  void parseSingleNetwork(std::fstream &fileWts);

  /**
   * @brief Parses a weight file containing multiple networks' weights and stores them in the weights vector.
   * @param fileWts Reference to the file stream opened for reading the weight file.
   */
  void parseMultipleNetworks(std::fstream &fileWts);

  /**
   * @brief Get data from dataFile and save it in datas and trueClasses if it contains class information.
   * @param dataFile The name of the data file.
   * @param nbAttributes The number of attributes.
   * @param nbClasses The number of classes.
   */
  void setDataFromFile(const std::string &dataFile, int nbAttributes, int nbClasses);

  /**
   * @brief Add predictions to the dataset using a prediction file.
   * @param predFile The name of the prediction file.
   * @param nbClasses The number of classes.
   * @param decisionThreshold The decision threshold (optional).
   * @param positiveClassIndex The index of the positive class (optional).
   */
  void setPredFromFile(const std::string &predFile, int nbClasses, double decisionThreshold = -1, int positiveClassIndex = -1);

  /**
   * @brief Add classes from a specified file into the dataset.
   * @param classFile The name of the class file.
   * @param nbClasses The number of classes.
   */
  void setClassFromFile(const std::string &classFile, int nbClasses);

  /**
   * @brief Return the samples' data.
   * @return std::vector<std::vector<double>>&
   */
  std::vector<std::vector<double>> &getDatas();

  /**
   * @brief Return the classes of the samples.
   * @return std::vector<int>&
   */
  std::vector<int> &getClasses();

  /**
   * @brief Return whether the dataset contains classes.
   * @return bool
   */
  bool getHasClasses() const;

  /**
   * @brief Return the predictions of the samples.
   * @return std::vector<int>&
   */
  std::vector<int> &getPredictions();

  /**
   * @brief Return the prediction output values of the samples.
   * @return std::vector<std::vector<double>>&
   */
  std::vector<std::vector<double>> &getOutputValuesPredictions();

  /**
   * @brief Return the number of classes in the dataset.
   * @return int
   */
  int getNbClasses() const;

  /**
   * @brief Return the number of attributes in the dataset.
   * @return int
   */
  int getNbAttributes() const;

  /**
   * @brief Return the number of samples in the dataset.
   * @return int
   */
  int getNbSamples() const;

  /**
   * @brief Add attributes and optionally classes from an attribute file into the dataset.
   * @param attributesFile The name of the attribute file.
   * @param nbAttributes The number of attributes.
   * @param nbClasses The number of classes (optional).
   */
  void setAttributes(const std::string &attributesFile, int nbAttributes, int nbClasses = -1);

  /**
   * @brief Return attribute names.
   * @return std::vector<std::string>&
   */
  std::vector<std::string> &getAttributeNames();

  /**
   * @brief Return class names.
   * @return std::vector<std::string>&
   */
  std::vector<std::string> &getClassNames();

  /**
   * @brief Return whether the dataset contains attribute names.
   * @return bool
   */
  bool getHasAttributeNames() const;

  /**
   * @brief Return whether the dataset contains class names.
   * @return bool
   */
  bool getHasClassNames() const;

  /**
   * @brief Return the weights.
   * @return std::vector<std::vector<std::vector<double>>>
   */
  std::vector<std::vector<std::vector<double>>> getWeights() const;

  /**
   * @brief Return the biases of the first layer.
   * @return std::vector<double>
   */
  std::vector<double> getInBiais(int netId) const;

  /**
   * @brief Return the weights of the first layer.
   *
   * @return std::vector<double>
   */
  std::vector<double> getInWeights(int netId) const;

  /**
   * @brief Return the number of training networks.
   *
   * @return int
   */
  int getNbNets() const;
};

#endif
