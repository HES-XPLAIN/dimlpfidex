#include "stairObj.h"

////////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the constants for the member variables.
 */
void StairObj::InitMemberConstForAnsi()

{
  HiKnot = 5.0;
  LowKnot = -HiKnot;
  Dist = HiKnot - LowKnot;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Activates the knots by computing their activation values.
 */
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

/**
 * @brief Computes the staircase function value for a given input.
 * @param x Input value.
 * @return Staircase function value.
 */
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

/**
 * @brief Constructs a StairObj with a specified number of bins.
 * @param nbBins Number of bins for the staircase function.
 */
StairObj::StairObj(int nbBins) : NbBins(nbBins), NbKnots(nbBins + 1)

{
  InitMemberConstForAnsi();
  ActivateKnots();
}

///////////////////////////////////////////////////////////////////
