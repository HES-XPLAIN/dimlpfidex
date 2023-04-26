using namespace std;
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "randFun.h"

///////////////////////////////////////////////////////////////////

void IntRandomFunction::StartOnlyOnece()

{
   #ifdef __unix__
   srandom(getpid());
   #elif defined(_WIN32)
   srand(getpid());
   #endif
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction()

{
   StartOnlyOnece();
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction(int lowBound, int hiBound)

{
   LowBound = lowBound;
   HiBound  = hiBound;
   Diff     = hiBound - lowBound + 1;
}

///////////////////////////////////////////////////////////////////

int IntRandomFunction::RandomInteger()

{
   #ifdef __unix__
   return (LowBound + (random() % Diff));
   #elif defined(_WIN32)
   return (LowBound + (rand() % Diff));
   #endif
}

///////////////////////////////////////////////////////////////////

void FloatRandomFunction::StartOnlyOnece()

{
   #ifdef __unix__
   srand48(getpid());
   #elif defined(_WIN32)
   srand(getpid());
   #endif
}

///////////////////////////////////////////////////////////////////

FloatRandomFunction::FloatRandomFunction()

{
   StartOnlyOnece();
}

///////////////////////////////////////////////////////////////////

FloatRandomFunction::FloatRandomFunction
(
   float lowBound,
   float hiBound)

{
   LowBound = lowBound;
   HiBound  = hiBound;

   Diff = HiBound - LowBound;
}

///////////////////////////////////////////////////////////////////

float FloatRandomFunction::RandomFloat()

{
   #ifdef __unix__
   return (LowBound + (drand48() * Diff));
   #elif defined(_WIN32)
   return (LowBound + ((float(rand()) / RAND_MAX) * Diff));
   #endif
}

///////////////////////////////////////////////////////////////////

double FloatRandomFunction::RandomDouble()

{
   #ifdef __unix__
   return (LowBound + (drand48() * Diff));
   #elif defined(_WIN32)
   return (LowBound + ((double(rand()) / RAND_MAX) * Diff));
   #endif

}

///////////////////////////////////////////////////////////////////
