#include "rulePro.h"
#include "misc.h"
#include "randFun.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////

int RuleProcessing::Max(const std::vector<int> &tab, int nbRules) const

{
  int max;
  int newVal;
  int indMax;

  if (nbRules == 0)
    return -1;

  if (nbRules == 1)
    return 0;

  max = tab[0];
  indMax = 0;

  for (int e = 1; e < nbRules; e++) {
    newVal = tab[e];

    if (newVal > max) {
      max = newVal;
      indMax = e;
    }
  }

  if (max == 0)
    return -1;

  return indMax;
}

///////////////////////////////////////////////////////////////////

int RuleProcessing::Min(const std::vector<int> &tab, int nbRules) const

{
  int min;
  int newVal;
  int indMin;

  if (nbRules == 0)
    return -1;

  if (nbRules == 1)
    return 0;

  min = 1111111111;

  for (int e = 0; e < nbRules; e++) {
    newVal = tab[e];

    if ((newVal > 0) && (newVal < min)) {
      min = newVal;
      indMin = e;
    }
  }

  if (min == 1111111111)
    return -1;

  return indMin;
}

///////////////////////////////////////////////////////////////////

int RuleProcessing::IsRuleEmpty(std::shared_ptr<Rule> rule) const

{
  int r;
  int nbAnt = rule->GetNbAnt();

  for (r = 0, rule->GoToBeg(); r < nbAnt; r++, rule->GoToNext())
    if (rule->IsAntDeleted() == 0)
      break;

  return ((r == nbAnt) ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////

int RuleProcessing::CountAnt()

{
  int a;
  int r;
  int nbAnt;
  int count;
  std::shared_ptr<Rule> rule;

  for (r = 0, count = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();
    nbAnt = rule->GetNbAnt();

    for (a = 0, rule->GoToBeg(); a < nbAnt; a++, rule->GoToNext())
      if (rule->IsAntDeleted() == 0)
        count++;
  }

  return count;
}

////////////////////////////////////////////////////////////////////////

int RuleProcessing::CountRules()

{
  int r;
  int count;

  for (r = 0, count = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    if (IsRuleEmpty(GetRule()) == 1)
      continue;

    count++;
  }

  return count;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::SetCarriedField()

{
  int r;
  std::shared_ptr<Rule> rule;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule) == 1)
      continue;

    Current->Carried = Data.Select(rule);
  }
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::DelListCar()

{
  int r;
  std::shared_ptr<Rule> rule;
  std::shared_ptr<StringInt> carried;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule) == 1)
      continue;

    carried = Current->Carried;
    carried->Del();
  }
}

////////////////////////////////////////////////////////////////////////

int RuleProcessing::CheckAllCarried(int toDrop)

{
  int p;
  int r;
  int nbEl;

  int nbEx = Data.GetNbEx();
  std::vector<int> checkTab(nbEx, 0);
  std::shared_ptr<Rule> rule;
  std::shared_ptr<StringInt> carriedEx;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    if (r == toDrop)
      continue;

    rule = GetRule();

    if (IsRuleEmpty(rule) == 1)
      continue;

    carriedEx = Current->Carried;

    nbEl = carriedEx->GetNbEl();

    for (p = 0, carriedEx->GoToBeg(); p < nbEl; p++, carriedEx->GoToNext())
      checkTab[carriedEx->GetVal()] += 1;
  }

  for (p = 0; p < nbEx; p++)
    if (checkTab[p] == 0) {
      return 0;
    }

  return 1;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::SetCountAntRules()

{
  int r;
  std::shared_ptr<Rule> rule;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule)) {
      TabRules[r] = 0;
      continue;
    }

    TabRules[r] = rule->GetNbAntWithout();
  }
}

////////////////////////////////////////////////////////////////////////

int RuleProcessing::GoToSavedAndRemRule(int indPrune)

{
  int r;
  std::shared_ptr<Rule> rule;

  if (CheckAllCarried(indPrune) == 1) {
    for (r = 0, GoToBeg(); r < indPrune; r++, GoToNext())
      ;

    rule = GetRule();
    rule->DeleteRule();
    (Current->Carried)->Del();

    return 1;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::FastRulePrune(int nbIt)

{
  int count;
  int i;
  int indMax;

  TabRules.resize(NbRules);

  SetCountAntRules();
  SetCarriedField();

  for (i = 0, count = 0; i < nbIt; i++) {
    indMax = Max(TabRules, NbRules);

    if (indMax < 0)
      break;

    TabRules[indMax] = 0;
    count += GoToSavedAndRemRule(indMax);
  }

  DelListCar();
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::RulePruneByMinCar()

{
  int count;
  int r;
  int indMin;
  std::shared_ptr<StringInt> carried;

  TabRules.resize(NbRules);

  SetCountAntRules();
  SetCarriedField();

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    if (TabRules[r] == 0)
      continue;

    carried = Data.Select(GetRule());
    TabRules[r] = carried->GetNbEl();
    carried->Del();
  }

  for (r = 0, count = 0; r < NbRules; r++) {
    indMin = Min(TabRules, NbRules);

    if (indMin < 0)
      break;

    TabRules[indMin] = 0;
    count += GoToSavedAndRemRule(indMin);
  }

  DelListCar();
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::GoToRuleAndRemAnt(int indPrune)

{
  int a;
  int r;
  int indRule;
  int indAnt;
  int remAnt;
  std::shared_ptr<Rule> rule;
  std::shared_ptr<StringInt> newCarried;

  for (r = 0, RuleInd.GoToBeg(); r < indPrune; r++, RuleInd.GoToNext())
    ;
  indRule = RuleInd.GetVal();

  for (r = 0, AntInd.GoToBeg(); r < indPrune; r++, AntInd.GoToNext())
    ;
  indAnt = AntInd.GetVal();

  for (r = 0, GoToBeg(); r < indRule; r++, GoToNext())
    ;

  rule = GetRule();

  for (a = 0, rule->GoToBeg(); a < indAnt; a++, rule->GoToNext())
    ;

  remAnt = rule->GetVar();
  rule->RemAnt();

  rule->SavePtrAnt();
  newCarried = Data.Select(rule);
  rule->PrevPtrAnt();

  if (AreSameClass(newCarried, ClassPatNet) == 0) {
    rule->SetAnt(remAnt);
  }

  newCarried->Del();
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::ComputeGain()

{
  int a;
  int r;
  int nbAnt;
  int remAnt;
  int diff;
  std::shared_ptr<Rule> rule;
  std::shared_ptr<StringInt> oldCarried;
  std::shared_ptr<StringInt> newCarried;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule))
      continue;

    oldCarried = Data.Select(rule);
    nbAnt = rule->GetNbAnt();

    for (a = 0, rule->GoToBeg(); a < nbAnt; a++, rule->GoToNext()) {
      if (rule->IsAntDeleted() == 1)
        continue;

      remAnt = rule->GetVar();
      rule->RemAnt();

      rule->SavePtrAnt();
      newCarried = Data.Select(rule);
      rule->PrevPtrAnt();

      rule->SetAnt(remAnt);

      if (AreSameClass(newCarried, ClassPatNet) == 1) {
        diff = newCarried->GetNbEl() - oldCarried->GetNbEl();

        RuleInd.Insert(r);
        AntInd.Insert(a);
        Gain.Insert(diff);
      }
      newCarried->Del();
    }

    oldCarried->Del();
  }
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::MixPrune()

{
  int indPrune;
  int nbIt;

  ComputeGain();
  nbIt = Gain.GetNbEl();

  if (nbIt > 10)
    nbIt = nbIt / 10;

  Gain.Del();
  RuleInd.Del();
  AntInd.Del();

  for (;;) {
    std::cout << "\nNumber of rules = " << CountRules() << std::endl;
    std::cout << "Number of antecedents = " << CountAnt() << std::endl;

    ComputeGain();

    std::cout << "Number of possible antecedents to prune = ";
    std::cout << Gain.GetNbEl() << std::endl;

    if (Gain.GetNbEl() == 0)
      return;

    for (int i = 0; i < nbIt; i++) {
      indPrune = Gain.FindIndMax();

      if (indPrune < 0)
        break;

      Gain.DelVal(indPrune, -1111111111);
      GoToRuleAndRemAnt(indPrune);
    }

    Gain.Del();
    RuleInd.Del();
    AntInd.Del();

    FastRulePrune(NbRules);
  }
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::RemSevThres(std::shared_ptr<Rule> r) const

{
  int a;
  int h;
  int first;
  float min;
  float max;
  float currentVal;

  std::vector<int> tabFreqLess(NbVar);
  std::vector<int> tabFreqGreat(NbVar);

  for (h = 0; h < NbVar; h++) {
    tabFreqLess[h] = 0;
    tabFreqGreat[h] = 0;
  }

  int nbAnt = r->GetNbAnt();

  for (a = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
    switch (r->GetRel()) {
    case '<':
      tabFreqLess[r->GetVar()] += 1;
      break;

    case '>':
      tabFreqGreat[r->GetVar()] += 1;
      break;

    default:
      break;
    }
  }

  for (h = 0; h < NbVar; h++) {
    if (tabFreqGreat[h] > 1) {
      for (a = 0, r->GoToBeg(), first = 0; a < nbAnt; a++, r->GoToNext()) {
        if ((r->GetVar() == h) && (r->GetRel() == '>')) {
          first++;

          if (first == 1)
            max = r->GetVal();
          else {
            currentVal = r->GetVal();

            if (currentVal > max)
              max = currentVal;
          }
        }
      }

      for (a = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
        if ((r->GetVar() == h) && (r->GetRel() == '>') && (r->GetVal() != max))
          r->RemAnt();
      }
    }
  }

  for (h = 0; h < NbVar; h++) {
    if (tabFreqLess[h] > 1) {
      for (a = 0, r->GoToBeg(), first = 0; a < nbAnt; a++, r->GoToNext()) {
        if ((r->GetVar() == h) && (r->GetRel() == '<')) {
          first++;

          if (first == 1)
            min = r->GetVal();
          else {
            currentVal = r->GetVal();

            if (currentVal < min)
              min = currentVal;
          }
        }
      }

      for (a = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
        if ((r->GetVar() == h) && (r->GetRel() == '<') && (r->GetVal() != min))
          r->RemAnt();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::Clean()

{
  int r;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext())
    RemSevThres(GetRule());
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::Insert(std::shared_ptr<Rule> r)

{
  int a;
  int nbAnt = r->GetNbAnt();

  if (NbRules != 0)
    Current = First;

  First = std::make_shared<Saved>();
  auto ru = std::make_shared<Rule>();

  for (a = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
    ru->Insert(r->GetVar(), r->GetVal(), r->GetRel());
  }

  First->OneRule = ru;

  if (NbRules != 0)
    First->Next = Current;

  NbRules++;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::Del()

{
  Current = First;

  for (int e = 0; e < NbRules; e++) {
    Current = Current->Next;
    (First->OneRule)->Del();
    First = Current;
  }
  NbRules = 0;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::RemCurrentRule() const

{
  std::shared_ptr<Rule> rule = GetRule();
  int nbAnt = rule->GetNbAnt();
  int a;

  for (a = 0, rule->GoToBeg(); a < nbAnt; a++, rule->GoToNext())
    rule->RemAnt();
}
////////////////////////////////////////////////////////////////////////

int RuleProcessing::TryEnlargedThres()

{
  int a;
  int t;
  int r;
  int var;
  int nbThresOneVar;
  int nbAnt;
  int nbCarried;
  int nbCarriedMod;
  int nbEnlarged;
  int indMax;
  int count;
  std::vector<int> vecCarried;
  float val;
  float thres;
  Rule oneCopy;
  std::shared_ptr<Rule> anotherCopy;
  std::shared_ptr<Rule> rule;
  std::vector<std::shared_ptr<Rule>> enlarged;

  OneVarThresDescr *oneVarDescr;
  std::shared_ptr<StringInt> carried;
  std::shared_ptr<StringInt> carriedMod;

  int nbRules = NbRules;

  for (r = 0, count = 0, GoToBeg(); r < nbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule))
      continue;

    nbAnt = rule->GetNbAnt();

    oneCopy.Copy(rule);

    enlarged.resize(NbHyp * NbVar * 2);
    vecCarried.resize(NbHyp * NbVar * 2);
    nbEnlarged = 0;

    carried = Data.Select(rule);
    nbCarried = carried->GetNbEl();

    a = 0;
    oneCopy.GoToBeg();
    while (a < nbAnt) {
      if (oneCopy.IsAntDeleted() == 1) {
        a++;
        oneCopy.GoToNext();
        continue;
      }

      var = oneCopy.GetVar();
      val = oneCopy.GetVal();

      oneVarDescr = Descr->GetDescr(var);

      nbThresOneVar = oneVarDescr->GetNbThres();

      if (nbThresOneVar > 1) {
        oneVarDescr->GoToBeg();

        for (t = 0; t < nbThresOneVar; t++, oneVarDescr->GoToNext()) {
          thres = oneVarDescr->GetThres();

          if (thres == val)
            continue;

          oneCopy.SetThres(thres);

          oneCopy.SavePtrAnt();
          carriedMod = Data.Select(std::make_shared<Rule>(oneCopy));
          oneCopy.PrevPtrAnt();

          nbCarriedMod = carriedMod->GetNbEl();

          if (nbCarriedMod > nbCarried && AreSameClass(carriedMod, ClassPatNet) == 1) {
            anotherCopy = std::make_shared<Rule>();

            oneCopy.SavePtrAnt();
            anotherCopy->Copy(std::make_shared<Rule>(oneCopy));
            oneCopy.PrevPtrAnt();

            vecCarried[nbEnlarged] = nbCarriedMod;
            enlarged[nbEnlarged] = anotherCopy;
            nbEnlarged++;
          }
          carriedMod->Del();

          oneCopy.SetThres(val);
        }
      }

      a++;
      oneCopy.GoToNext();
    }

    oneCopy.Del();
    carried->Del();

    if (nbEnlarged > 0) {
      RemCurrentRule();

      indMax = Max(vecCarried, nbEnlarged);
      rule = enlarged[indMax];

      Save();
      Insert(rule);
      Previous();

      for (t = 0; t < nbEnlarged; t++)
        enlarged[t]->Del();
    }

    count += nbEnlarged;
  }

  return count;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::EnlargeAndPrune()

{
  int found;

  do {
    std::cout << "Number of rules = " << CountRules() << std::endl;
    std::cout << "Number of antecedents = " << CountAnt() << std::endl;

    found = TryEnlargedThres();

    std::cout << "Number of possible extensions = " << found << "\n"
              << std::endl;

    RulePruneByMinCar();

  } while (found);
}

////////////////////////////////////////////////////////////////////////

RuleProcessing::RuleProcessing(
    int nbVar,
    int nbHyp,
    DataSet data,
    const std::vector<int> &classPatNet,
    std::shared_ptr<ThresDescr> descr) : NbVar(nbVar), NbHyp(nbHyp), Data(data), ClassPatNet(classPatNet), Descr(descr)

{
}

////////////////////////////////////////////////////////////////////////
