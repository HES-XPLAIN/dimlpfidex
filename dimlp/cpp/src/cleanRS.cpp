#include "cleanRS.h"
#include <stdlib.h>
#include <string.h>

using namespace std;

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ResetFlag() const

{
  for (int r = 0; r < NbRules; r++)
    Clean[r]->Flag = 0;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetFlagToOne() const

{
  for (int r = 0; r < NbRules; r++)
    Clean[r]->Flag = 1;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::CountFlaggedRules(int flag) const

{
  int r;
  int countRules;

  for (r = 0, countRules = 0; r < NbRules; r++)
    if (Clean[r]->Flag == flag)
      countRules++;

  return countRules;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::CountFlaggedAnt(int flag) const

{
  int r;
  int countAnt;

  for (r = 0, countAnt = 0; r < NbRules; r++)
    if (Clean[r]->Flag == flag)
      countAnt += Clean[r]->NbAnt;

  return countAnt;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::CountCarPatByFlags(int flag) const

{
  int r;
  int countPat;

  for (r = 0, countPat = 0; r < NbRules; r++)
    if (Clean[r]->Flag == flag)
      countPat += Clean[r]->NbAllCarried;

  return countPat;
}

////////////////////////////////////////////////////////////////////////

float CleanRuleStruct::ComputeAvgCar(int flag) const

{
  return (float)CountCarPatByFlags(flag) /
         (float)CountFlaggedRules(flag);
}

////////////////////////////////////////////////////////////////////////

float CleanRuleStruct::ComputeAvgAnt(int flag) const

{
  return (float)CountFlaggedAnt(flag) /
         (float)CountFlaggedRules(flag);
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::FindMinOnAnt(const AssocAnte *ant, int nbAnt) const

{
  int min;
  int indMin;

  indMin = 0;
  min = ant[0].Var;

  for (int a = 1; a < nbAnt; a++) {
    if (ant[a].Var <= min) {
      if (ant[a].Var < min) {
        min = ant[a].Var;
        indMin = a;
      }

      else // if (ant[a].Var == min)
      {
        if (ant[a].Rel == '>') {
          min = ant[a].Var;
          indMin = a;
        }
      }
    }
  }

  return indMin;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SortAnt(AssocAnte *ant, int nbAnt, int indRule) const

{
  int indMin;

  std::vector<AssocAnte> sorted(nbAnt);

  for (int a = 0; a < nbAnt; a++) {
    indMin = FindMinOnAnt(ant, nbAnt);

    sorted[a].Var = ant[indMin].Var;
    sorted[a].Val = ant[indMin].Val;
    sorted[a].Rel = ant[indMin].Rel;

    ant[indMin].Var = 2000000000;
  }

  Clean[indRule]->SevAnt = sorted;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::FindMinAntOnRules(int start) const

{
  int max;
  int indMax;

  max = 1111111111;
  indMax = -1;

  for (int r = start; r < NbRules; r++) {
    if (Clean[r]->Flag == 0 && Clean[r]->NbAnt < max) {
      max = Clean[r]->NbAnt;
      indMax = r;
    }
  }

  return indMax;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::FindMaxOnRules(int start) const

{
  int max;
  int indMax;

  max = -1;
  indMax = -1;

  for (int r = start; r < NbRules; r++) {
    if (Clean[r]->Flag == 0 && Clean[r]->NbAllCarried > max) {
      max = Clean[r]->NbAllCarried;
      indMax = r;
    }
  }

  return indMax;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SortRules(int minAnt)

{
  int indMax;
  std::shared_ptr<CleanRule> ptrClean;

  ResetFlag();

  for (int r = 0; r < NbRules - 1; r++) {
    if (minAnt == 0)
      indMax = FindMaxOnRules(r);
    else
      indMax = FindMinAntOnRules(r);

    if (indMax == -1)
      break;

    ptrClean = Clean[r];
    Clean[r] = Clean[indMax];
    Clean[indMax] = ptrClean;
  }
}

////////////////////////////////////////////////////////////////////////

// does not work with contradictions

float CleanRuleStruct::GlobalAcc(DataSet &data, int *vecWrong, int nbEl) const

{
  int e;
  int count;
  int cl;
  int clNet;

  for (e = 0, count = 0; e < nbEl; e++) {
    if (vecWrong[e] == -1) // -1 -> Non Carried
    {
      cout << "Warning ***\n\n";

      Bpnn->ForwardOneExample1(data, e);
      clNet = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
      cl = DefaultClass;

      if (cl == clNet)
        vecWrong[e] = 0;
    }

    if (vecWrong[e] == 0)
      count++;
  }

  return (float)count / (float)nbEl;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ComputeAcc(
    std::shared_ptr<StringInt> carried,
    int *indWrong,
    DataSet &data,
    DataSet &dataClass,
    int *nbPat,
    int *correct,
    int *wrong,
    float *acc) const

{
  int p;
  int cl;
  int clNet;
  int indPat;

  *nbPat = carried->GetNbEl();
  *correct = 0;
  *wrong = 0;

  for (p = 0, carried->GoToBeg(); p < *nbPat; p++, carried->GoToNext()) {
    indPat = carried->GetVal();

    Bpnn->ForwardOneExample1(data, indPat);
    clNet = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
    cl = Bpnn->Max(std::vector<float>(dataClass.GetExample(indPat), dataClass.GetExample(indPat) + NbOut));

    if (cl == clNet) {
      *correct += 1;
      indWrong[indPat] = 0;
    }

    else {
      *wrong += 1;
      indWrong[indPat] = 1;
    }
  }

  if (*nbPat != 0)
    *acc = (float)*correct / (float)*nbPat;
  else
    *acc = -1;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetSevInfo(std::shared_ptr<Rule> rule, int indClean)

{
  std::shared_ptr<StringInt> carried;

  carried = All.Select(rule);
  carried->GoToBeg();
  Bpnn->ForwardOneExample1(All, carried->GetVal());

  Clean[indClean]->Flag = 0;
  Clean[indClean]->NbAllCarried = carried->GetNbEl();
  Clean[indClean]->Classification = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  carried->Del();

  carried = Train.Select(rule);

  ComputeAcc(carried, WrongTrain.data(), Train, TrainClass,
             &(Clean[indClean]->NbCarriedTrain),
             &(Clean[indClean]->NbCorrectTrain),
             &(Clean[indClean]->NbWrongTrain),
             &(Clean[indClean]->AccuracyTrain));

  carried->Del();

  if (Test.GetNbEx() != 0) {
    carried = Test.Select(rule);

    ComputeAcc(carried, WrongTest.data(), Test, TestClass,
               &(Clean[indClean]->NbCarriedTest),
               &(Clean[indClean]->NbCorrectTest),
               &(Clean[indClean]->NbWrongTest),
               &(Clean[indClean]->AccuracyTest));

    carried->Del();
  }

  else {
    Clean[indClean]->NbCarriedTest = 0;
    Clean[indClean]->NbCorrectTest = 0;
    Clean[indClean]->NbWrongTest = 0;
    Clean[indClean]->AccuracyTest = 0.0;
  }

  if (Valid.GetNbEx() != 0) {
    carried = Valid.Select(rule);

    ComputeAcc(carried, WrongValid.data(), Valid, ValidClass,
               &(Clean[indClean]->NbCarriedValid),
               &(Clean[indClean]->NbCorrectValid),
               &(Clean[indClean]->NbWrongValid),
               &(Clean[indClean]->AccuracyValid));

    carried->Del();
  }

  else {
    Clean[indClean]->NbCarriedValid = 0;
    Clean[indClean]->NbCorrectValid = 0;
    Clean[indClean]->NbWrongValid = 0;
    Clean[indClean]->AccuracyValid = 0.0;
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::CreateStructures() {
  const int nbPrunedRules = Pruned->GetNbRules();

  int a;
  int r;
  int allAnt;
  std::shared_ptr<Rule> rule;

  Clean.resize(NbRules + 1);

  Clean[NbRules] = std::make_shared<CleanRule>(); // for Def
  Clean[NbRules]->Classification = -1;

  int countEffectRules = 0;
  int countEffectAnt;

  for (r = 0, Pruned->GoToBeg(); r < nbPrunedRules; r++, Pruned->GoToNext()) {
    rule = Pruned->GetRule();

    if (Pruned->IsRuleEmpty(rule))
      continue;

    allAnt = rule->GetNbAnt();
    countEffectAnt = rule->GetNbAntWithout();

    Clean[countEffectRules] = std::make_shared<CleanRule>();
    Clean[countEffectRules]->SevAnt.resize(countEffectAnt);
    Clean[countEffectRules]->NbAnt = countEffectAnt;

    int indexCleanAnt = 0;

    for (a = 0, rule->GoToBeg(); a < allAnt; a++, rule->GoToNext()) {
      if (rule->IsAntDeleted() == 1)
        continue;

      Clean[countEffectRules]->SevAnt[indexCleanAnt].Var = rule->GetVar();
      Clean[countEffectRules]->SevAnt[indexCleanAnt].Val = rule->GetVal();
      Clean[countEffectRules]->SevAnt[indexCleanAnt].Rel = rule->GetRel();

      indexCleanAnt++;
    }

    SortAnt(&Clean[countEffectRules]->SevAnt[0], Clean[countEffectRules]->NbAnt, countEffectRules);

    SetSevInfo(rule, countEffectRules);

    countEffectRules++;
  }

  SortRules(0);
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::WriteRules(int def, ostream &ruleFile)

{
  int r;
  int count;
  int nbRules;

  const AssocAnte *ptrAnt;

  if (def == 1) {
    SetFlagToOne();
    Clean[NbRules]->Flag = 1;
    nbRules = NbRules + 1;
  }

  else {
    SetFlagToOne();
    nbRules = NbRules;
  }

  ruleFile << "\n\n---------------------------------------------------------\n\n";

  for (r = 0, count = 0; r < NbRules; r++) {
    if (Clean[r]->Flag != 1)
      continue;

    count++;
    ruleFile << "Rule " << count << ": ";

    ptrAnt = const_cast<const AssocAnte *>(Clean[r]->SevAnt.data());

    for (int a = 0; a < Clean[r]->NbAnt; a++, ptrAnt++) {
      ruleFile << "(" << ptrAnt->Str << " ";
      ruleFile << ptrAnt->Rel << " ";
      ruleFile << ptrAnt->Val << ") ";
    }

    ruleFile << "Class = " << Clean[r]->StrClass;
    ruleFile << " (" << Clean[r]->NbAllCarried << ")";

    ruleFile << "\n\n";
  }

  if (def == 1) {
    ruleFile << "Default Rule: "
             << "Class " << Clean[NbRules]->StrClass;
  }
  if (def == 1) {
    ruleFile << " (" << Clean[NbRules]->NbAllCarried << ")\n\n";
  }

  ruleFile << "---------------------------------------------------------\n\n";

  ruleFile << "Training set:\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  for (r = 0, count = 0; r < nbRules; r++) {
    if (Clean[r]->Flag != 1)
      continue;

    if (r != NbRules) {
      ruleFile << "Rule ";
      ruleFile.width(4);
      count++;
      ruleFile << count << ": ";
    } else {
      ruleFile << "Default  : ";
    }

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCarriedTrain << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCorrectTrain << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbWrongTrain << " ";

    if (Clean[r]->AccuracyTrain != -1.0) {
      ruleFile.width(12);
      ruleFile << Clean[r]->AccuracyTrain << " ";
    } else {
      ruleFile << "              ";
    }

    ruleFile << "      Class = " << Clean[r]->StrClass;

    ruleFile << "\n";
  }

  ruleFile << "\n---------------------------------------------------------\n\n";

  ruleFile << "Accuracy of rules on training set = ";
  ruleFile << GlobalAcc(Train, WrongTrain.data(), Train.GetNbEx()) << "\n\n";

  if (Valid.GetNbEx() != 0) {
    ruleFile << "---------------------------------------------------------\n\n";

    ruleFile << "Validation set:\n\n";

    ruleFile << "---------------------------------------------------------\n\n";

    for (r = 0, count = 0; r < nbRules; r++) {
      if (Clean[r]->Flag != 1)
        continue;

      if (r != NbRules) {
        ruleFile << "Rule ";
        ruleFile.width(4);
        count++;
        ruleFile << count << ": ";
      } else {
        ruleFile << "Default  :";
      }

      ruleFile.width(5);
      ruleFile << Clean[r]->NbCarriedValid << " ";

      ruleFile.width(5);
      ruleFile << Clean[r]->NbCorrectValid << " ";

      ruleFile.width(5);
      ruleFile << Clean[r]->NbWrongValid << " ";

      if (Clean[r]->AccuracyValid != -1.0) {
        ruleFile.width(12);
        ruleFile << Clean[r]->AccuracyValid << " ";
      } else {
        ruleFile << "              ";
      }

      ruleFile << "      Class = " << Clean[r]->StrClass;
      ruleFile << "\n";
    }

    ruleFile << "\n";
    ruleFile << "---------------------------------------------------------\n\n";

    ruleFile << "Accuracy of rules on validation set = ";
    ruleFile << GlobalAcc(Valid, WrongValid.data(), Valid.GetNbEx()) << "\n\n";
  }

  ruleFile << "---------------------------------------------------------\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  ruleFile << "--- Number of rules = " << CountFlaggedRules(1) << "\n";
  ruleFile << "--- Number of antecedents = " << CountFlaggedAnt(1) << "\n";
  ruleFile << "--- Number of antecedents per rule = ";
  ruleFile << ComputeAvgAnt(1) << "\n";
  ruleFile << "--- Number of examples per rule = ";
  ruleFile << ComputeAvgCar(1) << "\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  if (def == 0 && Test.GetNbEx() != 0)
    UnordAccWithDef2(ruleFile);
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::CreateWrongVect()

{
  int p;

  WrongTrain.resize(Train.GetNbEx());

  for (p = 0; p < Train.GetNbEx(); p++)
    WrongTrain[p] = -1;

  if (Test.GetNbEx() != 0) {
    WrongTest.resize(Test.GetNbEx());

    for (p = 0; p < Test.GetNbEx(); p++)
      WrongTest[p] = -1;
  } else
    WrongTest.clear();

  if (Valid.GetNbEx() != 0) {
    WrongValid.resize(Valid.GetNbEx());

    for (p = 0; p < Valid.GetNbEx(); p++)
      WrongValid[p] = -1;
  } else
    WrongValid.clear();
}

////////////////////////////////////////////////////////////////////////

CleanRuleStruct::CleanRuleStruct(
    const DataSet &all,
    const DataSet &train,
    const DataSet &trainClass,
    const DataSet &valid,
    const DataSet &validClass,
    const DataSet &test,
    const DataSet &testClass,
    std::shared_ptr<RuleProcessing> processed,
    std::shared_ptr<BpNN> bpNn,
    float *out,
    int nbOut) : All(all), Train(train), TrainClass(trainClass), Test(test), TestClass(testClass), Valid(valid), ValidClass(validClass), NbOut(nbOut), Bpnn(bpNn), Pruned(processed), Out(out)

{
  NbRules = Pruned->CountRules();

  CreateWrongVect();
  CreateStructures();

  const int nbEx = All.GetNbEx();
  ClAllNet.resize(nbEx);

  for (int p = 0; p < nbEx; p++) {
    Bpnn->ForwardOneExample1(All, p);
    ClAllNet[p] = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  }
  SetClassPatNet();
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ResetSomeFields() const

{
  for (int r = 0; r < NbRules + 1; r++) {
    Clean[r]->NbAllCarried = 0;
    Clean[r]->NbCarriedTrain = 0;
    Clean[r]->NbCarriedTest = 0;
    Clean[r]->NbCarriedValid = 0;
    Clean[r]->NbCorrectTrain = 0;
    Clean[r]->NbCorrectTest = 0;
    Clean[r]->NbCorrectValid = 0;
    Clean[r]->NbWrongTrain = 0;
    Clean[r]->NbWrongTest = 0;
    Clean[r]->NbWrongValid = 0;
    Clean[r]->AccuracyTrain = 0;
    Clean[r]->AccuracyTest = 0;
    Clean[r]->AccuracyValid = 0;
  }
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::IsExampleCarried(
    DataSet &data,
    int index,
    std::shared_ptr<CleanRule> rule) const

{
  int a;
  int var;
  int isRuleEmpty = 1;
  float val;
  char rel;

  const AssocAnte *ptrAnt;

  const int nbRuleAnt = rule->NbAnt;
  const float *ptrPat = data.GetExample(index);

  for (a = 0, ptrAnt = const_cast<const AssocAnte *>(rule->SevAnt.data()); a < nbRuleAnt; a++, ptrAnt++) {
    if (IsAntDeleted(ptrAnt))
      continue;

    isRuleEmpty = 0;

    var = ptrAnt->Var;
    val = ptrAnt->Val;
    rel = ptrAnt->Rel;

    switch (rel) {
    case '<':
      if (*(ptrPat + var) < val)
        continue;
      else
        return 0;

    case '>':
      if (*(ptrPat + var) > val)
        continue;
      else
        return 0;

    default:
      break;
    }
  }

  if (isRuleEmpty == 1)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ElseRepresentation()

{
  ResetSomeFields();

  ElseRepAll();
  ElseRepTrain();
  ElseRepTest();
  ElseRepValid();
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::Fidelity100()

{
  int r;
  int nbEx;

  nbEx = All.GetNbEx();

  for (r = 0; r < NbRules + 1; r++)
    Clean[r]->NbAllCarried = 0;

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (IsExampleCarried(All, p, Clean[r]) == 1) {
        Clean[r]->NbAllCarried += 1;

        if (Clean[r]->Classification == ClAllNet[p])
          break;
        else
          return 0;
      }
    }

    if (r == NbRules) {
      if (Clean[NbRules]->Classification == ClAllNet[p])
        Clean[NbRules]->NbAllCarried += 1;
      else
        return 0;
    }
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::Fidelity100Def()

{
  int r;
  int nbEx;

  nbEx = All.GetNbEx();

  for (r = 0; r < NbRules; r++)
    Clean[r]->NbAllCarried = 0;

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (Clean[r]->Flag == 0)
        continue;

      if (IsExampleCarried(All, p, Clean[r]) == 1) {
        Clean[r]->NbAllCarried += 1;

        if (Clean[r]->Classification == ClAllNet[p])
          break;
        else
          return 0;
      }
    }
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ElseRepAll()

{
  int r;
  int nbEx;

  nbEx = All.GetNbEx();

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (IsExampleCarried(All, p, Clean[r]) == 1) {
        Clean[r]->NbAllCarried += 1;
        break;
      }
    }

    if (r == NbRules)
      Clean[NbRules]->NbAllCarried += 1;
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ElseRepTrain()

{
  int r;
  int nbEx;

  nbEx = Train.GetNbEx();

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (IsExampleCarried(Train, p, Clean[r]) == 1) {
        Clean[r]->NbCarriedTrain += 1;

        if (Clean[r]->Classification == ClassPatNetTrain[p])
          Clean[r]->NbCorrectTrain += 1;
        else
          Clean[r]->NbWrongTrain += 1;

        break;
      }
    }

    if (r == NbRules) {
      Clean[NbRules]->NbCarriedTrain += 1;

      if (Clean[NbRules]->Classification == ClassPatNetTrain[p])
        Clean[NbRules]->NbCorrectTrain += 1;
      else
        Clean[NbRules]->NbWrongTrain += 1;
    }
  }

  for (r = 0; r < NbRules + 1; r++) {
    if (Clean[r]->NbCarriedTrain == 0)
      Clean[r]->AccuracyTrain = -1;
    else
      Clean[r]->AccuracyTrain = (float)Clean[r]->NbCorrectTrain /
                                (float)Clean[r]->NbCarriedTrain;
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ElseRepTest()

{
  int r;
  int nbEx;

  nbEx = Test.GetNbEx();

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (IsExampleCarried(Test, p, Clean[r]) == 1) {
        Clean[r]->NbCarriedTest += 1;

        if (Clean[r]->Classification == ClassPatNetTest[p])
          Clean[r]->NbCorrectTest += 1;
        else
          Clean[r]->NbWrongTest += 1;

        break;
      }
    }

    if (r == NbRules) {
      Clean[NbRules]->NbCarriedTest += 1;

      if (Clean[NbRules]->Classification == ClassPatNetTest[p])
        Clean[NbRules]->NbCorrectTest += 1;
      else
        Clean[NbRules]->NbWrongTest += 1;
    }
  }

  for (r = 0; r < NbRules + 1; r++) {
    if (Clean[r]->NbCarriedTest == 0)
      Clean[r]->AccuracyTest = -1;
    else
      Clean[r]->AccuracyTest = (float)Clean[r]->NbCorrectTest /
                               (float)Clean[r]->NbCarriedTest;
  }

  int sumCov = 0;
  int sumCor = 0;

  for (r = 0; r < NbRules + 1; r++) {
    sumCov += Clean[r]->NbCarriedTest;
    sumCor += Clean[r]->NbCorrectTest;
  }

  float trueAcc = (float)sumCor / (float)sumCov;

  cout << "VERIFYING: " << trueAcc << "(" << sumCor << " " << sumCov << ")";
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::ElseRepValid()

{
  int r;
  int nbEx;

  nbEx = Valid.GetNbEx();

  for (int p = 0; p < nbEx; p++) {
    for (r = 0; r < NbRules; r++) {
      if (IsExampleCarried(Valid, p, Clean[r]) == 1) {
        Clean[r]->NbCarriedValid += 1;

        if (Clean[r]->Classification == ClassPatNetValid[p])
          Clean[r]->NbCorrectValid += 1;
        else
          Clean[r]->NbWrongValid += 1;

        break;
      }
    }

    if (r == NbRules) {
      Clean[NbRules]->NbCarriedValid += 1;

      if (Clean[NbRules]->Classification == ClassPatNetValid[p])
        Clean[NbRules]->NbCorrectValid += 1;
      else
        Clean[NbRules]->NbWrongValid += 1;
    }
  }

  for (r = 0; r < NbRules + 1; r++) {
    if (Clean[r]->NbCarriedValid == 0)
      Clean[r]->AccuracyValid = -1;
    else
      Clean[r]->AccuracyValid = (float)Clean[r]->NbCorrectValid /
                                (float)Clean[r]->NbCarriedValid;
  }
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::RandomPruneAnt()

{
  const int nbAllAnt = CountFlaggedAnt(1);
  int countFail = 0;
  int countPrune = 0;
  int a;
  int r;
  int nbAnt;
  int toDelVar;
  int pruned;

  AssocAnte *ant;

  IntRandomFunction randInt(0, NbRules - 1);

  ResetSomeFields();
  ElseRepAll();

  do {
    r = randInt.RandomInteger();
    a = randInt.RandomInteger() % Clean[r]->NbAnt;

    do {
      do {
        nbAnt = Clean[r]->NbAnt;
        ant = Clean[r]->SevAnt.data();
        ant = ant + a;

        if (IsAntDeleted(ant) == 1) {
          countFail++;

          if (countFail == nbAllAnt)
            return countPrune;

          a++;

          if (a == nbAnt) {
            r++;
            r = r % NbRules;
            a = 0;
          }
        }

      } while (IsAntDeleted(ant) == 1);

      toDelVar = ant->Var;
      RemAnt(ant);

      pruned = Fidelity100();

      if (pruned == 0) {
        countFail++;
        ant->Var = toDelVar;

        if (countFail == nbAllAnt)
          return countPrune;

        a++;

        if (a == nbAnt) {
          r++;
          r = r % NbRules;
          a = 0;
        }

      } else {
        countFail = 0;
        countPrune++;
      }

    } while (pruned == 0);

  } while (countFail < nbAllAnt);

  return countPrune;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::RemRule(int r) const

{
  const int nbAnt = Clean[r]->NbAnt;
  AssocAnte *ptrAnt;
  int a;

  for (a = 0, ptrAnt = Clean[r]->SevAnt.data(); a < nbAnt; a++, ptrAnt++)
    RemAnt(ptrAnt);
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::OrderedExpand(ThresDescr *descr)

{
  int countExpand = 0;
  int a;
  int r;
  int s;
  int nbAnt;
  float toExpVal;
  float thres;
  int expanded;
  int nbThresOneVar;
  int prevCarried;

  OneVarThresDescr *oneVarDescr;
  AssocAnte *ant;

  std::vector<int> vectCar(NbRules);

  Fidelity100();

  for (r = 0; r < NbRules; r++)
    vectCar[r] = Clean[r]->NbAllCarried;

  for (r = 0; r < NbRules; r++) {
    nbAnt = Clean[r]->NbAnt;

    for (a = 0, ant = Clean[r]->SevAnt.data(); a < nbAnt; a++, ant++) {
      if (IsAntDeleted(ant) == 1)
        continue;

      toExpVal = ant->Val;
      prevCarried = Clean[r]->NbAllCarried;

      if (prevCarried == 0)
        continue;

      oneVarDescr = descr->GetDescr(ant->Var);
      nbThresOneVar = oneVarDescr->GetNbThres();
      expanded = 0;

      if (nbThresOneVar > 1) {
        oneVarDescr->GoToBeg();

        for (int t = 0; t < nbThresOneVar; t++, oneVarDescr->GoToNext()) {
          thres = oneVarDescr->GetThres();

          if (thres == toExpVal)
            continue;

          ant->Val = thres;
          expanded = Fidelity100();

          if (expanded) {
            if (Clean[r]->NbAllCarried > prevCarried) {
              // cout << prevCarried << " " << Clean[r]->NbAllCarried << "\n";

              // prevCarried = Clean[r]->NbAllCarried;

              break;
            } else {
              expanded = 0;
              Clean[r]->NbAllCarried = prevCarried;

              // prevCarried = Clean[r]->NbAllCarried;
            }
          }
        }
      }

      if (expanded == 0) {
        ant->Val = toExpVal;

        for (s = 0; s < NbRules; s++)
          Clean[s]->NbAllCarried = vectCar[s];
      }

      else {

        for (s = 0; s < NbRules; s++)
          vectCar[s] = Clean[s]->NbAllCarried;

        countExpand++;
      }
    }
  }

  return countExpand;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::CheckAnt(int indOld) const

{
  const int oldNbAnt = Clean[indOld]->NbAnt;
  int countOldAnt = 0;
  int i;
  const AssocAnte *ptrOldAnt;

  for (i = 0, ptrOldAnt = const_cast<const AssocAnte *>(Clean[indOld]->SevAnt.data()); i < oldNbAnt; i++, ptrOldAnt++) {
    if (IsAntDeleted(ptrOldAnt) == 0)
      countOldAnt++;
  }

  return countOldAnt;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::CreateNewClean()

{
  int n;
  int r;
  int nbOldAnt;
  int nbNewAnt;
  const AssocAnte *ptrOldAnt;
  AssocAnte *ptrNewAnt;

  for (r = 0, n = 0; r < NbRules; r++)
    if (CheckAnt(r))
      n++;

  std::vector<std::shared_ptr<CleanRule>> newClean(n + 1);
  newClean[n] = std::make_shared<CleanRule>();

  newClean[n]->Classification = Clean[NbRules]->Classification;

  for (r = 0, n = 0; r < NbRules; r++) {
    nbOldAnt = Clean[r]->NbAnt;
    nbNewAnt = CheckAnt(r);

    if (nbNewAnt > 0) {
      newClean[n] = std::make_shared<CleanRule>();
      newClean[n]->SevAnt.resize(nbNewAnt);
      newClean[n]->NbAnt = nbNewAnt;
      newClean[n]->Classification = Clean[r]->Classification;

      newClean[n]->Flag = 1;
      newClean[n]->NbAllCarried = Clean[r]->NbAllCarried;
      newClean[n]->NbCarriedTrain = Clean[r]->NbCarriedTrain;
      newClean[n]->NbCarriedTest = Clean[r]->NbCarriedTest;
      newClean[n]->NbCarriedValid = Clean[r]->NbCarriedValid;
      newClean[n]->NbCorrectTrain = Clean[r]->NbCorrectTrain;
      newClean[n]->NbCorrectTest = Clean[r]->NbCorrectTest;
      newClean[n]->NbCorrectValid = Clean[r]->NbCorrectValid;
      newClean[n]->NbWrongTrain = Clean[r]->NbWrongTrain;
      newClean[n]->NbWrongTest = Clean[r]->NbWrongTest;
      newClean[n]->NbWrongValid = Clean[r]->NbWrongValid;
      newClean[n]->AccuracyTrain = Clean[r]->AccuracyTrain;
      newClean[n]->AccuracyTest = Clean[r]->AccuracyTest;
      newClean[n]->AccuracyValid = Clean[r]->AccuracyValid;

      ptrOldAnt = const_cast<const AssocAnte *>(Clean[r]->SevAnt.data());
      ptrNewAnt = newClean[n]->SevAnt.data();

      for (int a = 0; a < nbOldAnt; a++, ptrOldAnt++) {
        if (IsAntDeleted(ptrOldAnt) == 0) {
          ptrNewAnt->Var = ptrOldAnt->Var;
          ptrNewAnt->Val = ptrOldAnt->Val;
          ptrNewAnt->Rel = ptrOldAnt->Rel;

          ptrNewAnt++;
        }
      }

      n++;
    }
  }

  Clean = newClean;

  NbRules = n;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::SumCarried() const

{
  int r;
  int count;

  for (r = 0, count = 0; r < NbRules + 1; r++)
    count += Clean[r]->NbAllCarried;

  return count;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::PruneRule()

{
  int a;
  int nbAnt;
  int toRet = 0;
  std::vector<int> saved;
  AssocAnte *ptrAnt;

  Fidelity100();
  // cout << SumCarried() << " -- ";

  for (int r = 0; r < NbRules; r++) {
    if (CheckAnt(r) == 0)
      continue;

    nbAnt = Clean[r]->NbAnt;

    saved.resize(nbAnt);

    for (a = 0, ptrAnt = Clean[r]->SevAnt.data(); a < nbAnt; a++, ptrAnt++)
      saved[a] = ptrAnt->Var;

    RemRule(r);

    if ((Fidelity100() != 1) || (SumCarried() != All.GetNbEx())) {
      for (a = 0, ptrAnt = Clean[r]->SevAnt.data(); a < nbAnt; a++, ptrAnt++)
        ptrAnt->Var = saved[a];
    }

    else {
      toRet += 1;
    }
  }

  return toRet;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::WriteNumb() const

{
  int r;
  int n;

  for (r = 0, n = 0; r < NbRules; r++)
    if (CheckAnt(r))
      n++;

  cout << "\nNumber of rules = " << n;

  for (r = 0, n = 0; r < NbRules; r++)
    n += CheckAnt(r);

  cout << "\nNumber of antecedents = " << n << "\n";
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SimplifyElse(ThresDescr *descr)

{
  int rpa;
  int pr;
  int oe;
  int pr2;

  SortRules(0);
  SetFlagToOne();

  do {
    WriteNumb();

    rpa = RandomPruneAnt();
    pr = PruneRule();
    oe = OrderedExpand(descr);
    pr2 = PruneRule();

  } while ((rpa != 0) || (pr != 0) || (oe != 0) || (pr2 != 0));

  Fidelity100();

  CreateNewClean();

  DefaultClass = SetDefRule();

  CreateNewClean();

  Clean[NbRules]->Classification = DefaultClass;

  do {
    rpa = RandomPruneAnt();

  } while (rpa != 0);

  WriteNumb();

  CreateNewClean();
  Clean[NbRules]->Classification = DefaultClass;
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::SetDefRule()

{
  int r;
  int cl;
  int nbDelAnt;
  int defClass;
  int countAnt;
  std::vector<std::shared_ptr<StringInt>> vectList;
  std::vector<int> nbDeletedAnt(NbOut);

  ElseRepresentation();

  vectList.resize(NbOut);

  for (cl = 0; cl < NbOut; cl++)
    vectList[cl] = std::make_shared<StringInt>();

  for (cl = 0; cl < NbOut; cl++) {
    SetFlagToOne();

    for (r = NbRules - 1, countAnt = 0; r >= 0; r--) {
      if (Clean[r]->Classification == cl) {
        Clean[r]->Flag = 0;

        if (Fidelity100Def() == 0)
          Clean[r]->Flag = 1;
        else {
          vectList[cl]->Insert(r);
          countAnt += Clean[r]->NbAnt;
        }
      }
    }

    nbDeletedAnt[cl] = countAnt;
  }

  defClass = 0;
  nbDelAnt = nbDeletedAnt[0];

  for (cl = 1; cl < NbOut; cl++) {
    if (nbDeletedAnt[cl] > nbDelAnt) {
      defClass = cl;
      nbDelAnt = nbDeletedAnt[cl];
    }
  }

  const int nbDef = vectList[defClass]->GetNbEl();

  vectList[defClass]->GoToBeg();

  for (r = 0; r < nbDef; r++, vectList[defClass]->GoToNext())
    RemRule(vectList[defClass]->GetVal());

  for (cl = 0; cl < NbOut; cl++)
    vectList[cl]->Del();

  return defClass;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetClassPatNet()

{
  int p;

  const int nbTrain = TrainClass.GetNbEx();
  const int nbTest = TestClass.GetNbEx();
  const int nbValid = ValidClass.GetNbEx();

  ClassPatNetTrain.resize(nbTrain);
  for (p = 0; p < nbTrain; p++)
    ClassPatNetTrain[p] = Bpnn->Max(std::vector<float>(TrainClass.GetExample(p), TrainClass.GetExample(p) + NbOut));
  if (nbTest > 0) {
    ClassPatNetTest.resize(nbTest);
    for (p = 0; p < nbTest; p++)
      ClassPatNetTest[p] = Bpnn->Max(std::vector<float>(TestClass.GetExample(p), TestClass.GetExample(p) + NbOut));
  }
  if (nbValid > 0) {
    ClassPatNetValid.resize(nbValid);
    for (p = 0; p < nbValid; p++)
      ClassPatNetValid[p] = Bpnn->Max(std::vector<float>(ValidClass.GetExample(p), ValidClass.GetExample(p) + NbOut));
  }
}

////////////////////////////////////////////////////////////////////////

std::string CleanRuleStruct::ItoA(int n) const {
  int div;
  int mod;
  int i;
  std::string str;
  std::string strRev;

  i = 0;
  do {
    div = n / 10;
    mod = n % 10;

    str[i] = static_cast<char>(mod + '0');

    i++;
    n = div;

  } while (div != 0);

  for (int k = 0; k < i; k++)
    strRev += str[i - 1 - k];

  return strRev;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetAttr(vector<string> listAttr) const

{
  int a;
  int nbAnt;
  AssocAnte *ptr;

  for (int r = 0; r < NbRules; r++) {
    nbAnt = Clean[r]->NbAnt;

    for (a = 0, ptr = Clean[r]->SevAnt.data(); a < nbAnt; a++, ptr++) {
      ptr->Str = listAttr[ptr->Var];
    }
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetAttr() {
  int a;
  int nbAnt;
  AssocAnte *ptr;

  for (int r = 0; r < NbRules; r++) {
    nbAnt = Clean[r]->NbAnt;
    for (a = 0, ptr = Clean[r]->SevAnt.data(); a < nbAnt; a++, ptr++) {
      std::string varStr = ItoA(ptr->Var + 1);
      std::string str = "x" + varStr;
      ptr->Str = str;
    }
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetStrClass(vector<string> listClass, int def) const

{
  for (int r = 0; r < NbRules + def; r++) {
    Clean[r]->StrClass = listClass[Clean[r]->Classification];
  }
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::SetStrClass(int def) {
  for (int r = 0; r < NbRules + def; r++) {
    std::string str = ItoA(Clean[r]->Classification + 1);
    Clean[r]->StrClass = str;
  }
}

////////////////////////////////////////////////////////////////////////

int CleanRuleStruct::DefDef() const

{
  int r;
  int def;
  int count;
  int i;

  if (NbRules == 0)
    return 0;

  def = Clean[NbRules - 1]->Classification;

  for (r = NbRules - 2; r >= 0; r--) {
    if (Clean[r]->Classification != def)
      break;
  }

  r++;

  for (i = 0, count = 0; i < r; i++)
    count += Clean[i]->NbAnt;

  cout << "\nUnordered rules = " << NbRules << "\n";
  cout << "Unordered rules - default rule = " << r << "\n";
  cout << "True antecedents = " << count << "\n\n";

  for (i = 0, count = 0; i < NbRules; i++)
    count += Clean[i]->NbAnt;

  cout << "Antecedents = " << count << "\n\n";

  return r;
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::UnordAccWithDef(ostream &ruleFile)

{
  int r;
  int c;
  int s;
  int count;
  int z;
  int indRule;
  int cl;
  int clNet;
  int flagContr;

  int globalAcc = 0;
  int globalWithout = 0;
  int contr = 0;
  int fidelity = 0;
  int nbEx = Test.GetNbEx();
  int nbRules = DefDef();

  ResetSomeFields();

  for (int p = 0; p < nbEx; p++) {
    std::vector<int> vectRulCar(nbRules);

    Bpnn->ForwardOneExample1(Test, p);
    clNet = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    for (r = 0, c = 0, z = 0; r < nbRules; r++) {
      if (IsExampleCarried(Test, p, Clean[r]) == 1) {
        z = 1;

        cout << p + 1 << ": "
             << "-- R" << r + 1 << " " << 1 + Clean[r]->Classification << " " << clNet + 1 << " " << ClassPatNetTest[p] + 1 << "\n";

        Clean[r]->NbCarriedTest += 1;

        vectRulCar[c] = r;
        c++;

        if (Clean[r]->Classification == ClassPatNetTest[p])
          Clean[r]->NbCorrectTest += 1;
        else
          Clean[r]->NbWrongTest += 1;
      }
    }

    if (z == 0)
      cout << p + 1 << ": "
           << "-- Default"
           << " " << 1 + Clean[nbRules]->Classification << " " << clNet + 1 << " " << ClassPatNetTest[p] + 1 << "\n";
    cout << "\n";

    if (c == 0) /* Default */
    {
      Clean[nbRules]->NbCarriedTest += 1;

      if (Clean[nbRules]->Classification == ClassPatNetTest[p]) {
        Clean[nbRules]->NbCorrectTest += 1;
        globalAcc++;
      } else
        Clean[nbRules]->NbWrongTest += 1;

      if (clNet == Clean[nbRules]->Classification) {
        fidelity++;
        if (clNet == ClassPatNetTest[p])
          globalWithout++;
      }
    }

    else /* check contradictions */
    {
      indRule = vectRulCar[0];
      cl = Clean[indRule]->Classification;

      for (s = 1, flagContr = 0; s < c; s++) {
        indRule = vectRulCar[s];

        if (Clean[indRule]->Classification != cl) {
          contr++;
          flagContr = 1;
          break;
        }
      }

      if (flagContr == 0) {
        if (clNet == Clean[vectRulCar[0]]->Classification) {
          fidelity++;
          if (clNet == ClassPatNetTest[p])
            globalWithout++;
        }

        for (s = 0; s < c; s++) /* for global accuracy */
        {
          indRule = vectRulCar[s];

          if (Clean[indRule]->Classification == ClassPatNetTest[p]) {
            globalAcc++;
            break;
          }
        }
      } else {
        for (s = 0; s < c; s++) /* for global accuracy */
        {
          indRule = vectRulCar[s];

          if (clNet == Clean[indRule]->Classification && Clean[indRule]->Classification == ClassPatNetTest[p]) {
            globalAcc++;
            break;
          }
        }

        for (s = 0; s < c; s++) // fidelity
        {
          indRule = vectRulCar[s];

          if (clNet == Clean[indRule]->Classification) {
            fidelity++;

            if (clNet == ClassPatNetTest[p])
              globalWithout++;

            break;
          }
        }
      }
    }
  }

  cout << "--- Accuracy on testing set = " << (float)globalAcc * 100.0 / (float)nbEx << "\n";
  cout << "--- Fidelity = " << (float)fidelity * 100.0 / (float)nbEx << "\n";
  cout << "--- Acc on Fid = " << (float)globalWithout * 100.0 / (float)fidelity << "\n";
  cout << "--- Contradictions = " << (float)contr * 100.0 / (float)nbEx << "\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  ruleFile << "Testing set:\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  for (r = 0, count = 0; r < nbRules; r++) {
    if (r != NbRules) {
      ruleFile << "Rule ";
      ruleFile.width(4);
      count++;
      ruleFile << count << ": ";
    }

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCarriedTest << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCorrectTest << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbWrongTest << " ";

    if (Clean[r]->NbCarriedTest != 0) {
      Clean[r]->AccuracyTest = (float)Clean[r]->NbCorrectTest /
                               (float)Clean[r]->NbCarriedTest;
      ruleFile.width(12);
      ruleFile << Clean[r]->AccuracyTest << " ";
    } else
      ruleFile << "              ";

    ruleFile << "      Class = " << Clean[r]->StrClass;

    ruleFile << "\n";
  }

  ruleFile << "\n---------------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////////////////

void CleanRuleStruct::UnordAccWithDef2(ostream &ruleFile)

{
  int r;
  int c;
  int s;
  int count;
  int z;
  int indRule;
  int cl;
  int clNet;
  int flagContr;

  int globalAcc = 0;
  int globalWithout = 0;
  int contr = 0;
  int fidelity = 0;
  int nbEx = Test.GetNbEx();
  int nbRules = NbRules;
  int countDef = 0;
  int noFid = 0;
  int noFidBad = 0;

  ResetSomeFields();

  cout << "CLASSIFICATION ON THE TESTING SET\n\n";
  cout << "(Case: Activated Rule -- Rule Class, Net Class, True Class):\n\n";

  for (int p = 0; p < nbEx; p++) {
    std::vector<int> vectRulCar(nbRules);

    Bpnn->ForwardOneExample1(Test, p);
    clNet = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    for (r = 0, c = 0, z = 0; r < nbRules; r++) {
      if (IsExampleCarried(Test, p, Clean[r]) == 1) {
        z = 1;

        cout << p + 1 << ": R" << r + 1 << " -- " << 1 + Clean[r]->Classification << " " << clNet + 1 << " " << ClassPatNetTest[p] + 1;
        if (Clean[r]->Classification != clNet)
          cout << " ***";
        if ((Clean[r]->Classification == clNet) && (clNet != ClassPatNetTest[p]))
          cout << " ---";
        cout << "\n";
        Clean[r]->NbCarriedTest += 1;

        vectRulCar[c] = r;
        c++;

        if (Clean[r]->Classification == ClassPatNetTest[p])
          Clean[r]->NbCorrectTest += 1;
        else
          Clean[r]->NbWrongTest += 1;
      }
    }

    if (z == 0) {
      cout << p + 1 << ": "
           << " -- Net Class: " << clNet + 1 << " True Class: " << ClassPatNetTest[p] + 1;
      if (clNet != ClassPatNetTest[p])
        cout << " ---";
      cout << "\n";
    }
    cout << "\n";

    if (z == 0) /* Default */
    {
      countDef++;
      fidelity++;

      if (clNet == ClassPatNetTest[p]) {
        globalAcc++;
        globalWithout++;
      }
    }

    else /* check contradictions */
    {
      indRule = vectRulCar[0];
      cl = Clean[indRule]->Classification;

      for (s = 1, flagContr = 0; s < c; s++) {
        indRule = vectRulCar[s];

        if (Clean[indRule]->Classification != cl) {
          contr++;
          flagContr = 1;
          break;
        }
      }

      if (flagContr == 0) {
        if (clNet == Clean[vectRulCar[0]]->Classification) {
          fidelity++;
          if (clNet == ClassPatNetTest[p])
            globalWithout++;
        }

        else {
          if (clNet == ClassPatNetTest[p])
            noFid++;
          else
            noFidBad++;
        }

        for (s = 0; s < c; s++) /* for global accuracy */
        {
          indRule = vectRulCar[s];

          if (Clean[indRule]->Classification == ClassPatNetTest[p]) {
            globalAcc++;
            break;
          }
        }
      } else {
        for (s = 0; s < c; s++) /* for global accuracy */
        {
          indRule = vectRulCar[s];

          if (clNet == Clean[indRule]->Classification && Clean[indRule]->Classification == ClassPatNetTest[p]) {
            globalAcc++;
            break;
          }
        }

        for (s = 0; s < c; s++) // fidelity
        {
          indRule = vectRulCar[s];

          if (clNet == Clean[indRule]->Classification) {
            fidelity++;

            if (clNet == ClassPatNetTest[p])
              globalWithout++;

            break;
          }
        }

        if (s == c) // no fidelity
        {
          if (clNet == ClassPatNetTest[p])
            noFid++;
          else
            noFidBad++;
        }
      }
    }
  }

  ruleFile << "--- Accuracy of rules on testing set = " << (float)globalAcc / (float)nbEx << "\n";
  ruleFile << "--- Fidelity on testing set (" << fidelity << "/" << nbEx << ") = " << (float)fidelity / (float)nbEx << "\n";
  ruleFile << "--- Accuracy when rules and network agree (" << globalWithout << "/" << fidelity << ") = " << (float)globalWithout / (float)fidelity << "\n\n";
  ruleFile << "--- Default rule activations rate (network classification) = " << (float)countDef / (float)nbEx << "\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  ruleFile << "Testing set:\n\n";

  ruleFile << "---------------------------------------------------------\n\n";

  for (r = 0, count = 0; r < nbRules; r++) {
    if (r != NbRules) {
      ruleFile << "Rule ";
      ruleFile.width(4);
      count++;
      ruleFile << count << ": ";
    }

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCarriedTest << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbCorrectTest << " ";

    ruleFile.width(5);
    ruleFile << Clean[r]->NbWrongTest << " ";

    if (Clean[r]->NbCarriedTest != 0) {
      Clean[r]->AccuracyTest = (float)Clean[r]->NbCorrectTest /
                               (float)Clean[r]->NbCarriedTest;
      ruleFile.width(12);
      ruleFile << Clean[r]->AccuracyTest << " ";
    } else {
      ruleFile << "              ";
    }

    ruleFile << "      Class = " << Clean[r]->StrClass;

    ruleFile << "\n";
  }

  ruleFile << "\n---------------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////////////////
