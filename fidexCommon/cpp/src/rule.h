#ifndef RULE_H
#define RULE_H
#include "antecedant.h"
#include "checkFun.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Rule {
private:
  vector<Antecedant> _antecedants;
  vector<int> _coveredSamples;
  int _outClass;
  double _fidelity;
  double _accuracy;
  double _confidence;

  // PRIVATE SETTERS
  void setAntecedants(vector<Antecedant> antecedants) { _antecedants = antecedants; };
  void setCoveredSamples(vector<int> coveredSamples) { _coveredSamples = coveredSamples; };

public:
  Rule() = default;
  Rule(vector<Antecedant> antecedants,
       vector<int> coveredSamples,
       int out_class,
       double fidelity,
       double accuracy,
       double confidence);

  // SETTERS
  void setOutputClass(int outputClass) { _outClass = outputClass; };
  void setFidelity(double fidelity) { _fidelity = fidelity; };
  void setAccuracy(double accuracy) { _accuracy = accuracy; };
  void setConfidence(double confidence) { _confidence = confidence; };

  // GETTERS
  vector<Antecedant> getAntecedants() const { return _antecedants; }
  vector<int> getCoveredSamples() const { return _coveredSamples; }
  int getOutputClass() const { return _outClass; }
  double getFidelity() const { return _fidelity; }
  double getAccuracy() const { return _accuracy; }
  double getConfidence() const { return _confidence; }

  // UTILITIES
  void addAntecedant(Antecedant antecedant) { _antecedants.push_back(antecedant); };
  void addCoveredSample(int sampleId) { _coveredSamples.push_back(sampleId); };
  string toString(const vector<string> *attributes = NULL, const vector<string> *classes = NULL);
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

#endif
