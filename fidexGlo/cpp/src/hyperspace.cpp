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

// MODIFIED WHITH NEW STRUCTURE
Rule Hyperspace::ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence) {
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
    antecedants.push_back(Antecedant(attribut, inequalityBool, hypValue));
  }

  return Rule(antecedants, hyperbox->getCoveredSamples(), mainSamplePred, ruleAccuracy, ruleConfidence);
}

double Hyperspace::computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass) const { // Percentage of correct model prediction on samples covered by the rule
  int idSample;
  int total = 0; // Number of indexes predicted good
  vector<int> coveredSamples = hyperbox->getCoveredSamples();

  // cout << "HYPERBOX Covered samples: (size=" << coveredSamples.size() << ")" << endl;
  // for (int i : coveredSamples) {
  //   cout << i << ", ";
  // }
  // cout << endl;

  for (int i = 0; i < coveredSamples.size(); i++) {
    idSample = coveredSamples[i];
    // cout << "trainPreds[" << idSample << "] == trainTrueClass[" << idSample << "]" << endl;
    // cout << (*trainPreds)[idSample] << " == " << (*trainTrueClass)[idSample] << endl
    //      << endl;

    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]) {
      total += 1;
    }
  }

  int nbCovered = coveredSamples.size();
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
