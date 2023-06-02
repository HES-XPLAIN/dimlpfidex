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
  std::vector<int> TabRules;
  std::shared_ptr<ThresDescr> Descr;

  StringInt Gain;
  StringInt RuleInd;
  StringInt AntInd;

  struct Saved {
    std::shared_ptr<Rule> OneRule;
    std::shared_ptr<StringInt> Carried;
    std::shared_ptr<Saved> Next;
  };

  std::shared_ptr<Saved> First;
  std::shared_ptr<Saved> Current;
  std::shared_ptr<Saved> Memory;

  //----------------------------------------------------------------

  int Max(const std::vector<int> &tab, int nbRules) const;
  int Min(const std::vector<int> &tab, int nbRules) const;
  void SetCountAntRules();
  void SetCarriedField();
  void DelListCar();
  int CheckAllCarried(int toDrop);
  void RemCurrentRule() const;
  int GoToSavedAndRemRule(int indPrune);
  void GoToRuleAndRemAnt(int indPrune);
  void RemSevThres(std::shared_ptr<Rule> r) const;
  void ComputeGain();
  void RulePruneByMinCar();
  void FastRulePrune(int nbIt);
  int TryEnlargedThres();

  //----------------------------------------------------------------

public:
  int IsRuleEmpty(std::shared_ptr<Rule> rule) const;
  int GetNbRules() const { return NbRules; }
  void GoToBeg() { Current = First; }
  void GoToNext() { Current = Current->Next; }
  std::shared_ptr<Rule> GetRule() const { return Current->OneRule; }
  void Save() { Memory = Current; }
  void Previous() { Current = Memory; }

  int CountAnt();
  int CountRules();

  void Clean();
  void MixPrune();
  void EnlargeAndPrune();

  void Insert(std::shared_ptr<Rule> r);
  void Del();

  RuleProcessing(
      int nbVar,
      int nbHyp,
      DataSet data,
      const std::vector<int> &classPatNet,
      std::shared_ptr<ThresDescr> descr);

  RuleProcessing() = default;
};

#endif
