#ifndef DIMLP_H
#define DIMLP_H

#include "bpNN.h"
#include <vector>

class Dimlp : public BpNN {

  //---------------------------------------------------------------------

public:
  float Error(DataSet &data, DataSet &target, float *accuracy)

  { return ComputeError(data, target, accuracy); }

  void Train(DataSet &train,
             DataSet &trainTar,
             DataSet &test,
             DataSet &testTar,
             DataSet &valid,
             DataSet &validTar,
             const std::string &accuracyFile,
             bool fromBT = false)

  { TrainPhase(train, trainTar, test, testTar, valid, validTar, accuracyFile, fromBT); }

  //---------------------------------------------------------------------

  ~Dimlp() override = default; // Virtual destructor

  Dimlp(
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
      const std::string &weightFile,
      int seed = 0);

  Dimlp(const std::string &readFile, int nbLayers, const std::vector<int> &nbNeurons, int discrLevels);

  Dimlp(
      const std::string &readFile,
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
      const std::string &weightFile,
      int seed = 0);
};

#endif
