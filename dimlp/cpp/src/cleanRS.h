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
    AssocAnte *SevAnt;
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

  BpNN *Bpnn;
  RuleProcessing *Pruned;
  float *Out;

  CleanRule **Clean;
  int *WrongTrain;
  int *WrongTest;
  int *WrongValid;
  int *ClAllNet;
  int *VectVar;
  float *VectVal;

  int *ClassPatNetTrain;
  int *ClassPatNetTest;
  int *ClassPatNetValid;

  // ------------------------------------------------------------------------

  void ResetFlag() const;
  void SetFlagToOne() const;
  int CountFlaggedRules(int flag) const;
  int CountFlaggedAnt(int flag) const;
  int CountCarPatByFlags(int flag) const;
  float ComputeAvgCar(int flag) const;
  float ComputeAvgAnt(int flag) const;
  int FindMinOnAnt(const AssocAnte *ant, int nbAnt) const;
  void SortAnt(AssocAnte *ant, int nbAnt, int indRule);
  int FindMaxOnRules(int start) const;
  int FindMinAntOnRules(int start) const;
  void SortRules(int indMin);

  float GlobalAcc(DataSet &data, int *vecWrong, int nbEl);
  void ComputeAcc(
      StringInt *carried,
      int *indWrong,
      DataSet &data,
      DataSet &dataClass,
      int *nbPat,
      int *correct,
      int *wrong,
      float *acc);

  void SetSevInfo(Rule *rule, int indClean);
  void CreateWrongVect();
  void CreateStructures();

  void ResetSomeFields() const;
  int IsExampleCarried(DataSet &data, int index, const CleanRule *rule) const;
  int Fidelity100();
  int Fidelity100Def();

  void ElseRepAll();
  void ElseRepTrain();
  void ElseRepTest();
  void ElseRepValid();

  void RemAnt(AssocAnte *oneAnt) { oneAnt->Var = -1; }
  int IsAntDeleted(const AssocAnte *oneAnt) const { return ((oneAnt->Var == -1) ? 1 : 0); }

  int RandomPruneAnt();
  int SumCarried() const;

  void RemRule(int r);
  int PruneRule();
  int OrderedExpand(ThresDescr *descr);
  int CheckAnt(int indOld) const;
  void CreateNewClean();
  int SetDefRule();
  void DelClassPat();
  void SetClassPatNet();
  void WriteNumb() const;

  char *ItoA(int n);

  int DefDef() const;
  void UnordAccWithDef(std::ostream &ruleFile);
  void UnordAccWithDef2(std::ostream &ruleFile);

  // ------------------------------------------------------------------------

public:
  void WriteRules(int def, std::ostream &ruleFile);
  void ElseRepresentation();
  void SimplifyElse(ThresDescr *descr);

  void Del();

  void SetAttr(std::vector<std::string> listAttr) const;
  void SetAttr();
  void SetStrClass(std::vector<std::string> listClass, int def) const;
  void SetStrClass(int def);

  CleanRuleStruct(
      DataSet all,
      DataSet train,
      DataSet trainClass,
      DataSet valid,
      DataSet validClass,
      DataSet test,
      DataSet testClass,
      RuleProcessing *processed,
      BpNN *bpNn,
      float *out,
      int nbOut);

  ////////////////////////////////////////////////////////////////////////
};

#endif
