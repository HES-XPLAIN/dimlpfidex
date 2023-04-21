using namespace std;
#include "stairObj.h"

////////////////////////////////////////////////////////////////////////

void StairObj::InitMemberConstForAnsi()

{
     HiKnot  = 5.0;
     LowKnot = -HiKnot;
     Dist    = HiKnot - LowKnot;
}

////////////////////////////////////////////////////////////////////////

void StairObj::ActivateKnots()

{  int k;

   BinWidth = (float) (Dist) / (float) (NbBins);

   Knots          = new float[NbKnots];
   EvalKnots      = new float[NbKnots];


   for (k=0; k<NbKnots; k++)
   {
       Knots[k]     = LowKnot + (BinWidth*k);
       EvalKnots[k] = Activation(Knots[k]);
   }

   ValLowKnot   = Activation(-1111111111.0);
   ValHighKnot  = Activation(HiKnot);
}

///////////////////////////////////////////////////////////////////

float StairObj::Funct(float x)

{  int indBin;

   if (x < LowKnot) return ValLowKnot;
   if (x >= HiKnot)  return ValHighKnot;

   indBin = (int) ((x - LowKnot) / BinWidth);
   return *(EvalKnots + indBin);
}

///////////////////////////////////////////////////////////////////

StairObj::StairObj(int nbBins)

{
   NbBins  = nbBins;
   NbKnots = nbBins + 1;

   InitMemberConstForAnsi();
   ActivateKnots();
}

///////////////////////////////////////////////////////////////////

StairObj::~StairObj()

{
   delete Knots;
   delete EvalKnots;
}

///////////////////////////////////////////////////////////////////
