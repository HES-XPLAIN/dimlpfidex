#ifndef DIMLP_H
#define DIMLP_H

#include "bpNN.h"
#include <vector>

extern const std::string netType; // Extern declaration of the netType variable

/**
 * @brief The Dimlp class represents a Discretized Interpretable Multi-Layer Perceptron.
 *
 * This class provides functionality for training and evaluating a Dimlp neural network,
 * which is a type of neural network that includes discretized levels for interpretability.
 */
class Dimlp : public BpNN {

  //---------------------------------------------------------------------

public:
  /**
   * @brief Computes the error and accuracy of the Dimlp network on a given dataset.
   *
   * @param data The dataset to evaluate.
   * @param target The target values corresponding to the dataset.
   * @param accuracy Pointer to store the computed accuracy.
   * @return The computed error.
   */
  float Error(DataSet &data, DataSet &target, float *accuracy)

  { return ComputeError(data, target, accuracy); }

  /**
   * @brief Trains the Dimlp network using the provided datasets.
   *
   * @param train The training dataset.
   * @param trainTar The target values for the training dataset.
   * @param test The testing dataset.
   * @param testTar The target values for the testing dataset.
   * @param valid The validation dataset.
   * @param validTar The target values for the validation dataset.
   * @param accuracyFile The file to save accuracy metrics.
   * @param fromBT Flag indicating if training is done with bagging.
   */
  void Train(DataSet &train,
             DataSet &trainTar,
             DataSet &test,
             DataSet &testTar,
             DataSet &valid,
             DataSet &validTar,
             const std::string &accuracyFile,
             bool fromBT = false)

  { TrainPhase(train, trainTar, test, testTar, valid, validTar, accuracyFile, fromBT); }

  //---------------------------------------------------------------------

  /**
   * @brief Virtual destructor for Dimlp.
   */
  ~Dimlp() override = default; // Virtual destructor

  /**
   * @brief Constructs a Dimlp network with specified parameters.
   */
  Dimlp(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int discrLevels,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &weightFile,
      int seed = 0);

  /**
   * @brief Constructs a Dimlp network by reading weights from a file.
   */
  Dimlp(const std::string &readFile, int nbLayers, const std::vector<int> &nbNeurons, int discrLevels, int netId = 1);

  /**
   * @brief Constructs a Dimlp network by reading weights from a file and sets training parameters.
   */
  Dimlp(
      const std::string &readFile,
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int discrLevels,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &weightFile,
      int seed = 0);
};

#endif // DIMLP_H
