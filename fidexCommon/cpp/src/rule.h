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
  double fidelity;
  double accuracy;
  double confidence;

  // define to ease JSON lib use
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rule, antecedants, coveredSamples, outputClass, fidelity, accuracy, confidence)

public:
  Rule() = default;
  Rule(vector<Antecedant> antecedants,
       vector<int> coveredSamples,
       int out_class,
       double fidelity,
       double accuracy,
       double confidence);

  // SETTERS
  void setOutputClass(int value) { outputClass = value; };
  void setFidelity(double value) { fidelity = value; };
  void setAccuracy(double value) { accuracy = value; };
  void setConfidence(double value) { confidence = value; };
  void setAntecedants(vector<Antecedant> values) { antecedants = values; };
  void setCoveredSamples(vector<int> values) { coveredSamples = values; };

  // GETTERS
  vector<Antecedant> getAntecedants() const { return antecedants; }
  vector<int> getCoveredSamples() const { return coveredSamples; }
  int getOutputClass() const { return outputClass; }
  double getFidelity() const { return fidelity; }
  double getAccuracy() const { return accuracy; }
  double getConfidence() const { return confidence; }

  // UTILITIES
  void addAntecedant(Antecedant value) { antecedants.push_back(value); };
  void addCoveredSample(int value) { coveredSamples.push_back(value); };
  string toString(const vector<string> &attributes, const vector<string> &classes);
  bool isEqual(const Rule other) const;
  static vector<Rule> fromJsonFile(string filename);
  static void toJsonFile(string filename, vector<Rule> rules);
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

#endif
