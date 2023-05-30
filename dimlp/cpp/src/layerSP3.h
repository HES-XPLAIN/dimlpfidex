#ifndef LAYERSP3_H
#define LAYERSP3_H

#ifndef LAYER
#include "layer.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerSP3 : public Layer {

  //----------------------------------------------------------------

  float Activation1(float x) override { return Power3(x); }

  //----------------------------------------------------------------

public:
  LayerSP3(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif
