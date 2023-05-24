#ifndef BPNN_H
#define BPNN_H

using namespace std;
#include "randFun.h"
#include <iostream>
#include <vector>

#ifndef DATASETS
#include "dataSet.h"
#endif

#include "layerDmp.h"

#define LD2 1
#include "layerD2.h"

///////////////////////////////////////////////////////////////////

class BpNN {
  char ReadFile[80];
  char SaveFile[80];

  float Eta;
  float Mu;
  float Flat;

  float ErrParam, AccuracyParam, DeltaErrParam;
  int ShowErrParam, NbEpochsParam;

  int NbLayers; // Including Input and Output
  int NbWeightLayers;
  int *NbNeurons;

  Layer **VecLayer;

  //------------------------------------------------------------------------

  void InitRandomGen(int seed = 0);
  void CreateNetStruct(std::vector<int> nbNeurons);
  void WriteArchParam();

  void AssignParam(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const char saveFile[]);

  void WriteParam();
  void SaveWeights();
  void Push();
  void Pop();

  //------------------------------------------------------------------------

  void ForwardOneExample2(DataSet &data, int index);

  float ComputeErrorSameAct(
      DataSet &data,
      DataSet &target,
      float *accuracy);

  //------------------------------------------------------------------------

  void BackOneExample(DataSet &target, int index);
  void TrainOneEpoch(DataSet &data, DataSet &target, IntRandomFunction *r);

  //------------------------------------------------------------------------

public:
  int GetNbLayers() { return NbLayers; }
  int GetNbWeightLayers() { return NbWeightLayers; }
  int Max(float vec[], int nbEl);
  Layer *GetLayer(int indLayer) { return VecLayer[indLayer]; }

  void SaveWeights(char *str);

  virtual void ForwardOneExample1(DataSet &data, int index);
  virtual void ForwardOneExample1(float *ex);
  virtual void ForwardOneExample1();

  void ReadWeights();

  void PrintSpecErrDimlp(float specErr, float specAcc);

  virtual void PrintSpecErr(float err, float acc) { PrintSpecErrDimlp(err, acc); }

  void DefineSmlp();
  void DefineDimlp(int discrLevels);
  void DefineQmlp(int discrLevels);
  void DefineFdimlp(int discrLevels);
  void DefineFdimlp2(int discrLevels);

  void DefineSD(int discrLevels);
  void DefineSP3(int discrLevels);
  void DefineSP4(int discrLevels);
  void DefineSP5(int discrLevels);
  void DefineSR(int discrLevels);

  //------------------------------------------------------------------------

  float ComputeError(
      DataSet &data,
      DataSet &target,
      float *accuracy);

  void TrainPhase(DataSet &train,
                  DataSet &trainTar,
                  DataSet &test,
                  DataSet &testTar,
                  DataSet &valid,
                  DataSet &validTar,
                  const char *accuracyFile,
                  bool fromBT = false);

  //------------------------------------------------------------------------

  void Del();

  ~BpNN() { Del(); }

  BpNN(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      std::vector<int> nbNeurons,
      const char saveFile[],
      const char printNetType[],
      int seed = 0);

  BpNN(
      const char readFile[],
      int nbLayers,
      std::vector<int> nbNeurons,
      const char printNetType[]);

  BpNN(
      const char readFile[],
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      std::vector<int> nbNeurons,
      const char saveFile[],
      const char printNetType[],
      int seed = 0);
};

//------------------------------------------------------------------------

#endif
