#ifndef LAYERD3_H
#define LAYERD3_H

#ifndef LD3
#include "layer.h"
#include "stairObj.h"
#endif
#include <memory>

///////////////////////////////////////////////////////////////////

class LayerDimlp3 : public Layer {

  std::unique_ptr<StairObj> Stair;

  //----------------------------------------------------------------

  float Activation1(float x) override { return Stair->Funct(x); }
  void ForwLayer() override { ForwSpec(); }
  void AdaptWeights() override { AdaptWeightsSpec(); }
  void ComputeDeltaDown() override { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  LayerDimlp3(
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
