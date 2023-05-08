using namespace std;
#include "hyperspace.h"
#include <fstream>
#include <sstream>

namespace FidexGloNameSpace {
Hyperspace::Hyperspace(char *hyperLocusFile) {

  // Get hyperLocus
  fstream fileLocus;

  string line;

  fileLocus.open(hyperLocusFile, ios::in); // Read hyperLocus file
  if (fileLocus.fail()) {
    throw std::runtime_error("Error : file " + std::string(hyperLocusFile) + " not found");
  }

  while (!fileLocus.eof()) {
    getline(fileLocus, line);
    if (line.length() != 0) {
      std::stringstream myLine(line);
      double value;
      vector<double> tempVect;
      while (myLine >> value) {
        tempVect.push_back(value);
      }
      hyperLocus.push_back(tempVect);
    }
  }
  fileLocus.close(); // close file

  vector<pair<int, int>> discriminativeHyperplans;

  hyperbox = new Hyperbox(discriminativeHyperplans);
}

vector<vector<double>> Hyperspace::getHyperLocus() {
  return hyperLocus;
}

Hyperbox *Hyperspace::getHyperbox() {
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
      inequalityBool = 1;
    } else {
      inequalityBool = 0;
    }
    antecedants.push_back(make_tuple(attribut, inequalityBool, hypValue));
  }
  rule = make_tuple(antecedants, hyperbox->getCoveredSamples(), mainSamplePred, ruleAccuracy, ruleConfidence);

  return rule;
}

double Hyperspace::computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass, bool mainSampleCorrect) { // Percentage of correct model prediction on samples covered by the rule
  int idSample;
  int total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]) {
      total += 1;
    }
  }
  int nbCovered = hyperbox->getCoveredSamples().size();
  return float(total) / nbCovered;
}

double Hyperspace::computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int mainSamplePred) { // Mean output value of prediction of class chosen by the rule for the covered samples
  int idSample;
  int classSample;
  double total = 0; // Number of indexes predicted good
  for (int i = 0; i < hyperbox->getCoveredSamples().size(); i++) {
    idSample = hyperbox->getCoveredSamples()[i];
    total += (*trainOutputValuesPredictions)[idSample][mainSamplePred]; // Value of output prediction for class mainSamplePred(rule class)
  }

  int nbCovered = hyperbox->getCoveredSamples().size();
  return float(total) / nbCovered;
}
} // namespace FidexGloNameSpace
