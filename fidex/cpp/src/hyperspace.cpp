#include "hyperspace.h"

/**
 * @brief Constructs a Hyperspace object with a specified 2D vector of doubles of possible hyperplanes.
 *
 * @param matHypLocus 2D vector of doubles representing all the possible hyperplanes.
 */
Hyperspace::Hyperspace(const std::vector<std::vector<double>> &matHypLocus) : hyperLocus(matHypLocus) {
  std::vector<std::pair<int, int>> discriminativeHyperplans;
  hyperbox = std::make_shared<Hyperbox>(discriminativeHyperplans);
}

/**
 * @brief Gets the 2D vector of doubles of possible hyperplanes in the hyperspace.
 *
 * @return 2D vector of doubles representing all the possible hyperplanes.
 */
std::vector<std::vector<double>> Hyperspace::getHyperLocus() const {
  return hyperLocus;
}

/**
 * @brief Gets the hyperbox associated with the hyperspace.
 *
 * @return Shared pointer to the Hyperbox object.
 */
std::shared_ptr<Hyperbox> Hyperspace::getHyperbox() const {
  return hyperbox;
}

/**
 * @brief Extracts a rule from the hyperspace based on the main sample data and its prediction.
 *
 * @param mainSampleData Data of the main sample.
 * @param mainSamplePred Prediction of the main sample.
 * @param ruleAccuracy Accuracy of the rule.
 * @param ruleConfidence Confidence of the rule.
 * @param mus Means for the denormalization of the rule values (optional).
 * @param sigmas Standard deviations for the denormalization of the rule values (optional).
 * @param normalizationIndices Indices for the denormalization of the rule values (optional).
 * @return A Rule object representing the extracted rule.
 */
Rule Hyperspace::ruleExtraction(std::vector<double> &mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, const std::vector<double> &mus, const std::vector<double> &sigmas, const std::vector<int> &normalizationIndices) {

  bool denormalizing = false;
  // Check if we need to denormalize
  if (!mus.empty() && !sigmas.empty() && !normalizationIndices.empty()) {
    denormalizing = true;
    if (!(mus.size() == sigmas.size() && mus.size() == normalizationIndices.size())) {
      throw InternalError("Error during rule extraction : Means, standard deviations, and normalization indices must have the same number of elements.");
    }
  } else if (!mus.empty() || !sigmas.empty() || !normalizationIndices.empty()) {
    throw InternalError("Error during rule extraction : Means, standard deviations, and normalization indices must either all be specified or none at all.");
  }

  double hypValue;
  int attribut;
  bool inequalityBool;
  std::vector<Antecedent> antecedents;

  for (int k = 0; k < hyperbox->getDiscriminativeHyperplans().size(); k++) {

    attribut = hyperbox->getDiscriminativeHyperplans()[k].first % mainSampleData.size();
    hypValue = hyperLocus[hyperbox->getDiscriminativeHyperplans()[k].first][hyperbox->getDiscriminativeHyperplans()[k].second];
    double mainSampleValue = mainSampleData[attribut];

    if (hypValue <= mainSampleValue) {
      inequalityBool = true;
    } else {
      inequalityBool = false;
    }

    // Denormalization of values in case it was previously normalized
    if (denormalizing) {
      // Check if the attribute needs to be denormalized
      int index = -1;
      for (size_t i = 0; i < normalizationIndices.size(); ++i) {
        if (normalizationIndices[i] == attribut) {
          index = static_cast<int>(i);
          break;
        }
      }
      if (index != -1) {
        hypValue = hypValue * sigmas[index] + mus[index];
      }
    }

    antecedents.push_back(Antecedent(attribut, inequalityBool, hypValue));
  }
  return Rule(antecedents, hyperbox->getCoveredSamples(), mainSamplePred, hyperbox->getFidelity(), ruleAccuracy, ruleConfidence);
}

/**
 * @brief Computes the accuracy of the rule with respect to the training predictions and true classes of the covered samples.
 *
 * @param trainPreds Predictions of the training data.
 * @param trainTrueClass True classes of the training data.
 * @param hasTrueClasses Boolean indicating if true classes are available.
 * @param mainSampleCorrect Boolean indicating if the main sample is correctly classified.
 * @return The accuracy of the rule.
 */
double Hyperspace::computeRuleAccuracy(std::vector<int> &trainPreds, std::vector<int> &trainTrueClass, bool hasTrueClasses, bool mainSampleCorrect) const { // Percentage of correct model prediction on samples covered by the rule

  int total = 0; // Number of indexes predicted good
  std::vector<int> coveredSamples = hyperbox->getCoveredSamples();

  for (int idSample : coveredSamples) {
    if (trainPreds[idSample] == trainTrueClass[idSample]) {
      total += 1;
    }
  }

  if (hasTrueClasses && mainSampleCorrect) { // Add test sample value
    total += 1;
  }

  size_t nbCovered = coveredSamples.size();

  if (hasTrueClasses) {
    nbCovered += 1;
  }

  return static_cast<double>(total) / static_cast<double>(nbCovered);
}

/**
 * @brief Computes the confidence of the rule with respect to the main sample prediction and training output values of the covered samples.
 *
 * @param trainOutputValuesPredictions Output values of the training data predictions.
 * @param rulePred Prediction of the rule.
 * @param mainSamplePredValueOnRulePred Output value of the main sample prediction (optional).
 * @return The confidence of the rule.
 */
double Hyperspace::computeRuleConfidence(std::vector<std::vector<double>> &trainOutputValuesPredictions, const int rulePred, double mainSamplePredValueOnRulePred) const { // Mean output value of prediction of class chosen by the rule(which is the main sample prediction) for the covered samples

  double total = 0; // Number of indexes predicted good

  std::vector<int> coveredSamples = hyperbox->getCoveredSamples();
  // Value of output prediction for class mainSamplePred(rule class)
  for (int idSample : coveredSamples) {
    total += trainOutputValuesPredictions[idSample][rulePred]; // Value of output prediction for class rulePred(rule class)
  }

  size_t nbCovered = coveredSamples.size();

  if (mainSamplePredValueOnRulePred != -1.0) { // If we compute Fidex with a test sample
    total += mainSamplePredValueOnRulePred;    // Add test sample prediction value on rule prediction (which is the same as his own prediction)
    nbCovered += 1;
  }
  return total / static_cast<double>(nbCovered);
}
