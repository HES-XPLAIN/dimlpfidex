#ifndef STAIROBJ_H
#define STAIROBJ_H

#include "standAct.h"
#include <vector>

////////////////////////////////////////////////////////////////////////

/**
 * @brief The StairObj class is used to implement a staircase function as an activation function.
 *
 * This class precomputes activation values at discrete points (knots) and uses these values to
 * approximate the activation function for any input within a specified range.
 */
class StairObj {
  int NbBins; ///< Number of bins for the staircase function.
  // Stair is <= activation
  int NbKnots; ///< Number of knots (NbBins + 1) for the staircase function.

  float HiKnot;      ///< Upper bound of the knot range.
  float LowKnot;     ///< Lower bound of the knot range.
  float ValLowKnot;  ///< Activation value at the lower bound.
  float ValHighKnot; ///< Activation value at the upper bound.
  float Dist;        ///< Distance between HiKnot and LowKnot.

  float BinWidth; ///< Width of each bin.

  std::vector<float> Knots;     ///< Vector storing the positions of the knots.
  std::vector<float> EvalKnots; ///< Vector storing the activation values at the knots.

  //----------------------------------------------------------------

  /**
   * @brief Initializes the constants for the member variables.
   */
  void InitMemberConstForAnsi();

  /**
   * @brief Activates the knots by computing their activation values.
   */
  void ActivateKnots();

  /**
   * @brief Sigmoid activation function.
   * @param x Input value.
   * @return Activated value.
   */
  float Activation(float x) const { return Sigmoid(x); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Gets the upper bound of the knot range.
   * @return The upper bound of the knot range.
   */
  float GetHiKnot() const { return HiKnot; }

  /**
   * @brief Gets the vector of knot positions.
   * @return Reference to the vector of knot positions.
   */
  std::vector<float> &GetKnots() { return Knots; }

  /**
   * @brief Computes the staircase function value for a given input.
   */
  float Funct(float x) const;

  /**
   * @brief Constructs a StairObj with a specified number of bins.
   */
  explicit StairObj(int nbBins);
};

////////////////////////////////////////////////////////////////////////

#endif // STAIROBJ_H
