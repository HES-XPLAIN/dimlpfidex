#ifndef LAYERD2_H
#define LAYERD2_H

using namespace std;
#ifndef LD2
#include "layer.h"
#include "stairObj.h"
#endif

///////////////////////////////////////////////////////////////////

class LayerDimlp2 : public Layer {

  StairObj *Stair;

  //----------------------------------------------------------------

  float Activation1(float x) { return Stair->Funct(x); }
  void ForwLayer() { ForwSpec(); }
  void AdaptWeights() { ; }
  void AdaptBias() { ; }
  void ComputeDeltaDown() { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  void Del() {
    delete Stair;
    Layer::Del();
  }

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
