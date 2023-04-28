using namespace std;
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "randFun.h"

///////////////////////////////////////////////////////////////////

void IntRandomFunction::StartOnlyOnece(int seed)

{
   #ifdef __unix__
   if (seed == 0){
      srandom(getpid());
   }
   else{
      srandom(seed);
   }
   #elif defined(_WIN32)
   if (seed == 0){
      srand(getpid());
   }
   else{
      srand(seed);
   }
   #endif
}

///////////////////////////////////////////////////////////////////

IntRandomFunction::IntRandomFunction(int seed)

{
   cout << "SEEEEEEED" << seed << endl;
   StartOnlyOnece(seed);
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

void FloatRandomFunction::StartOnlyOnece(int seed)

{
   #ifdef __unix__
   if (seed == 0){
      srand48(getpid());
   }
   else{
      srand48(seed);
   }
   #elif defined(_WIN32)
   if (seed == 0){
      srand(getpid());
   }
   else{
      srand(seed);
   }
   #endif
}

///////////////////////////////////////////////////////////////////

FloatRandomFunction::FloatRandomFunction(int seed)

{
   cout << "SEEEEEEED" << seed << endl;
   StartOnlyOnece(seed);
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
