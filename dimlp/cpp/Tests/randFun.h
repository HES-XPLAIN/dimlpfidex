#ifndef RANDFUN_H
#define RANDFUN_H

using namespace std;
///////////////////////////////////////////////////////////////////

class IntRandomFunction {

  int LowBound;
  int HiBound;
  int Diff;

  void StartOnlyOnece(int seed = 0);

public:
  IntRandomFunction(int seed = 0);
  IntRandomFunction(int lowBound, int hiBound);

  int RandomInteger();
};

///////////////////////////////////////////////////////////////////

class FloatRandomFunction {

  float LowBound;
  float HiBound;
  float Diff;

  void StartOnlyOnece(int seed = 0);

public:
  FloatRandomFunction(int seed = 0);
  FloatRandomFunction(float lowBound, float hiBound);

  float RandomFloat();
  double RandomDouble();
};

///////////////////////////////////////////////////////////////////

#endif
