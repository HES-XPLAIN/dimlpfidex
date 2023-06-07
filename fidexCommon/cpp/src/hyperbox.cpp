#include "hyperbox.h"
using namespace std;

Hyperbox::Hyperbox(const vector<pair<int, int>> &m_discriminativeHyperplans)
    : discriminativeHyperplans(m_discriminativeHyperplans) {
}

Hyperbox::Hyperbox() = default;

void Hyperbox::setCoveredSamples(const vector<int> &m_coveredSamples) {
  coveredSamples = m_coveredSamples;
}

vector<pair<int, int>> Hyperbox::getDiscriminativeHyperplans() const {
  return discriminativeHyperplans;
}

void Hyperbox::computeCoveredSamples(const vector<int> &ancienCoveredSamples, int attribut, vector<vector<double>> *trainData, bool mainSampleGreater, double hypValue) {
  vector<int> newCoveredSamples;
  for (int idCoveredSample : ancienCoveredSamples) { // We check all already covered samples
    double sampleValue = (*trainData)[idCoveredSample][attribut];
    bool sampleGreater = hypValue <= sampleValue;
    if (mainSampleGreater == sampleGreater) {       // If both samples are on same side of hyperplan
      newCoveredSamples.push_back(idCoveredSample); // This sample is covered again
    }
  }
  coveredSamples = newCoveredSamples;
}

vector<int> Hyperbox::getCoveredSamples() const {
  return coveredSamples;
}

void Hyperbox::computeFidelity(const int mainsamplePred, vector<int> *trainPreds) {
  int coveredTrueClass = 0;                          // Number of samples covered by the hyperbox and of same class as the example
  size_t nbCovered = coveredSamples.size();          // Number of samples covered by the hyperbox
  for (int idSample : coveredSamples) {              // Loop on all covered samples
    if (mainsamplePred == (*trainPreds)[idSample]) { // Check if sample is of right class (class predicted by dimlp network for our main sample)
      coveredTrueClass += 1;
    }
  }

  fidelity = (double)coveredTrueClass / (double)nbCovered;
}

double Hyperbox::getFidelity() const {
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
