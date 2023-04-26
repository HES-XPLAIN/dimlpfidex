#ifndef LAYERDMP
#define LAYERDMP

using namespace std;
#include "layer.h"
#include "stairObj.h"

///////////////////////////////////////////////////////////////////

class LayerDimlp : public Layer {

  StairObj *Stair;

  //----------------------------------------------------------------

  float Activation1(float x) { return Stair->Funct(x); }
  void ForwLayer() { ForwSpec(); }
  void AdaptWeights() { AdaptWeightsSpec(); }
  void ComputeDeltaDown() { ; }

  //----------------------------------------------------------------

public:
  void Del() {
    delete Stair;
    Layer::Del();
  }

  LayerDimlp(float eta, float mu, float flat, int nbDown, int nbUp,
             int nbWeights, int nbWeightsForInit, int nbKnots);
};

///////////////////////////////////////////////////////////////////

#endif
