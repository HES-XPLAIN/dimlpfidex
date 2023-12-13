#ifndef HYPERSPACEGLO_H
#define HYPERSPACEGLO_H

// #include "../../../fidexCommon/cpp/src/antecedant.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperbox.h"
#include "../../../fidexCommon/cpp/src/rule.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

namespace FidexGloNameSpace {
class Hyperspace {
  vector<vector<double>> hyperLocus; // All the possible hyperplans
  shared_ptr<Hyperbox> hyperbox;

public:
  Hyperspace();
  explicit Hyperspace(const vector<vector<double>> &matHypLocus);

  shared_ptr<Hyperbox> getHyperbox() const;
  vector<vector<double>> getHyperLocus() const;
  Rule ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence);
  double computeRuleAccuracy(vector<int> *trainPreds, vector<int> *trainTrueClass) const;
  double computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions, const int mainSamplePred) const;
};
} // namespace FidexGloNameSpace

#endif
