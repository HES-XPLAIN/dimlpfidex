using namespace std;
#include "rulePro.h"
#include "misc.h"
#include "randFun.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////

int RuleProcessing::Max(int *tab, int nbRules)

{
  int max, newVal;
  int e, indMax;

  if (nbRules == 0)
    return -1;

  if (nbRules == 1)
    return 0;

  max = tab[0];
  indMax = 0;

  for (e = 1; e < nbRules; e++) {
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

int RuleProcessing::Min(int *tab, int nbRules)

{
  int min, newVal;
  int e, indMin;

  if (nbRules == 0)
    return -1;

  if (nbRules == 1)
    return 0;

  min = 1111111111;

  for (e = 0; e < nbRules; e++) {
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

int RuleProcessing::IsRuleEmpty(Rule *rule)

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
  int a, r, nbAnt, count;
  Rule *rule;

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
  int r, count;

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
  Rule *rule;

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
  Rule *rule;
  StringInt *carried;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule) == 1)
      continue;

    carried = Current->Carried;
    carried->DelAll();
  }
}

////////////////////////////////////////////////////////////////////////

/*int RuleProcessing::CheckAllCarriedAndDisj(int toDrop)

{  int p, r, nbEl;

   int        nbEx          = Data.GetNbEx();
   int        areAllCarried = 1;
   int        areDisj       = 0;
   int*       checkTab      = new int[nbEx];
   Rule*      rule;
   StringInt* carriedEx;

   for (p=0; p<nbEx; p++)
       checkTab[p] = 0;

   for (r=0, GoToBeg(); r<NbRules; r++, GoToNext())
   {
       if (r == toDrop) continue;

       rule = GetRule();

       if (IsRuleEmpty(rule) == 1) continue;

       carriedEx = Current->Carried;

       if (AreSameClass(carriedEx, ClassPatNet) == 0)
       {
          delete checkTab;
          return 0;
       }

       nbEl = carriedEx->GetNbEl();

       for (p=0, carriedEx->GoToBeg(); p<nbEl; p++, carriedEx->GoToNext())
           checkTab[carriedEx->GetVal()] += 1;
   }

   for (p=0; p<nbEx; p++)
       if (checkTab[p] == 0) break;

   if (p != nbEx)
      areAllCarried = 0;

   if (areAllCarried == 1)
   {
      for (p=0; p<nbEx; p++)
          if (checkTab[p] > 1) break;

      if (p == nbEx)
         areDisj = 1;
   }

   delete checkTab;

   if ((areAllCarried == 0) && (areDisj == 0)) return 0;
   if ((areAllCarried == 1) && (areDisj == 0)) return 2;
   if ((areAllCarried == 1) && (areDisj == 1)) return 3;
}*/

////////////////////////////////////////////////////////////////////////

int RuleProcessing::CheckAllCarried(int toDrop)

{
  int p, r, nbEl;

  int nbEx = Data.GetNbEx();
  int *checkTab = new int[nbEx];
  Rule *rule;
  StringInt *carriedEx;

  for (p = 0; p < nbEx; p++)
    checkTab[p] = 0;

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
      delete checkTab;
      return 0;
    }

  delete checkTab;

  return 1;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::SetCountAntRules()

{
  int r;
  Rule *rule;

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
  Rule *rule;

  if (CheckAllCarried(indPrune) == 1) {
    for (r = 0, GoToBeg(); r < indPrune; r++, GoToNext())
      ;

    rule = GetRule();
    rule->DeleteRule();
    (Current->Carried)->DelAll();

    return 1;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::FastRulePrune(int nbIt)

{
  int count, i, indMax;

  TabRules = new int[NbRules];

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
  delete TabRules;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::RulePruneByMinCar()

{
  int count, r, indMin;
  StringInt *carried;

  TabRules = new int[NbRules];

  SetCountAntRules();
  SetCarriedField();

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext()) {
    if (TabRules[r] == 0)
      continue;

    carried = Data.Select(GetRule());
    TabRules[r] = carried->GetNbEl();
    carried->DelAll();
  }

  for (r = 0, count = 0; r < NbRules; r++) {
    indMin = Min(TabRules, NbRules);

    if (indMin < 0)
      break;

    TabRules[indMin] = 0;
    count += GoToSavedAndRemRule(indMin);
  }

  DelListCar();
  delete TabRules;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::GoToRuleAndRemAnt(int indPrune)

{
  int a, r, indRule, indAnt, remAnt;
  Rule *rule;
  StringInt *newCarried;

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

  newCarried->DelAll();
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::ComputeGain()

{
  int a, r, nbAnt, remAnt, diff;
  Rule *rule;
  StringInt *oldCarried;
  StringInt *newCarried;

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

      newCarried->DelAll();
    }

    oldCarried->DelAll();
  }
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::MixPrune()

{
  int i, indPrune, nbIt;

  ComputeGain();
  nbIt = Gain.GetNbEl();

  if (nbIt > 10)
    nbIt = nbIt / 10;

  Gain.Del();
  RuleInd.Del();
  AntInd.Del();

  for (;;) {
    cout << "\nNumber of rules = " << CountRules() << endl;
    cout << "Number of antecedents = " << CountAnt() << endl;

    ComputeGain();

    cout << "Number of possible antecedents to prune = ";
    cout << Gain.GetNbEl() << endl;

    if (Gain.GetNbEl() == 0)
      return;

    for (i = 0; i < nbIt; i++) {
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

void RuleProcessing::RemSevThres(Rule *r)

{
  int a, h, first;
  float min, max, currentVal;

  int *tabFreqLess = new int[NbVar];
  int *tabFreqGreat = new int[NbVar];

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
        if ((r->GetVar() == h) && (r->GetRel() == '>'))
          if (r->GetVal() != max)
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
        if ((r->GetVar() == h) && (r->GetRel() == '<'))
          if (r->GetVal() != min)
            r->RemAnt();
      }
    }
  }

  delete tabFreqLess;
  delete tabFreqGreat;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::Clean()

{
  int r;

  for (r = 0, GoToBeg(); r < NbRules; r++, GoToNext())
    RemSevThres(GetRule());
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::Insert(Rule *r)

{
  int a;
  int nbAnt = r->GetNbAnt();

  if (NbRules != 0)
    Current = First;

  First = new Saved;
  Rule *ru = new Rule;

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
  int e;

  Current = First;

  for (e = 0; e < NbRules; e++) {
    Current = Current->Next;
    (First->OneRule)->Del();
    delete First;
    First = Current;
  }

  NbRules = 0;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::RemCurrentRule()

{
  Rule *rule = GetRule();
  int nbAnt = rule->GetNbAnt();
  int a;

  for (a = 0, rule->GoToBeg(); a < nbAnt; a++, rule->GoToNext())
    rule->RemAnt();
}
////////////////////////////////////////////////////////////////////////

int RuleProcessing::TryEnlargedThres()

{
  int a, r, t, var, nbThresOneVar, nbAnt, nbCarried, nbCarriedMod;
  int nbEnlarged, indMax, count;
  int *vecCarried;
  float val, thres;
  Rule oneCopy;
  Rule *anotherCopy;
  Rule *rr;
  Rule *rule;
  Rule **enlarged;

  OneVarThresDescr *oneVarDescr;
  StringInt *carried;
  StringInt *carriedMod;

  int nbRules = NbRules;

  for (r = 0, count = 0, GoToBeg(); r < nbRules; r++, GoToNext()) {
    rule = GetRule();

    if (IsRuleEmpty(rule))
      continue;

    nbAnt = rule->GetNbAnt();

    rr = oneCopy.Copy(rule);

    enlarged = new Rule *[NbHyp * NbVar * 2];
    vecCarried = new int[NbHyp * NbVar * 2];
    nbEnlarged = 0;

    carried = Data.Select(rule);
    nbCarried = carried->GetNbEl();

    for (a = 0, oneCopy.GoToBeg(); a < nbAnt; a++, oneCopy.GoToNext()) {
      if (oneCopy.IsAntDeleted() == 1)
        continue;

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
          carriedMod = Data.Select(&oneCopy);
          oneCopy.PrevPtrAnt();

          nbCarriedMod = carriedMod->GetNbEl();

          if (nbCarriedMod > nbCarried) {
            if (AreSameClass(carriedMod, ClassPatNet) == 1) {
              anotherCopy = new Rule;

              oneCopy.SavePtrAnt();
              rr = anotherCopy->Copy(&oneCopy);
              oneCopy.PrevPtrAnt();

              vecCarried[nbEnlarged] = nbCarriedMod;
              enlarged[nbEnlarged] = anotherCopy;
              nbEnlarged++;
            }
          }

          carriedMod->DelAll();

          oneCopy.SetThres(val);
        }
      }
    }

    oneCopy.Del();
    carried->DelAll();

    if (nbEnlarged > 0) {
      RemCurrentRule();

      indMax = Max(vecCarried, nbEnlarged);
      rule = enlarged[indMax];

      Save();
      Insert(rule);
      Previous();

      for (t = 0; t < nbEnlarged; t++)
        enlarged[t]->DelAll();
    }

    delete enlarged;
    delete vecCarried;

    count += nbEnlarged;
  }

  return count;
}

////////////////////////////////////////////////////////////////////////

void RuleProcessing::EnlargeAndPrune()

{
  int found;

  do {
    cout << "Number of rules = " << CountRules() << endl;
    cout << "Number of antecedents = " << CountAnt() << endl;

    found = TryEnlargedThres();

    cout << "Number of possible extensions = " << found << "\n"
         << endl;

    RulePruneByMinCar();

  } while (found);
}

////////////////////////////////////////////////////////////////////////

RuleProcessing::RuleProcessing(
    int nbVar,
    int nbHyp,
    DataSet data,
    std::vector<int> classPatNet,
    ThresDescr *descr)

{
  NbRules = 0;
  NbVar = nbVar;
  NbHyp = nbHyp;

  Data = data;
  ClassPatNet = classPatNet;
  Descr = descr;
}

////////////////////////////////////////////////////////////////////////
