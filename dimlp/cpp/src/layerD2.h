#ifndef LAYERD2_H
#define LAYERD2_H

#ifndef LD2
#include "layer.h"
#include "stairObj.h"
#endif
#include <memory>

///////////////////////////////////////////////////////////////////

class LayerDimlp2 : public Layer {

  std::unique_ptr<StairObj> Stair;

  //----------------------------------------------------------------

  float Activation1(float x) override { return Stair->Funct(x); }
  void ForwLayer() override { ForwSpec(); }
  void AdaptWeights() override {}
  void AdaptBias() override {}
  void ComputeDeltaDown() override { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  LayerDimlp2(
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
