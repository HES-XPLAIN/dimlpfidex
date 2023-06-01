#ifndef RULEPRO_H
#define RULEPRO_H

#ifndef DATASETS
#include "dataSet.h"
#endif

#ifndef THRESDESCR
#include "thresD.h"
#endif

#include <vector>

////////////////////////////////////////////////////////////////////////

class RuleProcessing

{
  int NbRules = 0;
  int NbVar;
  int NbHyp;

  DataSet Data;
  std::vector<int> ClassPatNet;
  int *TabRules;
  ThresDescr *Descr;

  StringInt Gain;
  StringInt RuleInd;
  StringInt AntInd;

  struct Saved {
    Rule *OneRule;
    StringInt *Carried;
    Saved *Next;
  };

  Saved *First;
  Saved *Current;
  Saved *Memory;

  //----------------------------------------------------------------

  int Max(const int *tab, int nbRules) const;
  int Min(const int *tab, int nbRules) const;
  void SetCountAntRules();
  void SetCarriedField();
  void DelListCar();
  int CheckAllCarried(int toDrop);
  void RemCurrentRule();
  int GoToSavedAndRemRule(int indPrune);
  void GoToRuleAndRemAnt(int indPrune);
  void RemSevThres(Rule *r);
  void ComputeGain();
  void RulePruneByMinCar();
  void FastRulePrune(int nbIt);
  int TryEnlargedThres();

  //----------------------------------------------------------------

public:
  int IsRuleEmpty(Rule *rule) const;
  int GetNbRules() const { return NbRules; }
  void GoToBeg() { Current = First; }
  void GoToNext() { Current = Current->Next; }
  Rule *GetRule() { return Current->OneRule; }
  void Save() { Memory = Current; }
  void Previous() { Current = Memory; }

  int CountAnt();
  int CountRules();

  void Clean();
  void MixPrune();
  void EnlargeAndPrune();

  void Insert(Rule *r);
  void Del();

  RuleProcessing(
      int nbVar,
      int nbHyp,
      DataSet data,
      const std::vector<int> &classPatNet,
      ThresDescr *descr);

  RuleProcessing() = default;
};

#endif
