#include <stdlib.h>
#include <sys/types.h>
#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include "randFun.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the random seed only once.
 *
 * @param seed The seed value for the random number generator. If 0, the seed is based on the current time.
 */
void IntRandomFunction::StartOnlyOnece(int seed) const

{

  if (seed == 0) {
    seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
  }

#if defined(__unix__) || defined(__APPLE__)
  srandom(seed);
#elif defined(_WIN32)
  srand(seed);
#endif
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor that initializes the random seed.
 *
 * @param seed The seed value for the random number generator.
 */
IntRandomFunction::IntRandomFunction(int seed)

{
  StartOnlyOnece(seed);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor that sets the range for random integers.
 *
 * @param lowBound The lower bound of the range.
 * @param hiBound The upper bound of the range.
 */
IntRandomFunction::IntRandomFunction(int lowBound, int hiBound) : LowBound(lowBound), HiBound(hiBound), Diff(hiBound - lowBound + 1)

{
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Generates a random integer within the specified range.
 *
 * @return A random integer within the range [lowBound, hiBound].
 */
int IntRandomFunction::RandomInteger() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (random() % Diff));
#elif defined(_WIN32)
  return (LowBound + (rand() % Diff));
#endif
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the random seed only once.
 *
 * @param seed The seed value for the random number generator. If 0, the seed is based on the current time.
 */
void FloatRandomFunction::StartOnlyOnece(int seed) const

{
  if (seed == 0) {
    seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
  }
#if defined(__unix__) || defined(__APPLE__)
  srand48(seed);
#elif defined(_WIN32)
  srand(seed);
#endif
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor that initializes the random seed.
 *
 * @param seed The seed value for the random number generator.
 */
FloatRandomFunction::FloatRandomFunction(int seed)

{
  StartOnlyOnece(seed);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor that sets the range for random floating-point numbers.
 *
 * @param lowBound The lower bound of the range.
 * @param hiBound The upper bound of the range.
 */
FloatRandomFunction::FloatRandomFunction(
    float lowBound,
    float hiBound) : LowBound(lowBound), HiBound(hiBound), Diff(hiBound - lowBound)

{
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Generates a random float within the specified range.
 *
 * @return A random float within the range [lowBound, hiBound].
 */
float FloatRandomFunction::RandomFloat() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (static_cast<float>(drand48()) * Diff));
#elif defined(_WIN32)
  return (LowBound + ((float(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Generates a random double within the specified range.
 *
 * @return A random double within the range [lowBound, hiBound].
 */
double FloatRandomFunction::RandomDouble() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (drand48() * Diff));
#elif defined(_WIN32)
  return (LowBound + ((double(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////
