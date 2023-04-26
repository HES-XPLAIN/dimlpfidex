#ifndef CLEANRS_H
#define CLEANRS_H

using namespace std;
#include <iostream>

#define WRITERULES 1

#ifndef  FROMREAL
#include "rulePro.h"
#include "bpNN.h"
#endif

////////////////////////////////////////////////////////////////////////

class CleanRuleStruct
{
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

   struct AssocAnte
   {
      int    Var;
      float  Val;
      char   Rel;
      char   Str[80];
   };

   struct CleanRule
   {
       AssocAnte*  SevAnt;
       int         NbAnt;
       int         Classification;
       char        StrClass[80];
       int         NbAllCarried;
       int         NbCarriedTrain;
       int         NbCarriedTest;
       int         NbCarriedValid;
       int         NbCorrectTrain;
       int         NbCorrectTest;
       int         NbCorrectValid;
       int         NbWrongTrain;
       int         NbWrongTest;
       int         NbWrongValid;
       float       AccuracyTrain;
       float       AccuracyTest;
       float       AccuracyValid;
       int         Flag;
   };

   BpNN*           Bpnn;
   RuleProcessing* Pruned;
   float*          Out;

   CleanRule**     Clean;
   int*            WrongTrain;
   int*            WrongTest;
   int*            WrongValid;
   int*            ClAllNet;
   int*            VectVar;
   float*          VectVal;

   int* ClassPatNetTrain;
   int* ClassPatNetTest;
   int* ClassPatNetValid;

// ------------------------------------------------------------------------

  void  ResetFlag();
  void  SetFlagToOne();
  int   CountFlaggedRules(int flag);
  int   CountFlaggedAnt(int flag);
  int   CountCarPatByFlags(int flag);
  float ComputeAvgCar(int flag);
  float ComputeAvgAnt(int flag);
  int   FindMinOnAnt(AssocAnte* ant, int nbAnt);
  void  SortAnt(AssocAnte* ant, int nbAnt, int indRule);
  int   FindMaxOnRules(int start);
  int   FindMinAntOnRules(int start);
  void  SortRules(int indMin);

  float GlobalAcc(DataSet& data, int* vecWrong, int nbEl);
  void  ComputeAcc
  (
     int        indClean,
     StringInt* carried,
     int*       indWrong,
     DataSet&   data,
     DataSet&   dataClass,
     int*       nbPat,
     int*       correct,
     int*       wrong,
     float*     acc
  );

  void  SetSevInfo(Rule* rule, int indClean);
  void  CreateWrongVect();
  void  CreateStructures();

  void  ResetSomeFields();
  int   IsExampleCarried(DataSet& data, int index, CleanRule* rule);
  int   Fidelity100();
  int   Fidelity100Def();

  void  ElseRepAll();
  void  ElseRepTrain();
  void  ElseRepTest();
  void  ElseRepValid();

  void  RemAnt(AssocAnte* oneAnt) { oneAnt->Var = -1; }
  int   IsAntDeleted(AssocAnte* oneAnt)
          { return ((oneAnt->Var == -1) ? 1 : 0); }

  int   RandomPruneAnt();
  int   SumCarried();

  void RemRule(int r);
  int  PruneRule();
  int  OrderedExpand(ThresDescr* descr);
  int  CheckAnt(int indOld);
  void CreateNewClean();
  int  SetDefRule();
  void DelClassPat();
  void SetClassPatNet();
  void WriteNumb();

  char* ItoA(int n);

  int  DefDef();
  void UnordAccWithDef(ostream& ruleFile);
  void UnordAccWithDef2(ostream& ruleFile);

// ------------------------------------------------------------------------

public:

  void  WriteRules(int def, ostream& ruleFile);
  void  ElseRepresentation();
  void  SimplifyElse(ThresDescr* descr);

  void  Del();

  void SetAttr(char** listAttr);
  void SetAttr();
  void SetStrClass(char** listClass, int def);
  void SetStrClass(int def);

  CleanRuleStruct
  (
     DataSet         all,
     DataSet         train,
     DataSet         trainClass,
     DataSet         valid,
     DataSet         validClass,
     DataSet         test,
     DataSet         testClass,
     RuleProcessing* processed,
     BpNN*           bpNn,
     float*          out,
     int             nbOut
  );

////////////////////////////////////////////////////////////////////////

};

#endif
