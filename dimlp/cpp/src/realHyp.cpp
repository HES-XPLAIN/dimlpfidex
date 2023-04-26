using namespace std;
#include "realHyp.h"
#include "misc.h"
#define STRINGINT 1
#include "vectWRV.h"

#define   FROMREAL 1
#include "cleanRS.h"

////////////////////////////////////////////////////////////////////////

int RealHyp::MaxOnPos(int* vec, int nbEl)

{  int i;
   int max = *vec;

   for (i=1; i<nbEl; i++)
   {
       if (vec[i] > max)
          max = vec[i];
   }

   return max;
}

////////////////////////////////////////////////////////////////////////

int RealHyp::GiveIndMax(int* vec, int nbEl)

{  int i;
   int max = *vec;
   int indMax = 0;

   for (i=1; i<nbEl; i++)
   {
       if (vec[i] > max)
       {
          max = vec[i];
          indMax = i;
       }
   }

   return indMax;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetConfirmedVirt(DataSet& data)

{  int    nbPat = data.GetNbEx();
   int*   hyp;
   int    p, h, v, indVirt;
   float* pat;

   cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES ...\n\n";

   for (v=0; v<NbIn; v++)
   {
       ConfBefFirstHyp[v] = 0;

       hyp = ConfirmedVirt[v];

       for (h=0; h<NbHyp; h++, hyp++)
           *hyp = 0;
   }

   for (p=0; p<nbPat; p++)
   {
       pat = data.GetExample(p);

       for (v=0; v<NbIn; v++, pat++)
       {
           indVirt = Virt->KnotInd(v, *pat);

/*//*/     if ((indVirt >= 0) && (indVirt <= NbHyp-1))
              *(ConfirmedVirt[v] + indVirt) = 1;
/*//*/     else if (indVirt == -1)
              ConfBefFirstHyp[v] = 1;
           else
              cout << "*** ELSE WARNING !\n\n";
       }
   }

   int count = 0;

   for (v=0; v<NbIn; v++)
   {
       hyp = ConfirmedVirt[v];

       for (h=0; h<NbHyp; h++, hyp++)
           if (*hyp == 1) count++;
   }

   cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetConfirmedVirt2()

{  const  int nbRules = SavedRules->GetNbRules();
   Rule*  rule;
   int*   hyp;
   int    a, h, r, v, var, indVirt, nbAnt;
   float  val;

   cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES FROM RULES ...\n\n";

   for (v=0; v<NbIn; v++)
   {
       hyp = ConfirmedVirt[v];

       for (h=0; h<NbHyp; h++, hyp++)
           *hyp = 0;
   }

   for (r=0, SavedRules->GoToBeg(); r<nbRules; r++, SavedRules->GoToNext())
   {
       rule = SavedRules->GetRule();

       if (SavedRules->IsRuleEmpty(rule) == 1) continue;

       nbAnt = rule->GetNbAnt();

       for (a=0, rule->GoToBeg(); a<nbAnt; a++, rule->GoToNext())
       {
           if (rule->IsAntDeleted() == 0)
           {
              var = rule->GetVar();
              val = rule->GetVal();

              indVirt = Virt->GetInd(var, val);

              if (indVirt == -1)
              {
                 cout << "Warning ... " << var << " " << val << "\n";
              }

              else
                 *(ConfirmedVirt[var] + indVirt) = 1;
           }
       }
   }

   int count = 0;

   for (v=0; v<NbIn; v++)
   {
       hyp = ConfirmedVirt[v];

       for (h=0; h<NbHyp; h++, hyp++)
           if (*hyp == 1) count++;
   }

   cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gr1
(
    int   var,
    int   indPat,
    int   startVirt,
    int   netAns)

{  int    k;
   int    newNetAns;

   float* ptrIn   = In + var;
   float* ptrVirt = Virt->GetEpsGoRight(var) + startVirt;
   int*   ptrConf = ConfirmedVirt[var] + startVirt;

   for (k=startVirt; k<NbHyp; k++, ptrVirt++, ptrConf++)
   {
       if (*ptrConf == 0) continue;

       *ptrIn = *ptrVirt;

       Bpnn->ForwardOneExample1();

       newNetAns = Bpnn->Max(Out, NbOut);

       if (newNetAns != netAns)
       {
           Descr->Insert(var, *((Virt->GetVirtHyp(var))+k), indPat);
           break;
       }

    }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gr2
(
    int   var,
    int   indPat,
    int   startVirt,
    int   netAns)

{  int    k;
   int    newNetAns;

   const int last = NbHyp-1;
//cout << "gr2\n";
   float* ptrIn   = In + var;
   float* ptrVirt = Virt->GetEpsGoRight(var) + startVirt;
   int*   ptrConf = ConfirmedVirt[var] + startVirt;

   for (k=startVirt; k<NbHyp; k++, ptrVirt++, ptrConf++)
   {
       if (*ptrConf == 0) continue;

       *ptrIn = *ptrVirt;

       Bpnn->ForwardOneExample1();

       newNetAns = Bpnn->Max(Out, NbOut);

       if (newNetAns != netAns)
       {
           Descr->Insert(var, *((Virt->GetVirtHyp(var))+k), indPat);
           break;
       }

       if (k == last)
          Descr->Insert(var, *((Virt->GetVirtHyp(var))+startVirt), indPat);
    }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gl1
(
    int   var,
    int   indPat,
    int   startVirt,
    int   netAns)

{  int    k;
   int    newNetAns;

   float* ptrIn    = In + var;
   float* ptrVirt  = Virt->GetEpsGoLeft(var) + startVirt;
   float* ptrStart = Virt->GetVirtHyp(var) + startVirt;
   int*   ptrConf  = ConfirmedVirt[var] + startVirt;

   for (k=startVirt; k>=0; k--, ptrVirt--, ptrConf--)
   {
       if (k != 0)
       {
          if (*(ptrConf-1) == 0) continue;
       }
       else
       {
          if (ConfBefFirstHyp[var] == 0) continue;
       }

       *ptrIn = *ptrVirt;

       Bpnn->ForwardOneExample1();

       newNetAns = Bpnn->Max(Out, NbOut);

       if (newNetAns != netAns)
       {
           Descr->Insert(var, *ptrStart, indPat);
           break;
       }
   }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gl2
(
    int   var,
    int   indPat,
    int   startVirt,
    int   netAns)

{  int    k;
   int    newNetAns;

   float* ptrIn    = In + var;
   float* ptrVirt  = Virt->GetEpsGoLeft(var) + startVirt;
   float* ptrStart = Virt->GetVirtHyp(var) + startVirt;
   int*   ptrConf  = ConfirmedVirt[var] + startVirt;

   for (k=startVirt; k>=0; k--, ptrVirt--, ptrConf--)
   {
       if (k != 0)
       {
          if (*(ptrConf-1) == 0) continue;
       }
       else
       {
          Descr->Insert(var, *ptrStart, indPat);
          break;
       }

       *ptrIn = *ptrVirt;

       Bpnn->ForwardOneExample1();

       newNetAns = Bpnn->Max(Out, NbOut);

       if (newNetAns != netAns)
       {
          Descr->Insert(var, *ptrStart, indPat);
          break;
       }
   }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::OneExRealHyp(DataSet& data, int indPat)

{   int   indVirt, v, netAns;
    float inv;

    Bpnn->ForwardOneExample1(data, indPat);
    netAns = ClassPatNet[indPat];
    In     = data.GetExample(indPat);

    for (v=0; v<NbIn; v++)
    {
         inv       = In[v];
         indVirt   = Virt->KnotInd(v, inv);

         if (indVirt == -1)
            GoRight(v, indPat, 0, netAns);

         else if (indVirt == NbHyp-1)
            GoLeft(v, indPat, NbHyp-1, netAns);

         else
         {
            GoLeft(v, indPat, indVirt, netAns);
            GoRight(v, indPat, indVirt+1, netAns);
         }

         In[v] = inv;
    }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetRealHyp(DataSet& data)

{  int   p;
   int   modulo;
   const int nbEx   = data.GetNbEx();

   cout << "*** ESTIMATING RELEVANCE OF DISCRIMINANT HYPER-PLANES ";
   cout << "WITH AVAILABLE EXAMPLES ...\n\n";

   if (nbEx >= 10)
      modulo = nbEx / 10;
   else
      modulo = 1;

   for (p=0; p<nbEx; p++)
   {
       if (p % modulo == 0)
          cout << p << "\n";

       OneExRealHyp(data, p);
   }

   cout << p << "\n\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetCountPatDiscr(StringInt* listPat, Rule* r)

{  int   a, i, j, t, v, nbThres, nbPatThres, listPatThresVal, val;
   float thres;

   OneVarThresDescr* varDescr;
   StringInt*        listPatOneThres;

   int nbPat     = listPat->GetNbEl();
   int nbRuleAnt = r->GetNbAnt();

   Descr->ResetAllCountPatDiscr();

   for (v=0; v<NbIn; v++)
   {
       varDescr = Descr->GetDescr(v);
       nbThres  = varDescr->GetNbThres();

       for (t=0, varDescr->GoToBeg(); t<nbThres; t++, varDescr->GoToNext())
       {

           thres = varDescr->GetThres();

           for (a=0, r->GoToBeg(); a<nbRuleAnt; a++, r->GoToNext())
           {
               if ((v == r->GetVar()) && (thres == r->GetVal())) break;
           }

           if (a != nbRuleAnt)
           {
              continue;
           }

           listPatOneThres = varDescr->GetPtrPat();
           nbPatThres      = listPatOneThres->GetNbEl();

           listPatOneThres->GoToBeg();
           for(i=0; i<nbPatThres; i++, listPatOneThres->GoToNext())
           {
              listPatThresVal = listPatOneThres->GetVal();

              for (j=0, listPat->GoToBeg(); j<nbPat; j++, listPat->GoToNext())
              {
                  val = listPat->GetVal();

                  if (listPatThresVal == val)
                     varDescr->IncCountPatDiscr();

                  if (val >= listPatThresVal) break;
              }
           }
       }
   }
}

////////////////////////////////////////////////////////////////////////

Ante* RealHyp::FindMostDiscrAnt(int sel)

{  int   v, t, var, oldCount, newCount, nbThres;
   float val;
   OneVarThresDescr* oneDescr;

   var = -1;
   val = -1;

   for (v=0, oldCount=sel, newCount=0; v<NbIn; v++)
   {
       oneDescr = Descr->GetDescr(v);
       nbThres  = oneDescr->GetNbThres();

       for (t=0, oneDescr->GoToBeg(); t<nbThres; t++, oneDescr->GoToNext())
       {
           newCount = oneDescr->GetCountPatDiscr();

           if (newCount > oldCount)
           {
              var      = v;
              val      = oneDescr->GetThres();
              oldCount = newCount;
           }
       }
   }

   Ante* ant = new Ante(var, val, '?');

   return ant;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::DeepSearch(DataSet& data, Rule* path, StringInt* subSet)

{  Rule newLeftPath;
   Rule newRightPath;

   StringInt* newListPat = data.Select(path, subSet);

   if (newListPat->GetNbEl() == 0)
   {
      newListPat->DelAll();
      path->Del();

      return;
   }

   if (AreSameClass(newListPat, ClassPatNet) == 1)
   {
      newListPat->DelAll();
      SaveRule(path);
      path->Del();

      return;
   }

   SetCountPatDiscr(newListPat, path);

   Ante* ant = FindMostDiscrAnt(0);
   int   var = ant->GetVar();
   float val = ant->GetVal();

   if (var < 0)
   {
//cout << "%%%%%%3333333\n\n";
//cout << newListPat->GetNbEl() << "\n";

      ant->DelAll();
      newListPat->DelAll();
      DeepSearch2(data, path);
      path->Del();

      return;
   }

   (newLeftPath.Copy(path))->Insert(var, val, '<');
   (newRightPath.Copy(path))->Insert(var, val, '>');

   path->Del();
   ant->DelAll();

   DeepSearch(data, &newLeftPath, newListPat);
   DeepSearch(data, &newRightPath, newListPat);

   newListPat->DelAll();
}

////////////////////////////////////////////////////////////////////////

int RealHyp::ComputeCorrect
(
    StringInt* listPatLeft,
    StringInt* listPatRight
)

{  int  p, q, ind, nbPat, max;
   int* vectMax;
   int* classLeft;
   int* classRight;

   classLeft  = new int[NbOut];
   classRight = new int[NbOut];

   for (p=0; p<NbOut; p++)
   {
       classLeft[p]  = 0;
       classRight[p] = 0;
   }

   int nbComp = NbOut*(NbOut-1);
   vectMax    = new int[nbComp];

   nbPat = listPatLeft->GetNbEl();

   for (p=0, listPatLeft->GoToBeg(); p<nbPat; p++, listPatLeft->GoToNext())
       classLeft[ClassPatNet[listPatLeft->GetVal()]] += 1;

   nbPat = listPatRight->GetNbEl();

   for (p=0, listPatRight->GoToBeg(); p<nbPat; p++, listPatRight->GoToNext())
       classRight[ClassPatNet[listPatRight->GetVal()]] += 1;

   for (p=0, ind=0; p<NbOut; p++)
   {
       for (q=0; q<NbOut; q++)
       {
           if (q == p) continue;

            vectMax[ind]   = classLeft[p] + classRight[q];
            ind++;
       }
   }

   max = MaxOnPos(vectMax, nbComp);

   delete classLeft;
   delete classRight;
   delete vectMax;

   return max;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetCountPatDiscr2(DataSet& data, StringInt* listPat, Rule* r)

{  int   a, t, v, nbThres;
   static int  nbCorrect;
   float thres;

   OneVarThresDescr* varDescr;
   StringInt*        newListPatLeft  = new StringInt;
   StringInt*        newListPatRight = new StringInt;
   Rule*             newLeftPath  = new Rule;
   Rule*             newRightPath = new Rule;

   Descr->ResetAllCountPatDiscr();
   int nbRuleAnt = r->GetNbAnt();

   for (v=0; v<NbIn; v++)
   {
       varDescr = Descr->GetDescr(v);
       nbThres  = varDescr->GetNbThres();

       for (t=0, varDescr->GoToBeg(); t<nbThres; t++, varDescr->GoToNext())
       {
           thres = varDescr->GetThres();

           for (a=0, r->GoToBeg(); a<nbRuleAnt; a++, r->GoToNext())
           {
               if ((v == r->GetVar()) && (thres == r->GetVal())) break;
           }

           if (a != nbRuleAnt)
           {
              continue;
           }

           (newLeftPath->Copy(r))->Insert(v, thres, '<');  //to del
           (newRightPath->Copy(r))->Insert(v, thres, '>'); // to del

           newListPatLeft = data.Select(newLeftPath); //to del
           newListPatRight = data.Select(newRightPath); //to del

           nbCorrect = ComputeCorrect(newListPatLeft, newListPatRight);
           varDescr->SetCountPat(nbCorrect);

           newLeftPath->Del();
           newRightPath->Del();

           newListPatLeft->Del();
           newListPatRight->Del();
       }
   }

   delete newListPatLeft;
   delete newListPatRight;
   delete newLeftPath;
   delete newRightPath;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::DeepSearch2(DataSet& data, Rule* path)

{  Rule newLeftPath;
   Rule newRightPath;

   StringInt* newListPat = data.Select(path);

   if (newListPat->GetNbEl() == 0)
   {
      newListPat->DelAll();
      path->Del();
      return;
   }

   if (AreSameClass(newListPat, ClassPatNet) == 1)
   {
      newListPat->DelAll();
      SaveRule(path);
      path->Del();

      return;
   }

   SetCountPatDiscr2(data, newListPat, path);

   Ante* ant = FindMostDiscrAnt(0);
   int   var = ant->GetVar();
   float val = ant->GetVal();

   if (var < 0)
   {
      Aborted = 1;
      ant->DelAll();
      newListPat->DelAll();

      return;
   }

   (newLeftPath.Copy(path))->Insert(var, val, '<');
   (newRightPath.Copy(path))->Insert(var, val, '>');

   path->Del();
   ant->DelAll();
   newListPat->DelAll();

   DeepSearch2(data, &newLeftPath);
   DeepSearch2(data, &newRightPath);
}

////////////////////////////////////////////////////////////////////////

void RealHyp::RuleExtraction
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
)

{  Rule       empty;
   StringInt* listAll;
   int        nbAnt1;
   int        nbAnt2;

   Aborted    = 0;
   SavedRules = new RuleProcessing(NbIn, NbHyp, data, ClassPatNet, Descr);

   cout << "*** BUILDING DECISION TREE ...\n\n";

   listAll = data.Select(&empty);
   DeepSearch(data, &empty, listAll);
   listAll->DelAll();

   if (Aborted)
   {
      cout << "*** TREE ABORTED !\n\n";
      cout << "*** TRYING AN ALTERNATIVE ALGORITHM \n\n";

      SavedRules->Del();
      Descr->Del();
      Virt->Del();

      return;
   }

   SavedRules->Clean();

   if (SavedRules->CountAnt() == 0)
   {
      cout << "*** NO RULES !\n\n";
      cout << "--- Number of rules = 0\n";
      cout << "--- Number of antecedents = 0\n";
      cout << "--- Number of antecedents per rule = 0\n";
      cout << "--- Number of examples per rule = 0\n";

      cout << "*** NO RULES !\n\n";

      cout << "--- Number of rules = 0\n";
      cout << "--- Number of antecedents = 0\n";
      cout << "--- Number of antecedents per rule = 0\n";
      cout << "--- Number of examples per rule = 0\n";

      SavedRules->Del();

      Descr->Del();
      Virt->Del();

      return;
   }

   cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n\n";
   SavedRules->MixPrune();

   cout << "\n\n*** EXPANDING RULES ...\n\n";
   SavedRules->EnlargeAndPrune();

   nbAnt1 = SavedRules->CountAnt();

   cout << "\n\n*** RETRYING RULE EXTRACTION ...\n\n";

   CleanRuleStruct clean(data, train, trainClass,
                         valid, validClass, test, testClass,
                         SavedRules, Bpnn, Out, NbOut);

// clean.WriteRules(0, ruleFile);

   Descr->Del();
   SetConfirmedVirt2();
   SetRealHyp(data);

   SavedRules->Del();

   cout << "*** BUILDING DECISION TREE ...\n\n";

   listAll = data.Select(&empty);
   DeepSearch(data, &empty, listAll);
   listAll->DelAll();

   if (Aborted)
   {
      cout << "*** TREE ABORTED !\n\n";
      Aborted = 0;
      goto A;
   }

   SavedRules->Clean();

   cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n\n";
   SavedRules->MixPrune();

   cout << "\n\n*** EXPANDING RULES ...\n\n";
   SavedRules->EnlargeAndPrune();

   nbAnt2 = SavedRules->CountAnt();

   if (nbAnt2 < nbAnt1)
   {
      cout << "\n\n*** RULE SET IMPROVED BY " << nbAnt1-nbAnt2;
      cout << " ANTECEDENTS\n";

      clean.Del();
      CleanRuleStruct clean(data, train, trainClass,
                            valid, validClass, test, testClass,
                            SavedRules, Bpnn, Out, NbOut);

      cout << "\n\n*** WRITING IF-THEN RULES ...\n\n";

      if (attr.IsFileAttr())
      {
         clean.SetAttr(attr.GetListAttr());
         clean.SetStrClass(attr.GetListClasses(), 0);
      }
      else
      {
         clean.SetAttr();
         clean.SetStrClass(0);
      }

      clean.WriteRules(0, ruleFile);
/*
 cout << "\n\n*** DETERMINING IF-THEN-ELSE RULES FROM IF-THEN RULES ...\n\n";

      cout << "\n\n*** PRUNING AND EXPANDING ANTECEDENTS ...\n\n";

      clean.SimplifyElse(Descr);
      clean.ElseRepresentation();

      cout << "\n\n*** WRITING IF-THEN-ELSE RULES ...\n\n";

      if (attr.IsFileAttr())
      {
         clean.SetAttr(attr.GetListAttr());
         clean.SetStrClass(attr.GetListClasses(), 1);
      }
      else
      {
         clean.SetAttr();
         clean.SetStrClass(1);
      }

      clean.WriteRules(1, ruleFile);

      clean.Del();
*/
   }

   else
   {
      cout << "\n\n*** RULE SET DOES NOT IMPROVE (" << nbAnt1-nbAnt2 << ")\n";
A:
      cout << "\n    (WRITING PREVIOUS ONE)\n";

      cout << "\n\n*** WRITING IF-THEN RULES ...\n\n";

      if (attr.IsFileAttr())
      {
         clean.SetAttr(attr.GetListAttr());
         clean.SetStrClass(attr.GetListClasses(), 0);
      }
      else
      {
         clean.SetAttr();
         clean.SetStrClass(0);
      }

      clean.WriteRules(0, ruleFile);
/*
 cout << "\n\n*** DETERMINING IF-THEN-ELSE RULES FROM IF-THEN RULES ...\n\n";

      cout << "\n\n*** PRUNING AND EXPANDING ANTECEDENTS ...\n\n";

      clean.SimplifyElse(Descr);
      clean.ElseRepresentation();

      cout << "\n\n*** WRITING IF-THEN-ELSE RULES ...\n\n";

      if (attr.IsFileAttr())
      {
         clean.SetAttr(attr.GetListAttr());
         clean.SetStrClass(attr.GetListClasses(), 1);
      }
      else
      {
         clean.SetAttr();
         clean.SetStrClass(1);
      }

      clean.WriteRules(1, ruleFile);

      clean.Del();
*/
   }

   SavedRules->Del();

   Descr->Del();
   Virt->Del();
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Del()

{  int i;

   for (i=0; i<NbIn; i++)
       delete ConfirmedVirt[i];

   delete ConfirmedVirt;
   delete ConfBefFirstHyp;
   delete ClassPatNet;
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp
(
   DataSet& data,
   BpNN*    nn,
   int      nbBins,
   int      nbIn,
   int      multiple,
   int      nbWeightLayers
)

{  int  p;

   Bpnn           = nn;
   NbBins         = nbBins;
   NbIn           = nbIn;
   Multiple       = multiple;
   NbWeightLayers = nbWeightLayers;

   NbHyp     = (NbBins+1)*Multiple;
   NbOut     = (Bpnn->GetLayer(NbWeightLayers-1))->GetNbUp();

   Bias      = (Bpnn->GetLayer(0))->GetBias();
   Weights   = (Bpnn->GetLayer(0))->GetWeights();
   Out       = (Bpnn->GetLayer(NbWeightLayers-1))->GetUp();


   Virt         = new VirtualHyp(NbBins, NbIn, Multiple, Bias, Weights);
   Descr        = new ThresDescr(NbIn);
   ClassPatNet  = new int[data.GetNbEx()];

   for (p=0; p<data.GetNbEx(); p++)
   {
       Bpnn->ForwardOneExample1(data, p);
       ClassPatNet[p] = Bpnn->Max(Out, NbOut);
   }

   ConfirmedVirt   = new int*[NbIn];
   ConfBefFirstHyp = new int[NbIn];

   for (p=0; p<NbIn; p++)
       ConfirmedVirt[p] = new int[NbHyp];

   SetConfirmedVirt(data);
   SetRealHyp(data);
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp
(
   DataSet& data,
   BpNN*    nn,
   int      nbBins,
   int      nbIn,
   int      multiple,
   int      nbWeightLayers,
   int      dummy
)

{  int  p;

   Bpnn           = nn;
   NbBins         = nbBins;
   NbIn           = nbIn;
   Multiple       = multiple;
   NbWeightLayers = nbWeightLayers;

   NbHyp     = (NbBins+1)*Multiple;
   NbOut     = (Bpnn->GetLayer(NbWeightLayers-1))->GetNbUp();

   Bias      = (Bpnn->GetLayer(0))->GetBias();
   Weights   = (Bpnn->GetLayer(0))->GetWeights();
   Out       = (Bpnn->GetLayer(NbWeightLayers-1))->GetUp();


   Virt         = new VirtualHyp(NbBins, NbIn, Multiple, Bias, Weights);
   Descr        = new ThresDescr(NbIn);
   ClassPatNet  = new int[data.GetNbEx()];

   for (p=0; p<data.GetNbEx(); p++)
   {
       Bpnn->ForwardOneExample1(data, p);
       ClassPatNet[p] = Bpnn->Max(Out, NbOut);
   }

   ConfirmedVirt   = new int*[NbIn];
   ConfBefFirstHyp = new int[NbIn];

   for (p=0; p<NbIn; p++)
       ConfirmedVirt[p] = new int[NbHyp];
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp
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
)

{  int  p;

   Bpnn           = nn;
   NbBins         = nbBins;
   NbIn           = nbIn;
   Multiple       = multiple;
   NbWeightLayers = nbWeightLayers;

   NbHyp     = (NbBins+1) * Multiple * nbNets;
   NbOut     = nbOut;
   Out       = out;

   Virt         = globalVirt;
   Descr        = new ThresDescr(NbIn);
   ClassPatNet  = new int[data.GetNbEx()];

   for (p=0; p<data.GetNbEx(); p++)
   {
       Bpnn->ForwardOneExample1(data, p);
       ClassPatNet[p] = Bpnn->Max(Out, NbOut);
   }

   ConfirmedVirt   = new int*[NbIn];
   ConfBefFirstHyp = new int[NbIn];

   for (p=0; p<NbIn; p++)
       ConfirmedVirt[p] = new int[NbHyp];

   SetConfirmedVirt(data);
   SetRealHyp(data);
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp
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
)

{  int  p;

   Bpnn           = nn;
   NbBins         = nbBins;
   NbIn           = nbIn;
   Multiple       = multiple;
   NbWeightLayers = nbWeightLayers;

   NbHyp     = (NbBins+1) * Multiple * nbNets;
   NbOut     = nbOut;
   Out       = out;

   Virt         = globalVirt;
   Descr        = new ThresDescr(NbIn);
   ClassPatNet  = new int[data.GetNbEx()];

   for (p=0; p<data.GetNbEx(); p++)
   {
       Bpnn->ForwardOneExample1(data, p);
       ClassPatNet[p] = Bpnn->Max(Out, NbOut);
   }

   ConfirmedVirt   = new int*[NbIn];
   ConfBefFirstHyp = new int[NbIn];

   for (p=0; p<NbIn; p++)
       ConfirmedVirt[p] = new int[NbHyp];
}

////////////////////////////////////////////////////////////////////////
