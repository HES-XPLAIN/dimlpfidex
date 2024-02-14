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

using namespace std;

class Hyperspace {
  vector<vector<double>> hyperLocus; // All the possible hyperplans
  shared_ptr<Hyperbox> hyperbox;

public:
  Hyperspace();
  explicit Hyperspace(const vector<vector<double>> &matHypLocus);

  shared_ptr<Hyperbox> getHyperbox() const;
  vector<vector<double>> getHyperLocus() const;
  Rule ruleExtraction(vector<double> &mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, const vector<double> &mus = std::vector<double>(), const vector<double> &sigmas = std::vector<double>(), const vector<int> &normalizationIndices = std::vector<int>());
  double computeRuleAccuracy(vector<int> &trainPreds, vector<int> &trainTrueClass, bool hasTrueClasses, bool mainSampleCorrect = false) const;
  double computeRuleConfidence(vector<vector<double>> &trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue = -1.0) const;
};

#endif
