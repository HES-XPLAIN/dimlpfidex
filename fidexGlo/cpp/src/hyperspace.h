#include "hyperbox.h"
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

class Hyperspace {

  vector<vector<double>> hyperLocus; // All the possible hyperplans
  Hyperbox *hyperbox;

public:
  Hyperspace();
  Hyperspace(char *hyperLocusFile);

  Hyperbox *getHyperbox();
  vector<vector<double>> getHyperLocus();
  tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>
  ruleExtraction(vector<double> *mainSampleData, const int mainSamplePred,
                 double ruleAccuracy, double ruleConfidence);
  double computeRuleAccuracy(vector<int> *trainPreds,
                             vector<int> *trainTrueClass,
                             bool mainSampleCorrect);
  double
  computeRuleConfidence(vector<vector<double>> *trainOutputValuesPredictions,
                        const int mainSamplePred);
};
