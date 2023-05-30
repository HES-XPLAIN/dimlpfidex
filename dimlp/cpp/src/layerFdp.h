#ifndef LAYERFDP_H
#define LAYERFDP_H

#ifndef LLL
#include "layer.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerFdimlp : public Layer {

  //----------------------------------------------------------------

  float Activation1(float x) override { return Gaussian(x); }
  float Activation2(float x) override { return Gaussian(x); }

  void ForwLayer() override { ForwSpec2(); }
  void AdaptWeights() override { AdaptWeightsSpec2(); }
  void AdaptBias() override { AdaptBiasSpec2(); }

  //----------------------------------------------------------------

public:
  LayerFdimlp(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);
};

///////////////////////////////////////////////////////////////////

#endif
