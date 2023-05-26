#ifndef HYPERSPACE_H
#define HYPERSPACE_H

#include "hyperbox.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

namespace FidexNameSpace {

/**
 * The Hyperspace class represents the space of hyperplans and provides
 * functionality to extract rules and compute rule accuracy and confidence.
 */
class Hyperspace {
  std::vector<std::vector<double>> hyperLocus; ///< All the possible hyperplans
  Hyperbox *hyperbox;

public:
  /**
   * Default constructor for the Hyperspace class.
   */
  Hyperspace();

  /**
   * Constructs a Hyperspace object with the hyperLocus data loaded from the specified file.
   *
   * @param hyperLocusFile The path to the file containing the hyperLocus data.
   * @throws std::runtime_error If the specified file is not found.
   */
  explicit Hyperspace(const char *hyperLocusFile);

  /**
   * Retrieves the Hyperbox object associated with this Hyperspace.
   *
   * @return A pointer to the Hyperbox object.
   */
  Hyperbox *getHyperbox();

  /**
   * Retrieves the hyperLocus data.
   *
   * @return A vector of vectors representing the hyperLocus data.
   */
  std::vector<std::vector<double>> getHyperLocus() const;

  /**
   * Extracts a rule based on the main sample data and prediction, and computes rule accuracy and confidence.
   *
   * @param mainSampleData A pointer to the vector containing the main sample data.
   * @param mainSamplePred The prediction for the main sample.
   * @param ruleAccuracy The computed rule accuracy.
   * @param ruleConfidence The computed rule confidence.
   * @param lines A reference to a vector to store the extracted rule lines.
   * @param hasAttributeNames Indicates whether attribute names are available.
   * @param attributeNames A pointer to a vector containing the attribute names.
   * @param hasClassNames Indicates whether class names are available.
   * @param classNames A pointer to a vector containing the class names.
   */
  void ruleExtraction(std::vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, std::vector<std::string> &lines, bool hasAttributeNames, std::vector<std::string> *attributeNames, bool hasClassNames, std::vector<std::string> *classNames);

  /**
   * Computes the accuracy of the rule based on the predicted and true classes of the covered samples.
   *
   * @param trainPreds A pointer to the vector containing the predicted classes for the training samples.
   * @param trainTrueClass A pointer to the vector containing the true classes for the training samples.
   * @param mainSampleCorrect Indicates whether the main sample prediction is correct.
   * @return The computed rule accuracy.
   */
  double computeRuleAccuracy(std::vector<int> *trainPreds, std::vector<int> *trainTrueClass, bool mainSampleCorrect = false) const;

  /**
   * Computes the confidence of the rule based on the output values of the covered samples.
   *
   * @param trainOutputValuesPredictions A pointer to the vector containing the output values and predictions for the training samples.
   * @param mainSamplePred The prediction for the main sample.
   * @param mainSamplePredValue The output value for the main sample prediction.
   * @return The computed rule confidence.
   */
  double computeRuleConfidence(std::vector<std::vector<double>> *trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue) const;
};
} // namespace FidexNameSpace

#endif
