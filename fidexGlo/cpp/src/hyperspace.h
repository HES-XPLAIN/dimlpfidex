#ifndef HYPERSPACEGLO_H
#define HYPERSPACEGLO_H

#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperbox.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

namespace FidexGloNameSpace {
class Hyperspace {

  std::vector<std::vector<double>> hyperLocus; // All the possible hyperplans
  std::shared_ptr<Hyperbox> hyperbox;

public:
  Hyperspace();
  explicit Hyperspace(const std::vector<std::vector<double>> &matHypLocus);

  std::shared_ptr<Hyperbox> getHyperbox() const;
  std::vector<std::vector<double>> getHyperLocus() const;
  std::tuple<std::vector<std::tuple<int, bool, double>>, std::vector<int>, int, double, double> ruleExtraction(std::vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence);
  double computeRuleAccuracy(std::vector<int> *trainPreds, std::vector<int> *trainTrueClass) const;
  double computeRuleConfidence(std::vector<std::vector<double>> *trainOutputValuesPredictions, const int mainSamplePred) const;
};
} // namespace FidexGloNameSpace

#endif
