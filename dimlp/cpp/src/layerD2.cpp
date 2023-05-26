#include "layerD2.h"
using namespace std;

///////////////////////////////////////////////////////////////////

LayerDimlp2::LayerDimlp2(
    float eta,
    float mu,
    float flat,
    int nbDown,
    int nbUp,
    int nbWeights,
    int nbWeightsForInit,
    int nbKnots) :

                   Layer(eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit)

{
  std::unique_ptr<StairObj> stair(new StairObj(nbKnots));
}

///////////////////////////////////////////////////////////////////
