#include "dimlp.h"
using namespace std;

////////////////////////////////////////////////////////////////////////
const char netType[] = "Discretized Interpretable Multi-Layer Perceptron";

Dimlp::Dimlp(
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int discrLevels,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const char weightFile[],
    int seed) :

                BpNN(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
                     showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
                     netType, seed)

{
  cout << "Number of quantized levels = " << discrLevels << "\n"
       << endl;

  DefineDimlp(discrLevels);
}

////////////////////////////////////////////////////////////////////////

Dimlp::Dimlp(
    const char readFile[],
    int nbLayers,
    const std::vector<int> &nbNeurons,
    int discrLevels) :

                       BpNN(readFile, nbLayers, nbNeurons,
                            netType)

{
  cout << "Number of quantized levels = " << discrLevels << "\n"
       << endl;

  DefineDimlp(discrLevels);
  ReadWeights();
}

////////////////////////////////////////////////////////////////////////

Dimlp::Dimlp(
    const char readFile[],
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int discrLevels,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const char weightFile[],
    int seed) :

                BpNN(readFile, eta, mu, flat, errParam, accuracyParam, deltaErrParam,
                     showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
                     netType, seed) {
  cout << "Number of quantized levels = " << discrLevels << "\n"
       << endl;

  DefineDimlp(discrLevels);
  ReadWeights();
}

////////////////////////////////////////////////////////////////////////
