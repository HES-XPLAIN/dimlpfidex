#ifndef HYPERBOX_H
#define HYPERBOX_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Hyperbox {
  std::vector<std::pair<int, int>> discriminativeHyperplans; // Ex: 16x50
  std::vector<int> coveredSamples;
  double fidelity = -1;

public:
  Hyperbox();
  explicit Hyperbox(const std::vector<std::pair<int, int>> &m_discriminativeHyperplans);

  void setCoveredSamples(const std::vector<int> &m_coveredSamples);
  std::vector<std::pair<int, int>> getDiscriminativeHyperplans() const;
  void resetDiscriminativeHyperplans();

  // Get new covered samples with a new discriminative hyperplan
  void computeCoveredSamples(const std::vector<int> &ancienCoveredSamples, int attribut, std::vector<std::vector<double>> *trainData, bool mainSampleGreater, double hypValue);

  void computeFidelity(const int mainsamplePred, std::vector<int> *trainPreds);
  double getFidelity() const;
  void setFidelity(double x);
  std::vector<int> getCoveredSamples() const;
  void discriminateHyperplan(int i, int j);
};

#endif
