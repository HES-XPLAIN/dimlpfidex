#ifndef RULE_H
#define RULE_H
// Find better way to include this
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedant.h"
#include "checkFun.h"
#include "dataSet.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Json = nlohmann::json;

class Rule {
private:
  std::vector<Antecedant> antecedants;
  std::vector<int> coveredSamples;
  int outputClass = -1;
  int coveringSize = -1;
  double fidelity = -1;
  double accuracy = -1;
  double confidence = -1;

  // define to ease JSON lib use
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rule, antecedants, coveredSamples, outputClass, coveringSize, fidelity, accuracy, confidence)

public:
  Rule() = default;
  Rule(const std::vector<Antecedant> &antecedants,
       const std::vector<int> &coveredSamples,
       int out_class,
       double fidelity,
       double accuracy,
       double confidence);

  // SETTERS
  void setOutputClass(int value) { outputClass = value; };
  void setCoveringSize(int value) { coveringSize = value; }
  void setFidelity(double value) { fidelity = value; };
  void setAccuracy(double value) { accuracy = value; };
  void setConfidence(double value) { confidence = value; };
  void setAntecedants(std::vector<Antecedant> const &values) { antecedants = values; };
  void setCoveredSamples(std::vector<int> const &values) {
    coveredSamples = values;
    coveringSize = static_cast<int>(values.size());
  };

  // GETTERS
  std::vector<Antecedant> getAntecedants() const { return antecedants; }
  int getNbAntecedants() const { return static_cast<int>(antecedants.size()); }
  std::vector<int> getCoveredSamples() const { return coveredSamples; }
  int getCoveringSize() const { return coveringSize; }
  int getOutputClass() const { return outputClass; }
  double getFidelity() const { return fidelity; }
  double getAccuracy() const { return accuracy; }
  double getConfidence() const { return confidence; }

  // UTILITIES
  void addAntecedant(Antecedant value) { antecedants.push_back(value); };
  void addCoveredSample(int value) {
    coveredSamples.push_back(value);
    coveringSize += 1;
  };
  std::string toString(const std::vector<std::string> &attributes, const std::vector<std::string> &classes) const;
  bool isEqual(const Rule &other) const;
  static std::vector<Rule> fromJsonFile(const std::string &filename);
  static void toJsonFile(const std::string &filename, const std::vector<Rule> &rules, float threshold, int positiveIndex);
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
inline std::ostream &operator<<(std::ostream &stream, const Rule &rule) {
  for (Antecedant a : rule.getAntecedants())
    stream << a;
  stream << std::endl;
  stream << "   #Antecedants: " << rule.getAntecedants().size() << std::endl
         << "   #Covered samples: " << rule.getCoveredSamples().size() << std::endl
         << "   Confidence: " << rule.getConfidence() << std::endl
         << "   Accuracy:   " << rule.getAccuracy() << std::endl
         << "   Covering:   " << rule.getCoveredSamples().size() << std::endl;

  return stream;
}

inline bool operator==(const Rule &r1, const Rule &r2) {
  return r1.isEqual(r2);
}

inline bool operator!=(const Rule &r1, const Rule &r2) {
  return !r1.isEqual(r2);
}

// FUNCTIONS TO HANDLE RULES PATTERNS AND FILES
std::string getAntStrPatternWithAttrIds(int nbAttributes);
std::string getAntStrPatternWithAttrNames(const std::vector<std::string> &attributeNames);
std::string getStrPatternWithClassIds(int nbClasses);
std::string getStrPatternWithClassNames(const std::vector<std::string> &classNames);
std::vector<bool> getRulePatternsFromString(const std::string &str, DataSetFid &dataset);
std::vector<bool> getRulesPatternsFromRuleFile(const std::string &rulesFile, DataSetFid &dataset, bool withClasses = true);
bool stringToRule(Rule &rule, const std::string &str, bool withAttributeNames, bool withClassNames, DataSetFid &dataset);
void getRules(std::vector<Rule> &rules, const std::string &rulesFile, DataSetFid &dataset);
std::tuple<double, double> writeRulesFile(const std::string &filename, const std::vector<Rule> &rules, const std::vector<std::string> &attributeNames,
                                          const std::vector<std::string> &classNames, float threshold, int positiveIndex);
void getActivatedRules(std::vector<int> &activatedRules, std::vector<Rule> &rules, std::vector<double> &testValues);
void getThresholdFromRulesFile(const std::string &filePath, float &decisionThreshold, int &positiveClassIndex);

#endif
