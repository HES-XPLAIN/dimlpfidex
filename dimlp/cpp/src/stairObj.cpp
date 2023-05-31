#include "stairObj.h"

////////////////////////////////////////////////////////////////////////

void StairObj::InitMemberConstForAnsi()

{
  HiKnot = 5.0;
  LowKnot = -HiKnot;
  Dist = HiKnot - LowKnot;
}

////////////////////////////////////////////////////////////////////////

void StairObj::ActivateKnots()

{
  BinWidth = Dist / static_cast<float>(NbBins);

  Knots.resize(NbKnots);
  EvalKnots.resize(NbKnots);

  for (int k = 0; k < NbKnots; k++) {
    Knots[k] = LowKnot + (BinWidth * static_cast<float>(k));
    EvalKnots[k] = Activation(Knots[k]);
  }

  ValLowKnot = Activation(static_cast<float>(-1111111111.0));
  ValHighKnot = Activation(HiKnot);
}

///////////////////////////////////////////////////////////////////

float StairObj::Funct(float x) const

{
  int indBin;

  if (x < LowKnot)
    return ValLowKnot;
  if (x >= HiKnot)
    return ValHighKnot;

  indBin = static_cast<int>((x - LowKnot) / BinWidth);
  return EvalKnots[indBin];
}

///////////////////////////////////////////////////////////////////

StairObj::StairObj(int nbBins) : NbBins(nbBins), NbKnots(nbBins + 1)

{
  InitMemberConstForAnsi();
  ActivateKnots();
}

///////////////////////////////////////////////////////////////////
