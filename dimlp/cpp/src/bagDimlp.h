#ifndef BAGDIMLP_H
#define BAGDIMLP_H

using namespace std;
#include "dimlp.h"
#include "virtHyp.h"

#define VIRT 1

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
  char *WeightFile;

  std::vector<int> NbNeurons;

  float *GlobalOut;

  Dimlp **VectDimlp;
  DataSet **VectData;
  DataSet **VectDataClass;
  DataSet **ValData;
  DataSet **ValDataClass;

  //---------------------------------------------------------------------

  //---------------------------------------------------------------------

public:
  float *GetGlobalOut() { return GlobalOut; }
  void MakeDataSets(DataSet &masterTrain, DataSet &masterClass, int nbPat);

  void TrainAll(
      DataSet &masterTrain,
      DataSet &masterClass,
      DataSet &test,
      DataSet &testTar,
      char genericWeightsFile[],
      char *accuracyFile,
      int seed = 0);
  void DefNetsWithWeights(char *prefix);

  VirtualHyp *MakeGlobalVirt(int nbBins, int nbIn, int multiple);

  void ForwardOneExample1(DataSet &data, int index);
  void ForwardOneExample1();

  void ComputeAcc(DataSet &data, DataSet &target, float *accuracy, int tW, char predFile[]);

  void Del();

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
      char weightFile[],
      int seed = 0);

  BagDimlp(
      int discrLevels,
      int nbLayers,
      std::vector<int> nbNeurons,
      int nbDimlpNets,
      char weightFile[],
      int seed = 0);
};

#endif
