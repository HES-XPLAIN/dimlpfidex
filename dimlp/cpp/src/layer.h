#ifndef LAYER_H
#define LAYER_H

using namespace std;
#include <iostream>

#include "errFunct.h"
#include "standAct.h"

#define LAYER 1

///////////////////////////////////////////////////////////////////

class Layer {
  float Eta, Mu, Flat;

  float EtaCentre, EtaSpread;

  int NbDown;
  int NbUp;
  int NbWeights;
  int NbWeightsForInit;

  float *Down;
  float *DeltaDown;
  float *Up;
  float *DeltaUp;

  float *Weights;
  float *OldWeights;
  float *ValidWeights;
  float *BiasWeights;
  float *OldBiasWeights;
  float *ValidBiasWeights;

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

  int GetNbDown() { return NbDown; }
  int GetNbUp() { return NbUp; }

  float *GetDown() { return Down; }
  float *GetUp() { return Up; }
  float *GetDeltaUp() { return DeltaUp; }

  float *GetWeights() { return Weights; }
  float *GetBias() { return BiasWeights; }

  void SetDown(float pat[]) { Down = pat; }
  void SetDeltaDown(float pat[]) { DeltaDown = pat; }

  virtual float Activation1(float x) { return Sigmoid(x); }
  virtual float Activation2(float x) { return Sigmoid(x); }

  virtual float HalfErrFunct(int nbTar, float netOut[], float target[]) { return Lmse(nbTar, netOut, target); }

  void ReadWeights(istream &inFile);
  void WriteWeights(ostream &outFile);
  void PushWeights();
  void PopWeights();

  void ForwSpec();
  void ForwSpec2();
  void ForwRadial();
  virtual void ForwLayer() { ForwFully(); }

  void ForwAndTransf1() {
    ForwLayer();
    ApplyTransf1();
  }
  void ForwAndTransf2() {
    ForwLayer();
    ApplyTransf2();
  }

  void ComputeDeltaOut(float target[]);

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
  void Del();

  //----------------------------------------------------------------

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
