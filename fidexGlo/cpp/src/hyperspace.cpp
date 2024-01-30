#include "hyperspace.h"
using namespace std;

namespace FidexGloNameSpace {
Hyperspace::Hyperspace(const vector<vector<double>> &matHypLocus) : hyperLocus(matHypLocus) {

  vector<pair<int, int>> discriminativeHyperplans;
  hyperbox = make_shared<Hyperbox>(discriminativeHyperplans);
}

vector<vector<double>> Hyperspace::getHyperLocus() const {
  return hyperLocus;
}

shared_ptr<Hyperbox> Hyperspace::getHyperbox() const {
  return hyperbox;
}

// MODIFIED WITH NEW STRUCTURE
Rule Hyperspace::ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, const vector<double> *mus, const vector<double> *sigmas, const vector<int> *normalization_indices) {

  bool denormalizing = false;
  // Check if we need to denormalize
  if (mus && sigmas && normalization_indices) {
    denormalizing = true;
    if (!(mus->size() == sigmas->size() && mus->size() == normalization_indices->size())) {
      throw InternalError("Error during rule extraction : Means, standard deviations, and normalization indices must have the same number of elements.");
    }
  } else if (mus || sigmas || normalization_indices) {
    throw InternalError("Error during rule extraction : Means, standard deviations, and normalization indices must either all be specified or none at all.");
  }

  double hypValue;
  int attribut;
  bool inequalityBool;
  vector<Antecedant> antecedants;

  for (int k = 0; k < hyperbox->getDiscriminativeHyperplans().size(); k++) {

    attribut = hyperbox->getDiscriminativeHyperplans()[k].first % (*mainSampleData).size();
    hypValue = hyperLocus[hyperbox->getDiscriminativeHyperplans()[k].first][hyperbox->getDiscriminativeHyperplans()[k].second];
    double mainSampleValue = (*mainSampleData)[attribut];

    if (hypValue <= mainSampleValue) {
      inequalityBool = true;
    } else {
      inequalityBool = false;
    }

    // Denormalization of values in case it was previously normalized
    if (denormalizing) {
      // Check if the attribute needs to be denormalized
      int index = -1;
      for (size_t i = 0; i < normalization_indices->size(); ++i) {
        if ((*normalization_indices)[i] == attribut) {
          index = static_cast<int>(i);
          break;
        }
      }
      if (index != -1) {
        hypValue = hypValue * (*sigmas)[index] + (*mus)[index];
      }
    }

    antecedants.push_back(Antecedant(attribut, inequalityBool, hypValue));
  }
  return Rule(antecedants, hyperbox->getCoveredSamples(), mainSamplePred, hyperbox->getFidelity(), ruleAccuracy, ruleConfidence);
}

double Hyperspace::computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass, bool hasTrueClasses, bool mainSampleCorrect) const { // Percentage of correct model prediction on samples covered by the rule

  std::cout << hasTrueClasses << " " << mainSampleCorrect << std::endl;
  int idSample;
  int total = 0; // Number of indexes predicted good
  vector<int> coveredSamples = hyperbox->getCoveredSamples();

  for (int i = 0; i < coveredSamples.size(); i++) {
    idSample = coveredSamples[i];
    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]) {
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

  return float(total) / float(nbCovered);
}

double Hyperspace::computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int rulePred) const { // Mean output value of prediction of class chosen by the rule(which is the main sample prediction) for the covered samples
  int idSample;
  double total = 0; // Number of indexes predicted good

  vector<int> coveredSamples = hyperbox->getCoveredSamples();
  // Value of output prediction for class mainSamplePred(rule class)
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    total += (*trainOutputValuesPredictions)[idSample][rulePred]; // Value of output prediction for class rulePred(rule class)
  }

  size_t nbCovered = coveredSamples.size();
  return float(total) / float(nbCovered);
}
} // namespace FidexGloNameSpace
