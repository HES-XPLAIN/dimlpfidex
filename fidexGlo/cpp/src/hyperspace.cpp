#include "hyperspace.h"
using namespace std;

namespace FidexGloNameSpace {
Hyperspace::Hyperspace(const std::vector<std::vector<double>> &matHypLocus) : hyperLocus(matHypLocus) {

  vector<pair<int, int>> discriminativeHyperplans;
  hyperbox = std::make_shared<Hyperbox>(discriminativeHyperplans);
}

vector<vector<double>> Hyperspace::getHyperLocus() const {
  return hyperLocus;
}

std::shared_ptr<Hyperbox> Hyperspace::getHyperbox() const {
  return hyperbox;
}

tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> Hyperspace::ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence) {

  tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule;

  double hypValue;
  int attribut;
  bool inequalityBool;

  vector<tuple<int, bool, double>> antecedants;
  for (int k = 0; k < hyperbox->getDiscriminativeHyperplans().size(); k++) {
    attribut = hyperbox->getDiscriminativeHyperplans()[k].first % (*mainSampleData).size();
    hypValue = hyperLocus[hyperbox->getDiscriminativeHyperplans()[k].first][hyperbox->getDiscriminativeHyperplans()[k].second];
    double mainSampleValue = (*mainSampleData)[attribut];
    if (hypValue <= mainSampleValue) {
      inequalityBool = true;
    } else {
      inequalityBool = false;
    }
    antecedants.push_back(make_tuple(attribut, inequalityBool, hypValue));
  }
  rule = make_tuple(antecedants, hyperbox->getCoveredSamples(), mainSamplePred, ruleAccuracy, ruleConfidence);

  return rule;
}

double Hyperspace::computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass) const { // Percentage of correct model prediction on samples covered by the rule
  int idSample;
  int total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]) {
      total += 1;
    }
  }
  size_t nbCovered = hyperbox->getCoveredSamples().size();
  return float(total) / static_cast<double>(nbCovered);
}

double Hyperspace::computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int mainSamplePred) const { // Mean output value of prediction of class chosen by the rule for the covered samples
  int idSample;
  double total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    total += (*trainOutputValuesPredictions)[idSample][mainSamplePred]; // Value of output prediction for class mainSamplePred(rule class)
  }

  size_t nbCovered = hyperbox->getCoveredSamples().size();
  return float(total) / static_cast<double>(nbCovered);
}
} // namespace FidexGloNameSpace
