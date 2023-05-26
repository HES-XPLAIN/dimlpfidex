#ifndef LAYERFDP_H
#define LAYERFDP_H

using namespace std;
#ifndef LLL
#include "layer.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerFdimlp : public Layer {

  //----------------------------------------------------------------

  float Activation1(float x) { return Gaussian(x); }
  float Activation2(float x) { return Gaussian(x); }

  void ForwLayer() { ForwSpec2(); }
  void AdaptWeights() { AdaptWeightsSpec2(); }
  void AdaptBias() { AdaptBiasSpec2(); }

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
