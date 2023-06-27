#ifndef LAYER_H
#define LAYER_H

#include <iostream>

#include "errFunct.h"
#include "randFun.h"
#include "standAct.h"
#include <math.h>

const int LAYER = 1;

///////////////////////////////////////////////////////////////////

class Layer {
  float Eta;
  float Mu;
  float Flat;

  float EtaCentre;
  float EtaSpread;

  int NbDown;
  int NbUp;
  int NbWeights;
  int NbWeightsForInit;

  float *Down;
  float *DeltaDown;
  std::vector<float> Up;
  std::vector<float> DeltaUp;

  std::vector<float> Weights;
  std::vector<float> OldWeights;
  std::vector<float> ValidWeights;
  std::vector<float> BiasWeights;
  std::vector<float> OldBiasWeights;
  std::vector<float> ValidBiasWeights;

  //----------------------------------------------------------------

  void AssignParam(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);

  void CreateStruct();

  void ApplyTransf1();
  void ApplyTransf2();

  void ForwFully();

  void ComputeDeltaDownStand();

  void AdaptBiasStand();
  void AdaptWeightsFully();

  //----------------------------------------------------------------

public:
  void InitWeights();

  int GetNbDown() const { return NbDown; }
  int GetNbUp() const { return NbUp; }

  float *GetDown() { return Down; }
  float *GetUp() { return Up.data(); }
  float *GetDeltaUp() { return DeltaUp.data(); }

  float *GetWeights() { return Weights.data(); }
  float *GetBias() { return BiasWeights.data(); }

  void SetDown(float pat[]) { Down = pat; }
  void SetDeltaDown(float pat[]) { DeltaDown = pat; }

  virtual float Activation1(float x) { return Sigmoid(x); }
  virtual float Activation2(float x) { return Sigmoid(x); }

  virtual float HalfErrFunct(int nbTar, const std::vector<float> &netOut, const std::vector<float> &target) { return Lmse(nbTar, netOut, target); }

  void ReadWeights(std::istream &inFile);
  void WriteWeights(std::ostream &outFile);
  void PushWeights();
  void PopWeights();

  void ForwSpec();
  void ForwSpec2();
  void ForwRadial();
  virtual void ForwLayer() {
    ForwFully();
  }

  void ForwAndTransf1() {
    ForwLayer();
    ApplyTransf1();
  }
  void ForwAndTransf2() {
    ForwLayer();
    ApplyTransf2();
  }

  void ComputeDeltaOut(const float target[]);

  void ComputeDeltaDownSpec2();
  virtual void ComputeDeltaDown() { ComputeDeltaDownStand(); }

  void AdaptBiasSpec2();
  virtual void AdaptBias() { AdaptBiasStand(); }

  void AdaptWeightsSpec();
  void AdaptWeightsSpec2();
  virtual void AdaptWeights() { AdaptWeightsFully(); }

  void BackLayer() {
    AdaptWeights();
    AdaptBias();
    ComputeDeltaDown();
  }
  void BackLayerWithout() {
    AdaptWeights();
    AdaptBias();
  }

  void SetEtas(float etaCentre, float etaSpread) {
    EtaCentre = etaCentre;
    EtaSpread = etaSpread;
  }

  //----------------------------------------------------------------

  virtual ~Layer() = default;

  Layer(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);
};

///////////////////////////////////////////////////////////////////

#endif
