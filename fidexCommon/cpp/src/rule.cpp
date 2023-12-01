#include "rule.h"

Rule::Rule(vector<Antecedant> antecedants, vector<int> coveredSamples, int out_class, double accuracy, double confidence) {
  setAntecedants(antecedants);
  setCoveredSamples(coveredSamples);
  setOutputClass(out_class);
  setAccuracy(accuracy);
  setConfidence(confidence);
}
