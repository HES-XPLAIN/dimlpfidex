#include "rule.h"

Rule::Rule(vector<Antecedant> antecedants, vector<int> coveredSamples, int out_class, double accuracy, double confidence) {
  setAntecedants(antecedants);
  setCoveredSamples(coveredSamples);
  setOutputClass(out_class);
  setAccuracy(accuracy);
  setConfidence(confidence);
}

ostream &operator<<(ostream &stream, const Rule &rule) {
  for (Antecedant a : rule.getAntecedants())
    stream << a;

  stream << "   Confidence: " << rule.getConfidence() << endl
         << "   Accuracy:   " << rule.getAccuracy() << endl
         << "   Covering:   " << rule.getCoveredSamples().size() << endl;

  return stream;
}
