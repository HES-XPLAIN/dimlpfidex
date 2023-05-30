#ifndef LAYERD4_H
#define LAYERD4_H

#ifndef LD4
#include "layer.h"
#include "stairObj.h"
#endif
#include <memory>

///////////////////////////////////////////////////////////////////

class LayerDimlp4 : public Layer {

  std::unique_ptr<StairObj> Stair;

  //----------------------------------------------------------------

  float Activation1(float x) override { return Stair->Funct(x); }
  void ForwLayer() override { ForwSpec(); }
  void AdaptWeights() override {}
  void AdaptBias() override {}
  void ComputeDeltaDown() override {}

  //----------------------------------------------------------------

public:
  LayerDimlp4(
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
