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

class Hyperspace {
  std::vector<std::vector<double>> hyperLocus; // All the possible hyperplans
  std::shared_ptr<Hyperbox> hyperbox;

public:
  Hyperspace();
  explicit Hyperspace(const std::vector<std::vector<double>> &matHypLocus);

  std::shared_ptr<Hyperbox> getHyperbox() const;
  std::vector<std::vector<double>> getHyperLocus() const;
  Rule ruleExtraction(std::vector<double> &mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, const std::vector<double> &mus = std::vector<double>(), const std::vector<double> &sigmas = std::vector<double>(), const std::vector<int> &normalizationIndices = std::vector<int>());
  double computeRuleAccuracy(std::vector<int> &trainPreds, std::vector<int> &trainTrueClass, bool hasTrueClasses, bool mainSampleCorrect = false) const;
  double computeRuleConfidence(std::vector<std::vector<double>> &trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue = -1.0) const;
};

#endif
