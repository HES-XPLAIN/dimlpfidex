#ifndef LAYERSP4_H
#define LAYERSP4_H

#ifndef LAYER
#include "layer.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerSP4 : public Layer {

  //----------------------------------------------------------------

  float Activation1(float x) override { return Power4(x); }

  //----------------------------------------------------------------

public:
  LayerSP4(
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
