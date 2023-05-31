#ifndef RANDFUN_H
#define RANDFUN_H

///////////////////////////////////////////////////////////////////

class IntRandomFunction {

  int LowBound;
  int HiBound;
  int Diff;

  void StartOnlyOnece(int seed = 0) const;

public:
  explicit IntRandomFunction(int seed = 0);
  explicit IntRandomFunction(int lowBound, int hiBound);

  int RandomInteger() const;
};

///////////////////////////////////////////////////////////////////

class FloatRandomFunction {

  float LowBound;
  float HiBound;
  float Diff;

  void StartOnlyOnece(int seed = 0) const;

public:
  explicit FloatRandomFunction(int seed = 0);
  explicit FloatRandomFunction(float lowBound, float hiBound);

  float RandomFloat() const;
  double RandomDouble() const;
};

///////////////////////////////////////////////////////////////////

#endif
