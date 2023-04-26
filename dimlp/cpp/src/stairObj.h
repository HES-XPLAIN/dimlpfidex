#ifndef STAIROBJ_H
#define STAIROBJ_H

using namespace std;
#include "standAct.h"

////////////////////////////////////////////////////////////////////////

class StairObj {
  int NbBins;
  int NbKnots; // Stair is <= activation

  float HiKnot;
  float LowKnot;
  float ValLowKnot;
  float ValHighKnot;
  float Dist;

  float BinWidth;

  float *Knots;
  float *EvalKnots;

  //----------------------------------------------------------------

  void InitMemberConstForAnsi();
  void ActivateKnots();
  float Activation(float x) { return Sigmoid(x); }

  //----------------------------------------------------------------

public:
  float GetHiKnot() { return HiKnot; }
  float *GetKnots() { return Knots; }
  float Funct(float x);

  StairObj(int nbBins);
  ~StairObj();
};

////////////////////////////////////////////////////////////////////////

#endif
