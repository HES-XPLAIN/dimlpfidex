#ifndef REALHYP_H
#define REALHYP_H

using namespace std;
#ifndef  BPNN
#include "bpNN.h"
#endif

#ifndef   VIRT
#include "virtHyp.h"
#endif

#define  STRINGINT 1
#define  THRESDESCR 1
#include "thresD.h"
#define  RULE 1
#include "ante.h"
#define  DATASETS 1
#include "rulePro.h"
#include "attrName.h"

////////////////////////////////////////////////////////////////////////

class RealHyp
{
   BpNN*           Bpnn;
   VirtualHyp*     Virt;
   ThresDescr*     Descr;
   RuleProcessing* SavedRules;

//----------------------------------------------------------------

   float*    In;
   float*    Out;
   float*    Bias;
   float*    Weights;

   int       NbBins;
   int       NbIn;
   int       Multiple;
   int       NbHyp;
   int       NbWeightLayers;
   int       NbOut;               // Attenzione al numero di classi quando = 1

   int**     ConfirmedVirt;
   int*      ConfBefFirstHyp;
   int*      ClassPatNet;

   int       Aborted;
//----------------------------------------------------------------

   void  SaveRule(Rule* path) { SavedRules->Insert(path); }

   int   MaxOnPos(int* vec, int nbEl);
   int   GiveIndMax(int* vec, int nbEl);

   void  SetConfirmedVirt2();

   void  Gl1(int var, int indPat, int startVirt, int netAns);
   void  Gr1(int var, int indPat, int startVirt, int netAns);

   virtual void GoLeft(int var, int indPat, int startVirt, int netAns)
                { Gl1(var, indPat, startVirt, netAns); }

   virtual void GoRight(int var, int indPat, int startVirt, int netAns)
                { Gr1(var, indPat, startVirt, netAns); }

   void  OneExRealHyp(DataSet& data, int indPat);

   Ante* FindMostDiscrAnt(int sel);

   int   ComputeCorrect(StringInt* listPatLeft, StringInt* listPatRight);
   void  SetCountPatDiscr(StringInt* listPat, Rule* r);
   void  SetCountPatDiscr2(DataSet& data, StringInt* listPat, Rule* r);

   void  DeepSearch(DataSet& data, Rule* path, StringInt* subSet);
   void  DeepSearch2(DataSet& data, Rule* path);

//----------------------------------------------------------------

public:

   int TreeAborted() { return Aborted; }

   void  SetConfirmedVirt(DataSet& data);
   void  SetRealHyp(DataSet& data);

   void  Gl2(int var, int indPat, int startVirt, int netAns);
   void  Gr2(int var, int indPat, int startVirt, int netAns);

   void RuleExtraction
   (
      DataSet&  data,
      DataSet&  train,
      DataSet&  trainClass,
      DataSet&  valid,
      DataSet&  validClass,
      DataSet&  test,
      DataSet&  testClass,
      AttrName& attr,
      ostream&  ruleFile
   );

//----------------------------------------------------------------

   void Del();

   RealHyp
   (
      DataSet& data,
      BpNN*    nn,
      int      nbBins,
      int      nbIn,
      int      multiple,
      int      nbWeightLayers
   );

   RealHyp
   (
      DataSet& data,
      BpNN*    nn,
      int      nbBins,
      int      nbIn,
      int      multiple,
      int      nbWeightLayers,
      int      dummy
   );

   RealHyp
   (
      VirtualHyp* globalVirt,
      int         nbNets,
      float*      out,
      int         nbOut,
      DataSet&    data,
      BpNN*       nn, 
      int         nbBins,
      int         nbIn,
      int         multiple,
      int         nbWeightLayers
   );

   RealHyp
   (
      VirtualHyp* globalVirt,
      int         nbNets,
      float*      out,
      int         nbOut,
      DataSet&    data,
      BpNN*       nn, 
      int         nbBins,
      int         nbIn,
      int         multiple,
      int         nbWeightLayers,
      int         dummy
   );

};

////////////////////////////////////////////////////////////////////////

#endif