using namespace std;
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "randFun.h"

///////////////////////////////////////////////////////////////////

void IntRandomFunction::StartOnlyOnece(int seed)

{
#if defined(__unix__) || defined(__APPLE__)
  if (seed == 0) {
    srandom(getpid());
  } else {
    srandom(seed);
  }
#elif defined(_WIN32)
  if (seed == 0) {
    srand(getpid());
  } else {
    srand(seed);
  }
#endif
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction(int seed)

{
  StartOnlyOnece(seed);
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction(int lowBound, int hiBound)

{
  LowBound = lowBound;
  HiBound = hiBound;
  Diff = hiBound - lowBound + 1;
}

///////////////////////////////////////////////////////////////////

int IntRandomFunction::RandomInteger()

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (random() % Diff));
#elif defined(_WIN32)
  return (LowBound + (rand() % Diff));
#endif
}

///////////////////////////////////////////////////////////////////

void FloatRandomFunction::StartOnlyOnece(int seed)

{
#if defined(__unix__) || defined(__APPLE__)
  if (seed == 0) {
    srand48(getpid());
  } else {
    srand48(seed);
  }
#elif defined(_WIN32)
  if (seed == 0) {
    srand(getpid());
  } else {
    srand(seed);
  }
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
    float hiBound)

{
  LowBound = lowBound;
  HiBound = hiBound;

  Diff = HiBound - LowBound;
}

///////////////////////////////////////////////////////////////////

float FloatRandomFunction::RandomFloat()

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (drand48() * Diff));
#elif defined(_WIN32)
  return (LowBound + ((float(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////

double FloatRandomFunction::RandomDouble()

{
#if defined(__unix__) || defined(__APPLE__)
  return (LowBound + (drand48() * Diff));
#elif defined(_WIN32)
  return (LowBound + ((double(rand()) / RAND_MAX) * Diff));
#endif
}

///////////////////////////////////////////////////////////////////
