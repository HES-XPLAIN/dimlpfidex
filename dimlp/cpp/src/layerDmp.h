#ifndef LAYERDMP
#define LAYERDMP

#include "layer.h"
#include "stairObj.h"
#include <memory>

///////////////////////////////////////////////////////////////////

class LayerDimlp : public Layer {

  std::unique_ptr<StairObj> Stair;

  //----------------------------------------------------------------

  float Activation1(float x) override { return Stair->Funct(x); }
  void ForwLayer() override { ForwSpec(); }
  void AdaptWeights() override { AdaptWeightsSpec(); }
  void ComputeDeltaDown() override {}

  //----------------------------------------------------------------

public:
  LayerDimlp(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit,
      int nbKnots);
};

///////////////////////////////////////////////////////////////////

#endif
