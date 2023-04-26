using namespace std;
#include <stdlib.h>
#include "virtHyp.h"
#include "stairObj.h"
#include "misc.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////

void VirtualHyp::CreateVirtualHyp()

{  int n;

   VecVirtHyp     = new float*[NbIn];
   VirtGoLeftEps  = new float*[NbIn];
   VirtGoRightEps = new float*[NbIn];

   for (n=0; n<NbIn; n++)
   {
       VecVirtHyp[n]     = new float[NbHyp];
       VirtGoLeftEps[n]  = new float[NbHyp];
       VirtGoRightEps[n] = new float[NbHyp];
   }
}

////////////////////////////////////////////////////////////////////////

void VirtualHyp::SetVirtualHyp()

{  StairObj stairObj(NbBins);
   int v, m, k;

   for (v=0; v<NbIn; v++)
   {
       float* ptrVar     = VecVirtHyp[v];
       float* ptrBias    = Bias + v;
       float* ptrWeights = Weights + v;

       for (m=0; m<Multiple; m++, ptrBias+=NbIn, ptrWeights+=NbIn)
       {
           float* ptrKnots = stairObj.GetKnots();

           for (k=0; k<NbKnots; k++, ptrKnots++, ptrVar++)
           {
                if (*ptrWeights != 0.0)
                   *ptrVar = (*ptrKnots - *ptrBias) / *ptrWeights;
                else
                   *ptrVar = (*ptrKnots - *ptrBias) * 1111111111.0 *
                                                      1111111111.0;
           }
       }
   }
}

////////////////////////////////////////////////////////////////////////

void VirtualHyp::SortVirtualHyp()

{  int v;

   for (v=0; v<NbIn; v++)
       qsort(VecVirtHyp[v], NbHyp, sizeof(float), Compare);
}

////////////////////////////////////////////////////////////////////////

void VirtualHyp::SetEpsVirt()

{  int k, v;
   float halfDiff;


   for (v=0; v<NbIn; v++)
   {
      float first = *(VecVirtHyp[v]+0);

      for (k=1; k<NbHyp-1; k++)
          if (*(VecVirtHyp[v]+k) != first) break;

      float second = *(VecVirtHyp[v]+k);

      *(VirtGoRightEps[v] + 0) = *(VecVirtHyp[v] + 0) + (second-first)*0.5;

      for (k=1; k<NbHyp-1; k++)
      {
          halfDiff = (*(VecVirtHyp[v]+k+1) - *(VecVirtHyp[v]+k)) * 0.5;

          if (halfDiff != 0.0)
             *(VirtGoRightEps[v] + k) = *(VecVirtHyp[v] + k) + halfDiff;
          else
             *(VirtGoRightEps[v] + k) = *(VirtGoRightEps[v] + k-1);
      }

      *(VirtGoRightEps[v] + NbHyp-1) = *(VecVirtHyp[v] + NbHyp-1) + 1.0;
   }

   for (v=0; v<NbIn; v++)
   {
      *(VirtGoLeftEps[v] + 0) = *(VecVirtHyp[v] + 0) - 1.0;  //not used

      for (k=1; k<NbHyp; k++)
      {
          halfDiff = (*(VecVirtHyp[v]+k) - *(VecVirtHyp[v]+k-1)) * 0.5;

          if (halfDiff != 0.0)
             *(VirtGoLeftEps[v] + k) = *(VecVirtHyp[v] + k) - halfDiff;
          else
             *(VirtGoLeftEps[v] + k) = *(VirtGoLeftEps[v] + k-1);
      }
   }
}

////////////////////////////////////////////////////////////////////////

int VirtualHyp::KnotInd(int var, float val) // var = index hid

{  int k;
   float* ptrKnots = VecVirtHyp[var];

   for (k=0; k<NbHyp; k++, ptrKnots++)
   {
       if (*ptrKnots > val) return k-1;
   }

   return NbHyp-1;
}

////////////////////////////////////////////////////////////////////////

int VirtualHyp::GetInd(int var, float val)

{  int k;
   float* ptrKnots = VecVirtHyp[var];

   for (k=0; k<NbHyp; k++, ptrKnots++)
       if (*ptrKnots == val) return k;

   return -1;
}

////////////////////////////////////////////////////////////////////////

void VirtualHyp::Del()

{  int i;

   for (i=0; i<NbIn; i++)
   {
       delete VecVirtHyp[i];
       delete VirtGoLeftEps[i];
       delete VirtGoRightEps[i];
   }

   delete VecVirtHyp;
   delete VirtGoLeftEps;
   delete VirtGoRightEps;
}

////////////////////////////////////////////////////////////////////////

VirtualHyp::VirtualHyp
(
   int    nbBins,
   int    nbIn,
   int    multiple,
   float* bias,
   float* weights)

{
   NbBins   = nbBins;
   NbIn     = nbIn;
   Multiple = multiple;
   Bias     = bias;
   Weights  = weights;

   NbKnots = NbBins + 1;
   NbHyp   = NbKnots*Multiple;

   CreateVirtualHyp();
   SetVirtualHyp();
   SortVirtualHyp();
   SetEpsVirt();
}

////////////////////////////////////////////////////////////////////////

VirtualHyp::VirtualHyp
(
   int          nbBins,
   int          nbIn,
   int          multiple,
   int          nbNets,
   VirtualHyp** virt)

{  int nbHypOld;
   int v, h, n;

   VirtualHyp* ptrOldVirt;
   float*      vectOldVirt;


   NbBins   = nbBins;
   NbIn     = nbIn;
   Multiple = multiple;

   NbKnots  = NbBins + 1;
   NbHyp    = NbKnots * Multiple * nbNets;
   nbHypOld = NbKnots * Multiple;

   CreateVirtualHyp();

   for (v=0; v<NbIn; v++)
   {
       for (n=0; n<nbNets; n++)
       {
           ptrOldVirt  = virt[n];
           vectOldVirt = ptrOldVirt->GetVirtHyp(v);

           for (h=0; h<nbHypOld; h++)
           {
               *(VecVirtHyp[v] + h + (n*nbHypOld)) = vectOldVirt[h];
           }
       }
   }

   SortVirtualHyp();
   SetEpsVirt();
}

////////////////////////////////////////////////////////////////////////
