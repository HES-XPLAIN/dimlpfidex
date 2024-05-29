#ifndef HYPERSPACEGLO_H
#define HYPERSPACEGLO_H

#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/rule.h"
#include "hyperbox.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

/**
 * @brief Represents a hyperspace composed of all the possible hyperplanes in this space and that manages a hyperbox and its associated discriminative hyperplanes.
 *
 * This class encapsulates the attributes and methods needed to handle a hyperspace which includes a hyperbox
 * and the possible hyperplanes in the feature space that discriminate between different classes of samples.
 */
class Hyperspace {
  std::vector<std::vector<double>> hyperLocus; ///< Matrix representing all the possible hyperplanes.
  std::shared_ptr<Hyperbox> hyperbox;          ///< Shared pointer to the Hyperbox of the Hyperspace.

public:
  /**
   * @brief Default constructor for Hyperspace.
   */
  Hyperspace();

  /**
   * @brief Constructs a Hyperspace object with a specified matrix of possible hyperplanes.
   */
  explicit Hyperspace(const std::vector<std::vector<double>> &matHypLocus);

  /**
   * @brief Gets the hyperbox associated with the hyperspace.
   */
  std::shared_ptr<Hyperbox> getHyperbox() const;

  /**
   * @brief Gets the matrix of possible hyperplanes in the hyperspace.
   */
  std::vector<std::vector<double>> getHyperLocus() const;

  /**
   * @brief Extracts a rule from the hyperspace based on the main sample data and its prediction.
   */
  Rule ruleExtraction(std::vector<double> &mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, const std::vector<double> &mus = std::vector<double>(), const std::vector<double> &sigmas = std::vector<double>(), const std::vector<int> &normalizationIndices = std::vector<int>());

  /**
   * @brief Computes the accuracy of the rule with respect to the training predictions and true classes of the covered samples.
   */
  double computeRuleAccuracy(std::vector<int> &trainPreds, std::vector<int> &trainTrueClass, bool hasTrueClasses, bool mainSampleCorrect = false) const;

  /**
   * @brief Computes the confidence of the rule with respect to the main sample prediction and training output values of the covered samples.
   */
  double computeRuleConfidence(std::vector<std::vector<double>> &trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue = -1.0) const;
};

#endif // HYPERSPACEGLO_H
