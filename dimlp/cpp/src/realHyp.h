#ifndef REALHYP_H
#define REALHYP_H

#ifndef BPNN
#include "bpNN.h"
#endif

#ifndef VIRT
#include "virtHyp.h"
#endif

#include "ante.h"
#include "attrName.h"
#include "rulePro.h"
#include "thresD.h"
#include <vector>

const int STRINGINT = 1;
const int THRESDESCR = 1;
const int RULE = 1;
const int DATASETS = 1;

////////////////////////////////////////////////////////////////////////

class RealHyp {
  BpNN *Bpnn;
  VirtualHyp *Virt;
  ThresDescr *Descr;
  RuleProcessing *SavedRules;

  //----------------------------------------------------------------

  float *In;
  float *Out;
  float *Bias;
  float *Weights;

  int NbBins;
  int NbIn;
  int Multiple;
  int NbHyp;
  int NbWeightLayers;
  int NbOut; // Attenzione al numero di classi quando = 1

  int **ConfirmedVirt;
  int *ConfBefFirstHyp;
  std::vector<int> ClassPatNet;

  int Aborted;
  //----------------------------------------------------------------

  void SaveRule(Rule *path) { SavedRules->Insert(path); }

  int MaxOnPos(const int *vec, int nbEl) const;
  int GiveIndMax(const int *vec, int nbEl) const;

  void SetConfirmedVirt2();

  void Gl1(int var, int indPat, int startVirt, int netAns);
  void Gr1(int var, int indPat, int startVirt, int netAns);

  virtual void GoLeft(int var, int indPat, int startVirt, int netAns) { Gl1(var, indPat, startVirt, netAns); }

  virtual void GoRight(int var, int indPat, int startVirt, int netAns) { Gr1(var, indPat, startVirt, netAns); }

  void OneExRealHyp(DataSet &data, int indPat);

  Ante *FindMostDiscrAnt(int sel);

  int ComputeCorrect(StringInt *listPatLeft, StringInt *listPatRight);
  void SetCountPatDiscr(StringInt *listPat, Rule *r);
  void SetCountPatDiscr2(DataSet &data, Rule *r);

  void DeepSearch(DataSet &data, Rule *path, StringInt *subSet);
  void DeepSearch2(DataSet &data, Rule *path);

  //----------------------------------------------------------------

public:
  int TreeAborted() const { return Aborted; }

  void SetConfirmedVirt(DataSet &data);
  void SetRealHyp(DataSet &data);

  void Gl2(int var, int indPat, int startVirt, int netAns);
  void Gr2(int var, int indPat, int startVirt, int netAns);

  void RuleExtraction(
      DataSet &data,
      const DataSet &train,
      const DataSet &trainClass,
      const DataSet &valid,
      const DataSet &validClass,
      const DataSet &test,
      const DataSet &testClass,
      const AttrName &attr,
      std::ostream &ruleFile);

  //----------------------------------------------------------------

  void Del();

  virtual ~RealHyp() = default;

  RealHyp(
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  RealHyp(
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);

  RealHyp(
      VirtualHyp *globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  RealHyp(
      VirtualHyp *globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      BpNN *nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);
};

////////////////////////////////////////////////////////////////////////

#endif
