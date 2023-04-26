#ifndef LAYERSD_H
#define LAYERSD_H

using namespace std;
#ifndef LAYER
#include "layer.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerSD : public Layer
{

//----------------------------------------------------------------   

   float Activation1(float x) { return Identity(x); }

//----------------------------------------------------------------

public:

   LayerSD
   (
      int      nbDown, 
      int      nbUp, 
      int      nbWeights,
      int      nbWeightsForInit)     :

      Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

   {   
      ;
   }
};

///////////////////////////////////////////////////////////////////

#endif