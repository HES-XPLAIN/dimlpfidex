#ifndef RANDFUN_H
#define RANDFUN_H

using namespace std;
///////////////////////////////////////////////////////////////////

class IntRandomFunction {

      int LowBound;
      int HiBound;
      int Diff;

      void StartOnlyOnece();

public:
      
      IntRandomFunction();
      IntRandomFunction(int lowBound, int hiBound);

      int RandomInteger();
};

///////////////////////////////////////////////////////////////////

class FloatRandomFunction {

      float LowBound;
      float HiBound;
      float Diff;

      void  StartOnlyOnece();

public:

      FloatRandomFunction();
      FloatRandomFunction(float lowBound, float hiBound);

      float  RandomFloat();
      double RandomDouble();
};

///////////////////////////////////////////////////////////////////

#endif