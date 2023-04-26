#ifndef DIMLP_H
#define DIMLP_H

using namespace std;
#include "bpNN.h"

class Dimlp : public BpNN {

  //---------------------------------------------------------------------

public:
  float Error(DataSet &data, DataSet &target, float *accuracy)

  {
    return ComputeError(data, target, accuracy);
  }

  void Train(DataSet &train, DataSet &trainTar, DataSet &test, DataSet &testTar,
             DataSet &valid, DataSet &validTar, char *accuracyFile,
             bool fromBT = false)

  {
    TrainPhase(train, trainTar, test, testTar, valid, validTar, accuracyFile,
               fromBT);
  }

  //---------------------------------------------------------------------

  Dimlp(float eta, float mu, float flat, float errParam, float accuracyParam,
        float deltaErrParam, int discrLevels, int showErrParam,
        int nbEpochsParam, int nbLayers, int nbNeurons[], char weightFile[]);

  Dimlp(char readFile[], int nbLayers, int nbNeurons[], int discrLevels);

  Dimlp(char readFile[], float eta, float mu, float flat, float errParam,
        float accuracyParam, float deltaErrParam, int discrLevels,
        int showErrParam, int nbEpochsParam, int nbLayers, int nbNeurons[],
        char weightFile[]);
};

#endif
