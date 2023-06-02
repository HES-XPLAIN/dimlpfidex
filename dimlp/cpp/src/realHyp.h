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
  std::shared_ptr<BpNN> Bpnn;
  std::shared_ptr<VirtualHyp> Virt;
  std::shared_ptr<ThresDescr> Descr;
  std::shared_ptr<RuleProcessing> SavedRules;

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

  std::vector<std::vector<int>> ConfirmedVirt;
  std::vector<int> ConfBefFirstHyp;
  std::vector<int> ClassPatNet;

  int Aborted;
  //----------------------------------------------------------------

  void SaveRule(std::shared_ptr<Rule> path) const { SavedRules->Insert(path); }

  int MaxOnPos(const std::vector<int> &vec) const;
  int GiveIndMax(const int *vec, int nbEl) const;

  void SetConfirmedVirt2();

  void Gl1(int var, int indPat, int startVirt, int netAns);
  void Gr1(int var, int indPat, int startVirt, int netAns);

  virtual void GoLeft(int var, int indPat, int startVirt, int netAns) { Gl1(var, indPat, startVirt, netAns); }

  virtual void GoRight(int var, int indPat, int startVirt, int netAns) { Gr1(var, indPat, startVirt, netAns); }

  void OneExRealHyp(DataSet &data, int indPat);

  std::shared_ptr<Ante> FindMostDiscrAnt(int sel) const;

  int ComputeCorrect(std::shared_ptr<StringInt> listPatLeft, std::shared_ptr<StringInt> listPatRight);
  void SetCountPatDiscr(std::shared_ptr<StringInt> listPat, std::shared_ptr<Rule> r) const;
  void SetCountPatDiscr2(DataSet &data, std::shared_ptr<Rule> r);

  void DeepSearch(DataSet &data, std::shared_ptr<Rule> path, std::shared_ptr<StringInt> subSet);
  void DeepSearch2(DataSet &data, std::shared_ptr<Rule> path);

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

  virtual ~RealHyp() = default;

  RealHyp(
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  RealHyp(
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);

  RealHyp(
      std::shared_ptr<VirtualHyp> globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  RealHyp(
      std::shared_ptr<VirtualHyp> globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);
};

////////////////////////////////////////////////////////////////////////

#endif
