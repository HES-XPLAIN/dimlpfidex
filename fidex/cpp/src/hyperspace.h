#include "hyperbox.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <tuple>


class Hyperspace 
{

    vector<vector<double>> hyperLocus; // All the possible hyperplans
    Hyperbox* hyperbox;


public:

    Hyperspace();
    Hyperspace(char* hyperLocusFile);

    Hyperbox* getHyperbox();
    vector<vector<double>> getHyperLocus();
    void ruleExtraction(vector<double>* mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, vector<string> &lines);
    double computeRuleAccuracy(vector<int>* trainPreds, vector<int>* trainTrueClass, bool mainSampleCorrect = 0);
    double computeRuleConfidence(vector<vector<double>>* trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue);

};



