#ifndef STAIROBJ_H
#define STAIROBJ_H

#include "standAct.h"
#include <vector>

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

  std::vector<float> Knots;
  std::vector<float> EvalKnots;

  //----------------------------------------------------------------

  void InitMemberConstForAnsi();
  void ActivateKnots();
  float Activation(float x) const { return Sigmoid(x); }

  //----------------------------------------------------------------

public:
  float GetHiKnot() const { return HiKnot; }
  std::vector<float> &GetKnots() { return Knots; }
  float Funct(float x) const;

  explicit StairObj(int nbBins);
};

////////////////////////////////////////////////////////////////////////

#endif
