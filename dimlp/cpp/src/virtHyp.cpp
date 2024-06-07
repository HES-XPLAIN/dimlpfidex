#include "virtHyp.h"
#include "misc.h"
#include "stairObj.h"
#include <stdlib.h>

#include <iostream>

////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates the structure for storing virtual hyperplanes.
 */
void VirtualHyp::CreateVirtualHyp()

{
  VecVirtHyp.resize(NbIn);
  VirtGoLeftEps.resize(NbIn);
  VirtGoRightEps.resize(NbIn);

  for (int n = 0; n < NbIn; n++) {
    VecVirtHyp[n].resize(NbHyp);
    VirtGoLeftEps[n].resize(NbHyp);
    VirtGoRightEps[n].resize(NbHyp);
  }
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Sets the virtual hyperplanes based on biases and weights.
 */
void VirtualHyp::SetVirtualHyp()

{
  StairObj stairObj(NbBins);
  for (int v = 0; v < NbIn; v++) {
    float *ptrVar = VecVirtHyp[v].data();
    const float *ptrBias = Bias + v;
    const float *ptrWeights = Weights + v;

    for (int m = 0; m < Multiple; m++, ptrBias += NbIn, ptrWeights += NbIn) {
      const float *ptrKnots = stairObj.GetKnots().data();

      for (int k = 0; k < NbKnots; k++, ptrKnots++, ptrVar++) {
        if (*ptrWeights != 0.0)
          *ptrVar = (*ptrKnots - *ptrBias) / *ptrWeights;
        else
          *ptrVar = static_cast<float>((*ptrKnots - *ptrBias) * 1111111111.0 *
                                       1111111111.0);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Sorts the virtual hyperplanes for each input variable.
 */
void VirtualHyp::SortVirtualHyp()

{
  for (int v = 0; v < NbIn; v++)
    qsort(VecVirtHyp[v].data(), NbHyp, sizeof(float), Compare);
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Sets the epsilon values for slightly shifted virtual hyperplanes.
 */
void VirtualHyp::SetEpsVirt()

{
  int k;
  int v;
  float halfDiff;

  for (v = 0; v < NbIn; v++) {
    float first = *(VecVirtHyp[v].data() + 0);

    for (k = 1; k < NbHyp - 1; k++)
      if (*(VecVirtHyp[v].data() + k) != first)
        break;

    float second = *(VecVirtHyp[v].data() + k);

    *(VirtGoRightEps[v].data() + 0) = *(VecVirtHyp[v].data() + 0) + static_cast<float>((second - first) * 0.5);

    for (k = 1; k < NbHyp - 1; k++) {
      halfDiff = (*(VecVirtHyp[v].data() + k + 1) - *(VecVirtHyp[v].data() + k)) * static_cast<float>(0.5);

      if (halfDiff != 0.0)
        *(VirtGoRightEps[v].data() + k) = *(VecVirtHyp[v].data() + k) + halfDiff;
      else
        *(VirtGoRightEps[v].data() + k) = *(VirtGoRightEps[v].data() + k - 1);
    }

    *(VirtGoRightEps[v].data() + NbHyp - 1) = *(VecVirtHyp[v].data() + NbHyp - 1) + static_cast<float>(1.0);
  }

  for (v = 0; v < NbIn; v++) {
    *(VirtGoLeftEps[v].data() + 0) = *(VecVirtHyp[v].data() + 0) - static_cast<float>(1.0); // not used

    for (k = 1; k < NbHyp; k++) {
      halfDiff = (*(VecVirtHyp[v].data() + k) - *(VecVirtHyp[v].data() + k - 1)) * static_cast<float>(0.5);

      if (halfDiff != 0.0)
        *(VirtGoLeftEps[v].data() + k) = *(VecVirtHyp[v].data() + k) - halfDiff;
      else
        *(VirtGoLeftEps[v].data() + k) = *(VirtGoLeftEps[v].data() + k - 1);
    }
  }
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Finds the index of the lower nearest knot (threshold) in the virtual hyperplane for a given value.
 *
 * @param var The index of the variable.
 * @param val The value to find the knot index for.
 * @return The knot index, which represents the position of the nearest knot (threshold) that is less than or equal to the given value.
 */
int VirtualHyp::KnotInd(int var, float val) {
  const std::vector<float> &knots = VecVirtHyp[var];

  for (int k = 0; k < NbHyp; k++) {
    if (knots[k] > val)
      return k - 1;
  }

  return NbHyp - 1;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Gets the index of the knot corresponding to a given value in the virtual hyperplanes.
 *
 * @param var The index of the variable.
 * @param val The value to find the index for.
 * @return The index of the value, or -1 if not found.
 */
int VirtualHyp::GetInd(int var, float val)

{
  const std::vector<float> &knots = VecVirtHyp[var];

  for (int k = 0; k < NbHyp; k++) {
    if (knots[k] == val)
      return k;
  }

  return -1;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a VirtualHyp object with the given parameters.
 *
 * @param nbBins Number of bins for discretization.
 * @param nbIn Number of input variables.
 * @param multiple Multiple of the input variables.
 * @param bias Pointer to the biases.
 * @param weights Pointer to the weights.
 */
VirtualHyp::VirtualHyp(
    int nbBins,
    int nbIn,
    int multiple,
    float *bias,
    float *weights) : NbBins(nbBins), NbIn(nbIn), Multiple(multiple), Bias(bias), Weights(weights)

{

  NbKnots = NbBins + 1;
  NbHyp = NbKnots * Multiple;

  CreateVirtualHyp();
  SetVirtualHyp();
  SortVirtualHyp();
  SetEpsVirt();
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a VirtualHyp object by merging multiple VirtualHyp objects.
 *
 * @param nbBins Number of bins for discretization.
 * @param nbIn Number of input variables.
 * @param multiple Multiple of the input variables.
 * @param nbNets Number of networks.
 * @param virt Vector of pointers to the VirtualHyp objects to merge.
 */
VirtualHyp::VirtualHyp(
    int nbBins,
    int nbIn,
    int multiple,
    int nbNets,
    std::vector<std::shared_ptr<VirtualHyp>> virt) : NbBins(nbBins), NbIn(nbIn), Multiple(multiple)

{
  int nbHypOld;

  std::shared_ptr<VirtualHyp> ptrOldVirt;
  const float *vectOldVirt;

  NbKnots = NbBins + 1;
  NbHyp = NbKnots * Multiple * nbNets;
  nbHypOld = NbKnots * Multiple;

  CreateVirtualHyp();

  for (int v = 0; v < NbIn; v++) {
    for (int n = 0; n < nbNets; n++) {
      ptrOldVirt = virt[n];
      vectOldVirt = ptrOldVirt->GetVirtHyp(v).data();

      for (int h = 0; h < nbHypOld; h++) {
        *(VecVirtHyp[v].data() + h + (n * nbHypOld)) = vectOldVirt[h];
      }
    }
  }

  SortVirtualHyp();
  SetEpsVirt();
}

////////////////////////////////////////////////////////////////////////
