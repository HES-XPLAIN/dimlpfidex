#ifndef HYPERBOX_H
#define HYPERBOX_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace FidexNameSpace {

/**
 * @class Hyperbox
 * @brief Represents a hyperbox in a discriminative hyperbox algorithm.
 */
class Hyperbox {
  std::vector<std::pair<int, int>> discriminativeHyperplans; ///< The discriminative hyperplans.
  std::vector<int> coveredSamples;                           ///< The covered samples.
  double fidelity = -1;                                      ///< The fidelity of the hyperbox.

public:
  /**
   * @brief Default constructor for Hyperbox.
   */
  Hyperbox();

  /**
   * @brief Constructor for Hyperbox with initial discriminative hyperplans.
   * @param m_discriminativeHyperplans The initial discriminative hyperplans.
   */
  explicit Hyperbox(const std::vector<std::pair<int, int>> &m_discriminativeHyperplans);

  /**
   * @brief Set the covered samples for the hyperbox.
   * @param m_coveredSamples The covered samples.
   */
  void setCoveredSamples(const std::vector<int> &m_coveredSamples);

  /**
   * @brief Get the discriminative hyperplans.
   * @return The discriminative hyperplans.
   */
  std::vector<std::pair<int, int>> getDiscriminativeHyperplans() const;

  /**
   * @brief Reset the discriminative hyperplans to an empty state.
   */
  void resetDiscriminativeHyperplans();

  /**
   * @brief Compute the new covered samples with a new discriminative hyperplan.
   * @param ancienCoveredSamples The previously covered samples.
   * @param attribut The attribute associated with the hyperplan.
   * @param trainData The training data.
   * @param mainSampleGreater Flag indicating if the main sample is greater than the hyperplan value.
   * @param hypValue The value of the hyperplan.
   */
  void computeCoveredSamples(const std::vector<int> &ancienCoveredSamples, int attribut, std::vector<std::vector<double>> *trainData, bool mainSampleGreater, double hypValue);

  /**
   * @brief Compute the fidelity of the hyperbox.
   * @param mainsamplePred The prediction of the main sample.
   * @param trainPreds The predictions of the training samples.
   */
  void computeFidelity(const int mainsamplePred, std::vector<int> *trainPreds);

  /**
   * @brief Get the fidelity of the hyperbox.
   * @return The fidelity of the hyperbox.
   */
  double getFidelity() const;

  /**
   * @brief Set the fidelity of the hyperbox.
   * @param x The fidelity value to set.
   */
  void setFidelity(double x);

  /**
   * @brief Get the covered samples of the hyperbox.
   * @return The covered samples.
   */
  std::vector<int> getCoveredSamples() const;

  /**
   * @brief Add a discriminative hyperplan to the hyperbox.
   * @param i The first attribute index of the hyperplan.
   * @param j The second attribute index of the hyperplan.
   */
  void discriminateHyperplan(int i, int j);
};
} // namespace FidexNameSpace
#endif
