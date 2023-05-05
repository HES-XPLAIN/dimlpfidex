#ifndef LAYERD3_H
#define LAYERD3_H

using namespace std;
#ifndef LD3
#include "layer.h"
#include "stairObj.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerDimlp3 : public Layer {

  StairObj *Stair;

  //----------------------------------------------------------------

  float Activation1(float x) { return Stair->Funct(x); }
  void ForwLayer() { ForwSpec(); }
  void AdaptWeights() { AdaptWeightsSpec(); }
  void ComputeDeltaDown() { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  void Del() {
    delete Stair;
    Layer::Del();
  }

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
