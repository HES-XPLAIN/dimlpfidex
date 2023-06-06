#ifndef CLEANRS_H
#define CLEANRS_H

#include <iostream>

#ifndef FROMREAL
#include "bpNN.h"
#include "rulePro.h"
#endif

////////////////////////////////////////////////////////////////////////

class CleanRuleStruct {
  DataSet All;
  DataSet Train;
  DataSet TrainClass;
  DataSet Test;
  DataSet TestClass;
  DataSet Valid;
  DataSet ValidClass;

  int NbRules;
  int NbAnt;
  int NbOut;
  int DefaultClass;

  struct AssocAnte {
    int Var;
    float Val;
    char Rel;
    std::string Str;
  };

  struct CleanRule {
    std::vector<AssocAnte> SevAnt;
    int NbAnt;
    int Classification;
    std::string StrClass;
    int NbAllCarried;
    int NbCarriedTrain;
    int NbCarriedTest;
    int NbCarriedValid;
    int NbCorrectTrain;
    int NbCorrectTest;
    int NbCorrectValid;
    int NbWrongTrain;
    int NbWrongTest;
    int NbWrongValid;
    float AccuracyTrain;
    float AccuracyTest;
    float AccuracyValid;
    int Flag;
  };

  std::shared_ptr<BpNN> Bpnn;
  std::shared_ptr<RuleProcessing> Pruned;
  float *Out;

  std::vector<std::shared_ptr<CleanRule>> Clean;
  std::vector<int> WrongTrain;
  std::vector<int> WrongTest;
  std::vector<int> WrongValid;
  std::vector<int> ClAllNet;

  std::vector<int> ClassPatNetTrain;
  std::vector<int> ClassPatNetTest;
  std::vector<int> ClassPatNetValid;

  // ------------------------------------------------------------------------

  void ResetFlag() const;
  void SetFlagToOne() const;
  int CountFlaggedRules(int flag) const;
  int CountFlaggedAnt(int flag) const;
  int CountCarPatByFlags(int flag) const;
  float ComputeAvgCar(int flag) const;
  float ComputeAvgAnt(int flag) const;
  int FindMinOnAnt(const AssocAnte *ant, int nbAnt) const;
  void SortAnt(AssocAnte *ant, int nbAnt, int indRule) const;
  int FindMaxOnRules(int start) const;
  int FindMinAntOnRules(int start) const;
  void SortRules(int indMin);

  float GlobalAcc(DataSet &data, int *vecWrong, int nbEl) const;
  void ComputeAcc(
      std::shared_ptr<StringInt> carried,
      int *indWrong,
      DataSet &data,
      DataSet &dataClass,
      int *nbPat,
      int *correct,
      int *wrong,
      float *acc) const;

  void SetSevInfo(std::shared_ptr<Rule> rule, int indClean);
  void CreateWrongVect();
  void CreateStructures();

  void ResetSomeFields() const;
  int IsExampleCarried(DataSet &data, int index, std::shared_ptr<CleanRule> rule) const;
  int Fidelity100();
  int Fidelity100Def();

  void ElseRepAll();
  void ElseRepTrain();
  void ElseRepTest();
  void ElseRepValid();

  void RemAnt(AssocAnte *oneAnt) const { oneAnt->Var = -1; }
  int IsAntDeleted(const AssocAnte *oneAnt) const { return ((oneAnt->Var == -1) ? 1 : 0); }

  int RandomPruneAnt();
  int SumCarried() const;

  void RemRule(int r) const;
  int PruneRule();
  int OrderedExpand(ThresDescr *descr);
  int CheckAnt(int indOld) const;
  void CreateNewClean();
  int SetDefRule();
  void SetClassPatNet();
  void WriteNumb() const;

  std::string ItoA(int n) const;

  int DefDef() const;
  void UnordAccWithDef(std::ostream &ruleFile);
  void UnordAccWithDef2(std::ostream &ruleFile);

  // ------------------------------------------------------------------------

public:
  void WriteRules(int def, std::ostream &ruleFile);
  void ElseRepresentation();
  void SimplifyElse(ThresDescr *descr);

  void Del() { NbRules = 0; };

  void SetAttr(std::vector<std::string> listAttr) const;
  void SetAttr();
  void SetStrClass(std::vector<std::string> listClass, int def) const;
  void SetStrClass(int def);

  CleanRuleStruct(
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
      int nbOut);

  ////////////////////////////////////////////////////////////////////////
};

#endif
