using namespace std;
#include "realHyp2.h"

///////////////////////////////////////////////////////////////////

RealHyp2::RealHyp2
(
   DataSet& data,
   BpNN*    nn,
   int      nbBins,
   int      nbIn,
   int      multiple,
   int      nbWeightLayers
 )                           :

RealHyp(data, nn, nbBins, nbIn, multiple, nbWeightLayers, 333)
{
   SetConfirmedVirt(data);
   SetRealHyp(data);
}

///////////////////////////////////////////////////////////////////

RealHyp2::RealHyp2
(
   VirtualHyp* globalVirt,
   int         nbNets,
   float*      out,
   int         nbOut,
   DataSet&    data,
   BpNN*       nn,
   int         nbBins,
   int         nbIn,
   int         multiple,
   int         nbWeightLayers
)                             :

RealHyp(globalVirt, nbNets, out, nbOut,
        data, nn, nbBins, nbIn, multiple, nbWeightLayers, 333)
{
   SetConfirmedVirt(data);
   SetRealHyp(data);
}

///////////////////////////////////////////////////////////////////
