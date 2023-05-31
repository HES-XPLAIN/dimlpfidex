#ifndef REALHYP2_H
#define REALHYP2_H

#include "realHyp.h"

///////////////////////////////////////////////////////////////////

class RealHyp2 : public RealHyp {

  //----------------------------------------------------------------

  void GoLeft(int var, int indPat, int startVirt, int netAns) override { Gl2(var, indPat, startVirt, netAns); }

  void GoRight(int var, int indPat, int startVirt, int netAns) override { Gr2(var, indPat, startVirt, netAns); }

  //----------------------------------------------------------------

public:
  virtual ~RealHyp2() = default;

  RealHyp2(
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  RealHyp2(
      VirtualHyp *globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);
};

///////////////////////////////////////////////////////////////////

#endif
