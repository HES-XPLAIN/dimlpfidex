#ifndef RULE_H
#define RULE_H
// Find better way to include this
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedant.h"
#include "checkFun.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using Json = nlohmann::json;

class Rule {
private:
  vector<Antecedant> antecedants;
  vector<int> coveredSamples;
  int outputClass;
  int coveringSize;
  double fidelity;
  double accuracy;
  double confidence;

  // define to ease JSON lib use
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rule, antecedants, coveredSamples, outputClass, coveringSize, fidelity, accuracy, confidence)

public:
  Rule() = default;
  Rule(const vector<Antecedant> &antecedants,
       const vector<int> &coveredSamples,
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
  void setAntecedants(vector<Antecedant> const &values) { antecedants = values; };
  void setCoveredSamples(vector<int> const &values) {
    coveredSamples = values;
    coveringSize = static_cast<int>(values.size());
  };

  // GETTERS
  vector<Antecedant> getAntecedants() const { return antecedants; }
  int getNbAntecedants() const { return static_cast<int>(antecedants.size()); }
  vector<int> getCoveredSamples() const { return coveredSamples; }
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
  string toString(const vector<string> &attributes, const vector<string> &classes) const;
  bool isEqual(const Rule &other) const;
  static vector<Rule> fromJsonFile(const string &filename);
  static void toJsonFile(const string &filename, const vector<Rule> &rules);
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
inline ostream &operator<<(ostream &stream, const Rule &rule) {
  for (Antecedant a : rule.getAntecedants())
    stream << a;
  stream << endl;
  stream << "   #Antecedants: " << rule.getAntecedants().size() << endl
         << "   #Covered samples: " << rule.getCoveredSamples().size() << endl
         << "   Confidence: " << rule.getConfidence() << endl
         << "   Accuracy:   " << rule.getAccuracy() << endl
         << "   Covering:   " << rule.getCoveredSamples().size() << endl;

  return stream;
}

inline bool operator==(const Rule &r1, const Rule &r2) {
  return r1.isEqual(r2);
}

inline bool operator!=(const Rule &r1, const Rule &r2) {
  return !r1.isEqual(r2);
}

// FUNCTIONS TO HANDLE RULES FILES
std::string getAntStrPatternWithAttrIds(int nbAttributes);
std::string getAntStrPatternWithAttrNames(const std::vector<std::string> &attributeNames);
std::string getStrPatternWithClassIds(int nbClasses);
std::string getStrPatternWithClassNames(const std::vector<std::string> &classNames);
std::vector<bool> getRulePatternsFromString(const std::string &str, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames);
std::vector<bool> getRulesPatternsFromRuleFile(const std::string &rulesFile, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames);
bool stringToRule(Rule &rule, const std::string &str, bool withAttributeNames, bool withClassNames, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames);
void getRulesPlus(std::vector<Rule> &rules, const std::string &rulesFile, const vector<string> &attributeNames, const vector<string> &classNames, int nbAttributes, int nbClasses);

#endif
