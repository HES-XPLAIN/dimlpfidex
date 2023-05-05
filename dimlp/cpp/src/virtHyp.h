#ifndef VIRTHYP_H
#define VIRTHYP_H

using namespace std;
///////////////////////////////////////////////////////////////////

class VirtualHyp {
  int NbBins;
  int NbKnots;

  int NbIn;
  int Multiple;
  int NbHyp;

  float *Bias;
  float *Weights;

  float **VecVirtHyp;
  float **VirtGoLeftEps;
  float **VirtGoRightEps;

  //----------------------------------------------------------------

  void CreateVirtualHyp();
  void SetVirtualHyp();
  void SortVirtualHyp();
  void SetEpsVirt();

  //----------------------------------------------------------------

public:
  float *GetVirtHyp(int var) { return VecVirtHyp[var]; }
  float *GetEpsGoLeft(int var) { return VirtGoLeftEps[var]; }
  float *GetEpsGoRight(int var) { return VirtGoRightEps[var]; }

  int KnotInd(int var, float val);
  int GetInd(int var, float val);

  void Del();

  VirtualHyp(int nbBins, int nbIn, int multiple, float *bias, float *weights);

  VirtualHyp(
      int nbBins,
      int nbIn,
      int multiple,
      int nbNets,
      VirtualHyp **virt);
};

///////////////////////////////////////////////////////////////////

#endif
