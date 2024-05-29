#ifndef HYPERBOX_H
#define HYPERBOX_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Represents a hyperbox composed of discriminative hyperplanes in the feature space used to construct an explaining rule for a sample of interest, discriminating between different classes of samples.
 *
 * This class encapsulates the attributes and methods needed to handle hyperboxes
 * which include discriminative hyperplanes and the samples they cover.
 */
class Hyperbox {
  std::vector<std::pair<int, int>> discriminativeHyperplans; ///< Vector of discriminative hyperplanes represented by a dimension (attribute index) and a hyperplane value in this dimension.
  std::vector<int> coveredSamples;                           ///< Vector of sample IDs covered by the hyperbox.
  double fidelity = -1;                                      ///< Fidelity of the samples covered by the hyperbox with respect to the model's prediction.

public:
  /**
   * @brief Default constructor for Hyperbox.
   */
  Hyperbox();

  /**
   * @brief Constructs a Hyperbox object with specified discriminative hyperplanes.
   */
  explicit Hyperbox(const std::vector<std::pair<int, int>> &m_discriminativeHyperplans);

  /**
   * @brief Sets the covered samples of the hyperbox.
   */
  void setCoveredSamples(const std::vector<int> &m_coveredSamples);

  /**
   * @brief Gets the discriminative hyperplanes of the hyperbox.
   */
  std::vector<std::pair<int, int>> getDiscriminativeHyperplans() const;

  /**
   * @brief Resets the discriminative hyperplanes of the hyperbox.
   */
  void resetDiscriminativeHyperplans();

  /**
   * @brief Computes the new covered samples with a new discriminative hyperplane.
   */
  void computeCoveredSamples(const std::vector<int> &ancienCoveredSamples, int attribut, std::vector<std::vector<double>> &trainData, bool mainSampleGreater, double hypValue);

  /**
   * @brief Computes the fidelity of the samples covered by the hyperbox with respect to the model's prediction.
   */
  void computeFidelity(const int mainsamplePred, std::vector<int> &trainPreds);

  /**
   * @brief Gets the fidelity of the samples covered by the hyperbox.
   */
  double getFidelity() const;

  /**
   * @brief Sets the fidelity of the samples covered by the hyperbox.
   */
  void setFidelity(double x);

  /**
   * @brief Gets the covered samples of the hyperbox.
   */
  std::vector<int> getCoveredSamples() const;

  /**
   * @brief Adds a new discriminative hyperplane to the hyperbox.
   */
  void discriminateHyperplan(int dimVal, int hypVal);
};

#endif // HYPERBOX_H
