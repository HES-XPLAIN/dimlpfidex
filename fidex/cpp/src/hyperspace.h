#ifndef HYPERSPACE_H
#define HYPERSPACE_H

#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexCommon/cpp/src/hyperbox.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

namespace FidexNameSpace {
class Hyperspace {

  std::vector<std::vector<double>> hyperLocus; // All the possible hyperplans
  std::shared_ptr<Hyperbox> hyperbox;

public:
  Hyperspace();
  explicit Hyperspace(const std::vector<std::vector<double>> &matHypLocus);

  std::shared_ptr<Hyperbox> getHyperbox() const;
  std::vector<std::vector<double>> getHyperLocus() const;
  void ruleExtraction(std::vector<double> *mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, std::vector<std::string> &lines, bool hasAttributeNames, std::vector<std::string> *attributeNames, bool hasClassNames, std::vector<std::string> *classNames);
  double computeRuleAccuracy(std::vector<int> *trainPreds, std::vector<int> *trainTrueClass, bool mainSampleCorrect = false) const;
  double computeRuleConfidence(std::vector<std::vector<double>> *trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue) const;
};
} // namespace FidexNameSpace

#endif
