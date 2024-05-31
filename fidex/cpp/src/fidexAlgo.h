#ifndef FIDEXALGO_H
#define FIDEXALGO_H
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/parameters.h"
#include "hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <numeric>
#include <omp.h>
#include <random>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

/**
 * @brief Class to implement the Fidex algorithm for rule extraction.
 *
 * Fidex is an algorithm designed to extract a rule that explains the decisions of a machine learning
 * model on a specific sample. It works by identifying hyperplanes in the feature space that discriminate
 * between different classes of samples and constructing a rule based on these hyperplanes.
 */
class Fidex {

public:
  // Struct to handle special parameters
  /**
   * @brief Structure to handle some parameters of the current execution.
   */
  struct currentExecutionSpecs {
    bool showInitialFidelity = false;  ///< Flag to indicate if the initial fidelity should be shown.
    double mainSamplePredValue = -1.0; ///< The predicted value of the sample of interest.
    int nbIt = 0;                      ///< The number of iterations of the algorithm.
  };

private:
  DataSetFid *_trainDataset;   ///< Pointer to the training dataset.
  Parameters *_parameters;     ///< Pointer to the parameters object.
  Hyperspace *_hyperspace;     ///< Pointer to the hyperspace.
  bool _usingTestSamples;      ///< Flag to indicate if test samples are being used.
  std::mt19937 _rnd;           ///< Random number generator.
  currentExecutionSpecs specs; ///< Instance of the currentExecutionSpecs structure to store special parameters.

  // UTILS

  /**
   * @brief Attempts to compute a rule with Fidex algorithm based on given parameters and updates the rule object if successful.
   */
  bool tryComputeFidex(Rule &rule, std::vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, bool verbose, bool detailedVerbose = false, bool foundRule = false);

  /**
   * @brief Performs a dichotomic (binary) search to find a rule with the best covering that meets the minimum fidelity criteria.
   */
  int dichotomicSearch(Rule &bestRule, std::vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int mainSampleClass, int left, int right, bool verbose);

  /**
   * @brief Attempts to compute a rule multiple times up to a maximum number of failed attempts, adjusting fidelity if necessary.
   */
  bool retryComputeFidex(Rule &rule, std::vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, bool verbose);

public:
  /**
   * @brief Default constructor for Fidex.
   */
  Fidex() = default;

  /**
   * @brief Constructs a Fidex object with the given training dataset, parameters, and hyperspace and sets the random seed.
   */
  Fidex(DataSetFid &_trainDataset, Parameters &parameters, Hyperspace &Hyperspace, bool usingTestSamples);

  // execute algo
  /**
   * @brief Executes the Fidex algorithm to compute an explaining rule for the given sample based on the training samples and hyperlocus and directed by the given parameters.
   */
  bool compute(Rule &rule, std::vector<double> &mainSampleValues, int mainSamplePred, double minFidelity, int minNbCover, int mainSampleClass = -1);

  // SETTERS

  /**
   * @brief Sets the flag to show the initial fidelity.
   *
   * @param value Boolean value to set the flag.
   */
  void setShowInitialFidelity(bool value) {
    specs.showInitialFidelity = value;
  }

  /**
   * @brief Sets the predicted value of the main sample.
   *
   * @param value Double value representing the predicted value.
   */
  void setMainSamplePredValue(double value) {
    specs.mainSamplePredValue = value;
  }

  /**
   * @brief Sets the number of iterations of the algorithm.
   *
   * @param value Integer value representing the number of iterations.
   */
  void setNbIt(int value) {
    specs.nbIt = value;
  }

  // GETTERS

  /**
   * @brief Gets the flag indicating whether to show the initial fidelity.
   *
   * @return Boolean value indicating the flag state.
   */
  bool getShowInitialFidelity() const {
    return specs.showInitialFidelity;
  }

  /**
   * @brief Gets the predicted value of the main sample.
   *
   * @return Double value representing the predicted value.
   */
  double getMainSamplePredValue() const {
    return specs.mainSamplePredValue;
  }

  /**
   * @brief Gets the number of iterations of the algorithm.
   *
   * @return Integer value representing the number of iterations.
   */
  int getNbIt() const {
    return specs.nbIt;
  }

  // UTILS

  /**
   * @brief Launches the Fidex algorithm with specified parameters to attempt creating a rule for the given sample that meets given minimum covering and minimum fidelity criteria.
   */
  bool launchFidex(Rule &rule, std::vector<double> &mainSampleValues, int mainSamplePred, int mainSampleClass, bool verbose = false);
};

#endif // FIDEXALGO_H
