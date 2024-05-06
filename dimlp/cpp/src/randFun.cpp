#include <stdlib.h>
#include <sys/types.h>
#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include "randFun.h"

///////////////////////////////////////////////////////////////////

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

IntRandomFunction::IntRandomFunction(int seed)

{
  StartOnlyOnece(seed);
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction(int lowBound, int hiBound) : LowBound(lowBound), HiBound(hiBound), Diff(hiBound - lowBound + 1)

{
}

///////////////////////////////////////////////////////////////////

int IntRandomFunction::RandomInteger() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (random() % Diff));
#elif defined(_WIN32)
  return (LowBound + (rand() % Diff));
#endif
}

///////////////////////////////////////////////////////////////////

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

FloatRandomFunction::FloatRandomFunction(int seed)

{
  StartOnlyOnece(seed);
}

///////////////////////////////////////////////////////////////////

FloatRandomFunction::FloatRandomFunction(
    float lowBound,
    float hiBound) : LowBound(lowBound), HiBound(hiBound), Diff(hiBound - lowBound)

{
}

///////////////////////////////////////////////////////////////////

float FloatRandomFunction::RandomFloat() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (static_cast<float>(drand48()) * Diff));
#elif defined(_WIN32)
  return (LowBound + ((float(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////

double FloatRandomFunction::RandomDouble() const

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (drand48() * Diff));
#elif defined(_WIN32)
  return (LowBound + ((double(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////
