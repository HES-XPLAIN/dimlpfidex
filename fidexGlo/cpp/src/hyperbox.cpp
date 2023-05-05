using namespace std;
#include "hyperbox.h"
#include <fstream>
#include <sstream>
#include <string>

Hyperbox::Hyperbox(vector<pair<int, int>> m_discriminativeHyperplans) {
  discriminativeHyperplans = m_discriminativeHyperplans;
}

Hyperbox::Hyperbox() {
}

void Hyperbox::setCoveredSamples(vector<int> m_coveredSamples) {
  coveredSamples = m_coveredSamples;
}

vector<pair<int, int>> Hyperbox::getDiscriminativeHyperplans() {
  return discriminativeHyperplans;
}

void Hyperbox::computeCoveredSamples(vector<int> ancienCoveredSamples, int attribut, vector<vector<double>> *trainData, bool mainSampleGreater, double hypValue) {
  vector<int> newCoveredSamples;
  for (int l = 0; l < ancienCoveredSamples.size(); l++) { // We check all already covered samples
    int idCoveredSample = ancienCoveredSamples[l];
    double sampleValue = (*trainData)[idCoveredSample][attribut];
    bool sampleGreater = hypValue <= sampleValue;
    if (!(mainSampleGreater ^ sampleGreater)) {     // If both samples are on same side of hyperplan (^ = xor)
      newCoveredSamples.push_back(idCoveredSample); // This sample is covered again
    }
  }
  coveredSamples = newCoveredSamples;
}

vector<int> Hyperbox::getCoveredSamples() {
  return coveredSamples;
}

void Hyperbox::computeFidelity(const int mainsamplePred, vector<int> *trainPreds) {
  int coveredTrueClass = 0;                         // Number of samples covered by the hyperbox and of same class as the example
  int nbCovered = coveredSamples.size();            // Number of samples covered by the hyperbox
  for (int k = 0; k < coveredSamples.size(); k++) { // Loop on all covered samples
    int idSample = coveredSamples[k];
    if (mainsamplePred == (*trainPreds)[idSample]) { // Check if sample is of right class (class predicted by dimlp network for our main sample)
      coveredTrueClass += 1;
    }
  }
  fidelity = (double)coveredTrueClass / (double)nbCovered;
}

double Hyperbox::getFidelity() {
  return fidelity;
}

void Hyperbox::setFidelity(double x) {
  fidelity = x;
}

void Hyperbox::discriminateHyperplan(int i, int j) {
  discriminativeHyperplans.push_back(make_pair(i, j));
}

void Hyperbox::resetDiscriminativeHyperplans() {
  vector<pair<int, int>> disc;
  discriminativeHyperplans = disc;
}
