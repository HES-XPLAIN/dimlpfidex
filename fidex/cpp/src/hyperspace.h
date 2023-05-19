#ifndef HYPERSPACE_H
#define HYPERSPACE_H

#include "hyperbox.h"
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

namespace FidexNameSpace {
class Hyperspace {

  vector<vector<double>> hyperLocus; // All the possible hyperplans
  Hyperbox *hyperbox;

public:
  Hyperspace();
  Hyperspace(const char *hyperLocusFile);

  Hyperbox *getHyperbox();
  vector<vector<double>> getHyperLocus();
  void ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, vector<string> &lines, bool hasAttributeNames, vector<string> *attributeNames, bool hasClassNames, vector<string> *classNames);
  double computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass, bool mainSampleCorrect = 0);
  double computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue);
};
} // namespace FidexNameSpace

#endif
