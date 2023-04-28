using namespace std;
#include "dimlp.h"
////////////////////////////////////////////////////////////////////////
char netType[] = "Discretized Interpretable Multi-Layer Perceptron";

Dimlp::Dimlp
(
    float    eta,
    float    mu,
    float    flat,
    float    errParam,
    float    accuracyParam,
    float    deltaErrParam,
    int      discrLevels,
    int      showErrParam,
    int      nbEpochsParam,
    int      nbLayers,
    int      nbNeurons[],
    char     weightFile[],
    int      seed) :

    BpNN(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
         showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
         netType, seed)

{
   cout << "Number of quantized levels = " << discrLevels << "\n\n";

   DefineDimlp(discrLevels);
}

////////////////////////////////////////////////////////////////////////

Dimlp::Dimlp
(
   char  readFile[],
   int   nbLayers,
   int   nbNeurons[],
   int   discrLevels)  :

   BpNN(readFile, nbLayers, nbNeurons,
        netType)

{
   cout << "Number of quantized levels = " << discrLevels << "\n\n";

   DefineDimlp(discrLevels);
   ReadWeights();
}

////////////////////////////////////////////////////////////////////////

Dimlp::Dimlp
(
    char     readFile[],
    float    eta,
    float    mu,
    float    flat,
    float    errParam,
    float    accuracyParam,
    float    deltaErrParam,
    int      discrLevels,
    int      showErrParam,
    int      nbEpochsParam,
    int      nbLayers,
    int      nbNeurons[],
    char     weightFile[],
    int      seed) :

    BpNN(readFile, eta, mu, flat, errParam, accuracyParam, deltaErrParam,
         showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
         netType, seed)
{
   cout << "Number of quantized levels = " << discrLevels << "\n\n";

   DefineDimlp(discrLevels);
   ReadWeights();
}

////////////////////////////////////////////////////////////////////////
