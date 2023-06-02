#ifndef VIRTHYP_H
#define VIRTHYP_H
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////

class VirtualHyp {
  int NbBins;
  int NbKnots;

  int NbIn;
  int Multiple;
  int NbHyp;

  float *Bias;
  float *Weights;

  std::vector<std::vector<float>> VecVirtHyp;
  std::vector<std::vector<float>> VirtGoLeftEps;
  std::vector<std::vector<float>> VirtGoRightEps;

  //----------------------------------------------------------------

  void CreateVirtualHyp();
  void SetVirtualHyp();
  void SortVirtualHyp();
  void SetEpsVirt();

  //----------------------------------------------------------------

public:
  std::vector<float> &GetVirtHyp(int var) { return VecVirtHyp[var]; }
  std::vector<float> &GetEpsGoLeft(int var) { return VirtGoLeftEps[var]; }
  std::vector<float> &GetEpsGoRight(int var) { return VirtGoRightEps[var]; }

  int KnotInd(int var, float val);
  int GetInd(int var, float val);

  VirtualHyp(int nbBins, int nbIn, int multiple, float *bias, float *weights);

  VirtualHyp(
      int nbBins,
      int nbIn,
      int multiple,
      int nbNets,
      std::vector<std::shared_ptr<VirtualHyp>> virt);
};

///////////////////////////////////////////////////////////////////

#endif
