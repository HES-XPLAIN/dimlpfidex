#ifndef BAGDIMLP_H
#define BAGDIMLP_H

#include "dimlp.h"
#include "virtHyp.h"

const int VIRT = 1;

class BagDimlp : public Dimlp {
  int NbDimlpNets;
  int NbOut;

  float Eta;
  float Mu;
  float Flat;
  float ErrParam;
  float AccuracyParam;
  float DeltaErrParam;
  int DiscrLevels;
  int ShowErrParam;
  int NbEpochsParam;
  int NbLayers;
  const char *WeightFile;

  std::vector<int> NbNeurons;

  std::vector<float> GlobalOut;

  std::vector<std::shared_ptr<Dimlp>> VectDimlp;
  std::vector<std::shared_ptr<DataSet>> VectData;
  std::vector<std::shared_ptr<DataSet>> VectDataClass;
  std::vector<std::shared_ptr<DataSet>> ValData;
  std::vector<std::shared_ptr<DataSet>> ValDataClass;

  //---------------------------------------------------------------------

  //---------------------------------------------------------------------

public:
  float *GetGlobalOut() { return GlobalOut.data(); }
  void MakeDataSets(DataSet &masterTrain, DataSet &masterClass, int nbPat);

  void TrainAll(
      DataSet &test,
      DataSet &testTar,
      const char genericWeightsFile[],
      const char *accuracyFile,
      int seed = 0);
  void DefNetsWithWeights(const char *prefix);

  std::shared_ptr<VirtualHyp> MakeGlobalVirt(int nbBins, int nbIn, int multiple);

  using BpNN::ForwardOneExample1;
  void ForwardOneExample1(DataSet &data, int index) override;
  void ForwardOneExample1() override;

  void ComputeAcc(DataSet &data, DataSet &target, float *accuracy, int tW, const char predFile[]);

  BagDimlp(
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
      std::vector<int> nbNeurons,
      int nbDimlpNets,
      const char weightFile[],
      int seed = 0);

  BagDimlp(
      int discrLevels,
      int nbLayers,
      std::vector<int> nbNeurons,
      int nbDimlpNets,
      const char weightFile[],
      int seed = 0);
};

#endif
