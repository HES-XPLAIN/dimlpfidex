#ifndef LAYERD4_H
#define LAYERD4_H

using namespace std;
#ifndef  LD4
#include "layer.h"
#include "stairObj.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerDimlp4 : public Layer
{

   StairObj* Stair;

//----------------------------------------------------------------

   float Activation1(float x) { return Stair->Funct(x); }
   void  ForwLayer()          { ForwSpec(); }
   void  AdaptWeights()       { ; }
   void  AdaptBias()          { ; }
   void  ComputeDeltaDown()   { ; }

//----------------------------------------------------------------

public:

   void Del() { delete Stair; Layer::Del(); }

   LayerDimlp4
   (
      float    eta,
      float    mu,
      float    flat,
      int      nbDown,
      int      nbUp,
      int      nbWeights,
      int      nbWeightsForInit,
      int      nbKnots);

};

///////////////////////////////////////////////////////////////////

#endif
