using namespace std;
#include <vector>
#include <iostream>
#include <cstring>

class Hyperbox 
{
      vector<pair<int, int>> discriminativeHyperplans;
      vector<int> coveredSamples;
      double fidelity = -1;


public:

      Hyperbox();
      Hyperbox(vector<pair<int, int>> m_discriminativeHyperplans);

      void setCoveredSamples(vector<int> m_coveredSamples);
      vector<pair<int, int>> getDiscriminativeHyperplans();
      void resetDiscriminativeHyperplans();
      
      // Get new covered samples with a new discriminative hyperplan
      void computeCoveredSamples(vector<int> ancienCoveredSamples, int attribut, vector<vector<double>>* trainData, bool mainSampleGreater, double hypValue,vector<double>* mainSampleData);

      void computeFidelity(const int mainsamplePred, vector<int>* trainPreds);
      double getFidelity();
      void setFidelity(double x);
      vector<int> getCoveredSamples();
      void discriminateHyperplan(int i, int j);


};
