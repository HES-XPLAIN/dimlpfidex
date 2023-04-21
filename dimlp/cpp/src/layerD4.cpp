using namespace std;
#include "layerD4.h"

///////////////////////////////////////////////////////////////////

LayerDimlp4::LayerDimlp4
(
   float    eta, 
   float    mu, 
   float    flat, 
   int      nbDown, 
   int      nbUp, 
   int      nbWeights,
   int      nbWeightsForInit,
   int      nbKnots)            :

   Layer    (eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit)

{   
    Stair = new StairObj(nbKnots);
}

///////////////////////////////////////////////////////////////////

