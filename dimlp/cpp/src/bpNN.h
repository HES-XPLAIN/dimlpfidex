#ifndef BPNN_H
#define BPNN_H

#include "randFun.h"
#include <iostream>
#include <vector>

#ifndef DATASETS
#include "dataSet.h"
#endif

#include "layerD2.h"
#include "layerDmp.h"

#include "writeErr.h"
#include <math.h>

#include "layerD3.h"
#include "layerD4.h"
#include "layerFdp.h"

#include "layerRad.h"
#include "layerSD.h"
#include "layerSP3.h"
#include "layerSP4.h"
#include "layerSP5.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

const int LD2 = 1;

///////////////////////////////////////////////////////////////////

class BpNN {
  std::string ReadFile;
  std::string SaveFile;

  float Eta;
  float Mu;
  float Flat;

  float ErrParam;
  float AccuracyParam;
  float DeltaErrParam;
  int ShowErrParam;
  int NbEpochsParam;

  int NbLayers; // Including Input and Output
  int NbWeightLayers;
  std::vector<int> NbNeurons;

  std::vector<std::shared_ptr<Layer>> VecLayer;

  static int initRandomGen;

  //------------------------------------------------------------------------

  void InitRandomGen(int seed = 0) const;
  void CreateNetStruct(std::vector<int> nbNeurons);
  void WriteArchParam() const;

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
      const std::string &saveFile);

  void WriteParam() const;
  void SaveWeights() const;
  void Push() const;
  void Pop() const;

  //------------------------------------------------------------------------

  void ForwardOneExample2(DataSet &data, int index) const;

  float ComputeErrorSameAct(
      DataSet &data,
      DataSet &target,
      float *accuracy) const;

  //------------------------------------------------------------------------

  void BackOneExample(DataSet &target, int index) const;
  void TrainOneEpoch(DataSet &data, DataSet &target, const IntRandomFunction *r) const;

  //------------------------------------------------------------------------

public:
  static void resetInitRandomGen();
  int GetNbLayers() const { return NbLayers; }
  int GetNbWeightLayers() const { return NbWeightLayers; }
  int Max(const std::vector<float> &vec) const;
  std::shared_ptr<Layer> GetLayer(int indLayer) { return VecLayer[indLayer]; }

  void SaveWeights(const char *str) const;

  virtual void ForwardOneExample1(DataSet &data, int index);
  virtual void ForwardOneExample1(float *ex);
  virtual void ForwardOneExample1();

  void ReadWeights() const;

  void PrintSpecErrDimlp(float specErr, float specAcc) const;

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

  BpNN(const BpNN &) = default;            // Default copy constructor
  BpNN &operator=(const BpNN &) = default; // Default copy assignment operator

  virtual ~BpNN() = default; // virtual destructor

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
      const std::vector<int> &nbNeurons,
      const std::string &saveFile,
      const char printNetType[],
      int seed = 0);

  BpNN(
      const std::string &readFile,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const char printNetType[]);

  BpNN(
      const std::string &readFile,
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &saveFile,
      const char printNetType[],
      int seed = 0);
};

//------------------------------------------------------------------------

#endif
