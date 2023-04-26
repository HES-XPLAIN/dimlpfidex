#ifndef RULEPRO_H
#define RULEPRO_H

using namespace std;
#ifndef  DATASETS
#include "dataSet.h"
#endif

#define  STRINGINT 1

#ifndef  THRESDESCR
#include "thresD.h"
#endif

////////////////////////////////////////////////////////////////////////

class RuleProcessing

{  int   NbRules;
   int   NbVar;
   int   NbHyp;

   DataSet     Data;
   int*        ClassPatNet;
   int*        TabRules;
   ThresDescr* Descr;

   StringInt Gain;
   StringInt RuleInd;
   StringInt AntInd;

   struct Saved
   {
       Rule*      OneRule;
       StringInt* Carried;
       Saved*     Next;
   };

   Saved* First;
   Saved* Current;
   Saved* Memory;

//----------------------------------------------------------------

   int   Max(int* tab, int nbRules);
   int   Min(int* tab, int nbRules);
   void  SetCountAntRules();
   void  SetCarriedField();
   void  DelListCar();
   //int   CheckAllCarriedAndDisj(int toDrop);
   int   CheckAllCarried(int toDrop);
   void  RemCurrentRule();
   int   GoToSavedAndRemRule(int indPrune);
   void  GoToRuleAndRemAnt(int indPrune);
   void  RemSevThres(Rule* r);
   void  ComputeGain();
   void  RulePruneByMinCar();
   void  FastRulePrune(int nbIt);
   int   TryEnlargedThres();

//----------------------------------------------------------------

public:

   int   IsRuleEmpty(Rule* rule);
   int   GetNbRules() { return NbRules; }
   void  GoToBeg() { Current = First; }
   void  GoToNext() { Current = Current->Next; }
   Rule* GetRule() { return Current->OneRule; }
   void  Save() { Memory = Current; }
   void  Previous() { Current = Memory; }

   int   CountAnt();
   int   CountRules();

   void  Clean();
   void  MixPrune();
   void  EnlargeAndPrune();

   void  Insert(Rule* r);
   void  Del();


   RuleProcessing
   (
     int         nbVar,
     int         nbHyp,
     DataSet     data,
     int*        classPatNet,
     ThresDescr* descr
   );

   RuleProcessing() { NbRules=0; }

};

#endif
