#ifndef DATASETFID_H
#define DATASETFID_H

#include "checkFun.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

/**
 * @class DataSetFid
 * @brief Represents a dataset with associated data, true classes, predictions, and weights.
 */
class DataSetFid {

  std::vector<std::vector<double>> datas;                   ///< The data values.
  std::vector<int> trueClasses;                             ///< The true classes.
  std::vector<int> predictions;                             ///< The predicted classes.
  std::vector<std::vector<double>> outputValuesPredictions; ///< The output values of predictions.
  std::vector<std::vector<double>> weights;                 ///< The weights.
  bool hasDatas = false;                                    ///< Flag indicating if the dataset has data.
  bool hasClasses = false;                                  ///< Flag indicating if the dataset has true classes.
  bool hasWeights = false;                                  ///< Flag indicating if the dataset has weights.

public:
  /**
   * @brief Default constructor for DataSetFid.
   */
  DataSetFid();

  /**
   * @brief Constructor for DataSetFid that reads data, prediction, and true class files.
   * @param dataFile The file containing the data.
   * @param predFile The file containing the predictions.
   * @param trueClassFile The optional file containing the true classes. Pass nullptr if not available.
   */
  DataSetFid(const char *dataFile, const char *predFile, const char *trueClassFile = nullptr);

  /**
   * @brief Constructor for DataSetFid that reads a weight file.
   * @param weightFile The file containing the weights.
   */
  explicit DataSetFid(const char *weightFile);

  /**
   * @brief Get the data values.
   * @return A pointer to the vector of data values.
   * @throws std::runtime_error if the dataset does not have data.
   */
  std::vector<std::vector<double>> *getDatas();

  /**
   * @brief Get the true classes.
   * @return A pointer to the vector of true classes.
   * @throws std::runtime_error if the dataset does not have true classes.
   */
  std::vector<int> *getTrueClasses();

  /**
   * @brief Get the predicted classes.
   * @return A pointer to the vector of predicted classes.
   * @throws std::runtime_error if the dataset does not have predictions.
   */
  std::vector<int> *getPredictions();

  /**
   * @brief Get the output values of predictions.
   * @return A pointer to the vector of output values of predictions.
   * @throws std::runtime_error if the dataset does not have predictions.
   */
  std::vector<std::vector<double>> *getOutputValuesPredictions();

  /**
   * @brief Get the weights.
   * @return The vector of weights.
   * @throws std::runtime_error if the dataset does not have weights.
   */
  std::vector<std::vector<double>> getWeights() const;

  /**
   * @brief Get the input biases.
   * @return The vector of input biases.
   * @throws std::runtime_error if the dataset does not have weights.
   */
  std::vector<double> getInBiais() const;

  /**
   * @brief Get the input weights.
   * @return The vector of input weights.
   * @throws std::runtime_error if the dataset does not have weights.
   */
  std::vector<double> getInWeights() const;
};

/**
 * @class Attribute
 * @brief Represents attributes associated with a dataset.
 */
class Attribute {

  std::vector<std::string> attributes; ///< The attributes.
  bool hasAttributes = true;           ///< Flag indicating if the dataset has attributes.

public:
  /**
   * @brief Default constructor for Attribute.
   */
  Attribute();

  /**
   * @brief Constructor for Attribute that reads an attribute file.
   * @param attributeFile The file containing the attributes.
   */
  explicit Attribute(const char *attributFile);

  /**
   * @brief Get the attributes.
   * @return A pointer to the vector of attributes.
   * @throws std::runtime_error if the dataset does not have attributes.
   */
  std::vector<std::string> *getAttributes();
};

#endif
