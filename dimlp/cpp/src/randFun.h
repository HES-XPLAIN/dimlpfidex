#ifndef RANDFUN_H
#define RANDFUN_H
#include <chrono>

///////////////////////////////////////////////////////////////////

/**
 * @brief Class for generating random integers within a specified range.
 */
class IntRandomFunction {
  int LowBound; ///< Lower bound of the range.
  int HiBound;  ///< Upper bound of the range.
  int Diff;     ///< Difference between upper and lower bound.

  /**
   * @brief Initializes the random seed only once.
   */
  void StartOnlyOnece(int seed = 0) const;

public:
  /**
   * @brief Constructor that initializes the random seed.
   */
  explicit IntRandomFunction(int seed = 0);

  /**
   * @brief Constructor that sets the range for random integers.
   */
  explicit IntRandomFunction(int lowBound, int hiBound);

  /**
   * @brief Generates a random integer within the specified range.
   */
  int RandomInteger() const;
};

///////////////////////////////////////////////////////////////////

/**
 * @brief Class for generating random floating-point numbers within a specified range.
 */
class FloatRandomFunction {
  float LowBound; ///< Lower bound of the range.
  float HiBound;  ///< Upper bound of the range.
  float Diff;     ///< Difference between upper and lower bound.

  /**
   * @brief Initializes the random seed only once.
   */
  void StartOnlyOnece(int seed = 0) const;

public:
  /**
   * @brief Constructor that initializes the random seed.
   */
  explicit FloatRandomFunction(int seed = 0);

  /**
   * @brief Constructor that sets the range for random floating-point numbers.
   */
  explicit FloatRandomFunction(float lowBound, float hiBound);

  /**
   * @brief Generates a random float within the specified range.
   */
  float RandomFloat() const;

  /**
   * @brief Generates a random double within the specified range.
   */
  double RandomDouble() const;
};

///////////////////////////////////////////////////////////////////

#endif // RANDFUN_H
