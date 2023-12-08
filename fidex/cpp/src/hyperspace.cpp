#include "hyperspace.h"
using namespace std;

namespace FidexNameSpace {
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

void Hyperspace::ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, vector<string> &lines, bool hasAttributeNames, vector<string> *attributeNames, bool hasClassNames, vector<string> *classNames) {
  double hypValue;
  int attribut;

  string line;
  string inequality;
  for (int k = 0; k < hyperbox->getDiscriminativeHyperplans().size(); k++) {

    attribut = hyperbox->getDiscriminativeHyperplans()[k].first % (*mainSampleData).size();
    hypValue = hyperLocus[hyperbox->getDiscriminativeHyperplans()[k].first][hyperbox->getDiscriminativeHyperplans()[k].second];
    double mainSampleValue = (*mainSampleData)[attribut];
    if (hypValue <= mainSampleValue) {
      inequality = ">=";
    } else {
      inequality = "<";
    }
    if (hasAttributeNames) {
      line += (*attributeNames)[attribut];
    } else {
      line += "X" + std::to_string(attribut);
    }
    line += inequality + formattingDoubleToString(hypValue) + " ";
  }
  if (hasClassNames) {
    line += "-> " + (*classNames)[mainSamplePred];
  } else {
    line += "-> class " + std::to_string(mainSamplePred);
  }
  std::cout << "Extracted rule :" << std::endl;
  std::cout << line << endl;
  lines.push_back(line);
  line = "Train Covering size : " + std::to_string(hyperbox->getCoveredSamples().size());
  std::cout << line << endl;
  lines.push_back(line);
  line = "Train Fidelity : " + formattingDoubleToString(hyperbox->getFidelity());
  std::cout << line << endl;
  lines.push_back(line);
  line = "Train Accuracy : " + formattingDoubleToString(ruleAccuracy);
  std::cout << line << endl;
  lines.push_back(line);
  if (ruleConfidence != -1) {
    line = "Train Confidence : " + formattingDoubleToString(ruleConfidence);
    std::cout << line << endl;
    lines.push_back(line);
  }
  std::cout << std::endl;
}

double Hyperspace::computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass, bool mainSampleCorrect) const { // Percentage of correct model prediction on samples covered by the rule
  int idSample;
  int total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]) {
      total += 1;
    }
  }
  if (mainSampleCorrect) { // Add test sample value
    total += 1;
  }
  size_t nbCovered = hyperbox->getCoveredSamples().size();
  nbCovered += 1;

  return float(total) / static_cast<double>(nbCovered);
}

double Hyperspace::computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int rulePred, double mainSamplePredValueOnRulePred) const { // Mean output value of prediction of class chosen by the rule(which is the main sample prediction) for the covered samples
  int idSample;
  double total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    total += (*trainOutputValuesPredictions)[idSample][rulePred]; // value of output prediction of class chosen by the rule (rulePred)
  }

  total += mainSamplePredValueOnRulePred; // Add test sample prediction value on rule prediction (which is the same as his own prediction)

  size_t nbCovered = hyperbox->getCoveredSamples().size();
  nbCovered += 1;

  return float(total) / static_cast<double>(nbCovered);
}
} // namespace FidexNameSpace
