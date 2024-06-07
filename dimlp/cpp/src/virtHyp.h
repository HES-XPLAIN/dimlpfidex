#ifndef VIRTHYP_H
#define VIRTHYP_H
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////

/**
 * @brief The VirtualHyp class represents virtual hyperplanes used for discretization.
 *
 * This class provides functionality for creating and managing virtual hyperplanes,
 * which are used to discretize continuous input variables for neural networks.
 */
class VirtualHyp {
  int NbBins;  ///< Number of bins for discretization.
  int NbKnots; ///< Number of knots in the staircase function.

  int NbIn;     ///< Number of input variables.
  int Multiple; ///< Multiple of the input variables.
  int NbHyp;    ///< Number of hyperplanes.

  float *Bias;    ///< Pointer to the biases.
  float *Weights; ///< Pointer to the weights.

  std::vector<std::vector<float>> VecVirtHyp;     ///< Vector of virtual hyperplanes.
  std::vector<std::vector<float>> VirtGoLeftEps;  ///< Vector of epsilon values for slightly left-shifted hyperplanes.
  std::vector<std::vector<float>> VirtGoRightEps; ///< Vector of epsilon values for slightly right-shifted hyperplanes.

  //----------------------------------------------------------------

  /**
   * @brief Creates the structure for storing virtual hyperplanes.
   */
  void CreateVirtualHyp();

  /**
   * @brief Sets the virtual hyperplanes based on biases and weights.
   */
  void SetVirtualHyp();

  /**
   * @brief Sorts the virtual hyperplanes for each input variable.
   */
  void SortVirtualHyp();

  /**
   * @brief Sets the epsilon values for slightly shifted virtual hyperplanes.
   */
  void SetEpsVirt();

  //----------------------------------------------------------------

public:
  /**
   * @brief Gets the virtual hyperplane for a specific variable.
   *
   * @param var The index of the variable.
   * @return A reference to the vector of virtual hyperplane values.
   */
  std::vector<float> &GetVirtHyp(int var) { return VecVirtHyp[var]; }

  /**
   * @brief Gets the epsilon values for slightly left-shifted hyperplanes for a specific variable.
   *
   * @param var The index of the variable.
   * @return A reference to the vector of epsilon values for slightly left-shifted hyperplanes.
   */
  std::vector<float> &GetEpsGoLeft(int var) { return VirtGoLeftEps[var]; }

  /**
   * @brief Gets the epsilon values for slightly right-shifted hyperplanes for a specific variable.
   *
   * @param var The index of the variable.
   * @return A reference to the vector of epsilon values for slightly right-shifted hyperplanes.
   */
  std::vector<float> &GetEpsGoRight(int var) { return VirtGoRightEps[var]; }

  /**
   * @brief Finds the index of the lower nearest knot (threshold) in the virtual hyperplane for a given value.
   */
  int KnotInd(int var, float val);

  /**
   * @brief Gets the index of the knot corresponding to a given value in the virtual hyperplanes.
   */
  int GetInd(int var, float val);

  /**
   * @brief Constructs a VirtualHyp object with the given parameters.
   */
  VirtualHyp(int nbBins, int nbIn, int multiple, float *bias, float *weights);

  /**
   * @brief Constructs a VirtualHyp object by merging multiple VirtualHyp objects.
   */
  VirtualHyp(
      int nbBins,
      int nbIn,
      int multiple,
      int nbNets,
      std::vector<std::shared_ptr<VirtualHyp>> virt);
};

///////////////////////////////////////////////////////////////////

#endif // VIRTHYP_H
