#include "realHyp.h"
#include "misc.h"
#define STRINGINT 1

#define FROMREAL 1
#include "cleanRS.h"

using namespace std;

////////////////////////////////////////////////////////////////////////

int RealHyp::MaxOnPos(const std::vector<int> &vec) const

{
  int max = vec[0];

  for (int i = 1; i < vec.size(); i++) {
    if (vec[i] > max)
      max = vec[i];
  }

  return max;
}

////////////////////////////////////////////////////////////////////////

int RealHyp::GiveIndMax(const int *vec, int nbEl) const

{
  int max = *vec;
  int indMax = 0;

  for (int i = 1; i < nbEl; i++) {
    if (vec[i] > max) {
      max = vec[i];
      indMax = i;
    }
  }

  return indMax;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetConfirmedVirt(DataSet &data)

{
  int nbPat = data.GetNbEx();
  int *hyp;
  int h;
  int v;
  int indVirt;
  const float *pat;

  cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES ...\n\n";

  for (v = 0; v < NbIn; v++) {
    ConfBefFirstHyp[v] = 0;

    hyp = ConfirmedVirt[v].data();

    for (h = 0; h < NbHyp; h++, hyp++)
      *hyp = 0;
  }

  for (int p = 0; p < nbPat; p++) {
    pat = data.GetExample(p);

    for (v = 0; v < NbIn; v++, pat++) {
      indVirt = Virt->KnotInd(v, *pat);

      if ((indVirt >= 0) && (indVirt <= NbHyp - 1))
        *(ConfirmedVirt[v].data() + indVirt) = 1;
      else if (indVirt == -1)
        ConfBefFirstHyp[v] = 1;
      else
        cout << "*** ELSE WARNING !\n\n";
    }
  }

  int count = 0;

  for (v = 0; v < NbIn; v++) {
    hyp = ConfirmedVirt[v].data();

    for (h = 0; h < NbHyp; h++, hyp++)
      if (*hyp == 1)
        count++;
  }

  cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetConfirmedVirt2()

{
  const int nbRules = SavedRules->GetNbRules();
  std::shared_ptr<Rule> rule;
  int *hyp;
  int a;
  int h;
  int r;
  int v;
  int var;
  int indVirt;
  int nbAnt;
  float val;

  cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES FROM RULES ...\n\n";

  for (v = 0; v < NbIn; v++) {
    hyp = ConfirmedVirt[v].data();

    for (h = 0; h < NbHyp; h++, hyp++)
      *hyp = 0;
  }

  for (r = 0, SavedRules->GoToBeg(); r < nbRules; r++, SavedRules->GoToNext()) {
    rule = SavedRules->GetRule();

    if (SavedRules->IsRuleEmpty(rule) == 1)
      continue;

    nbAnt = rule->GetNbAnt();

    for (a = 0, rule->GoToBeg(); a < nbAnt; a++, rule->GoToNext()) {
      if (rule->IsAntDeleted() == 0) {
        var = rule->GetVar();
        val = rule->GetVal();

        indVirt = Virt->GetInd(var, val);

        if (indVirt == -1) {
          cout << "Warning ... " << var << " " << val << "\n";
        }

        else
          *(ConfirmedVirt[var].data() + indVirt) = 1;
      }
    }
  }

  int count = 0;

  for (v = 0; v < NbIn; v++) {
    hyp = ConfirmedVirt[v].data();

    for (h = 0; h < NbHyp; h++, hyp++)
      if (*hyp == 1)
        count++;
  }

  cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gr1(
    int var,
    int indPat,
    int startVirt,
    int netAns)

{
  int newNetAns;

  float *ptrIn = In + var;
  const float *ptrVirt = Virt->GetEpsGoRight(var).data() + startVirt;
  const int *ptrConf = ConfirmedVirt[var].data() + startVirt;

  for (int k = startVirt; k < NbHyp; k++, ptrVirt++, ptrConf++) {
    if (*ptrConf == 0)
      continue;

    *ptrIn = *ptrVirt;

    Bpnn->ForwardOneExample1();

    newNetAns = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    if (newNetAns != netAns) {
      Descr->Insert(var, *((Virt->GetVirtHyp(var)).data() + k), indPat);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gr2(
    int var,
    int indPat,
    int startVirt,
    int netAns)

{
  int newNetAns;

  const int last = NbHyp - 1;
  float *ptrIn = In + var;
  const float *ptrVirt = Virt->GetEpsGoRight(var).data() + startVirt;
  const int *ptrConf = ConfirmedVirt[var].data() + startVirt;

  for (int k = startVirt; k < NbHyp; k++, ptrVirt++, ptrConf++) {
    if (*ptrConf == 0)
      continue;

    *ptrIn = *ptrVirt;

    Bpnn->ForwardOneExample1();

    newNetAns = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    if (newNetAns != netAns) {
      Descr->Insert(var, *((Virt->GetVirtHyp(var)).data() + k), indPat);
      break;
    }

    if (k == last)
      Descr->Insert(var, *((Virt->GetVirtHyp(var)).data() + startVirt), indPat);
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gl1(
    int var,
    int indPat,
    int startVirt,
    int netAns)

{
  int newNetAns;

  float *ptrIn = In + var;
  const float *ptrVirt = Virt->GetEpsGoLeft(var).data() + startVirt;
  const float *ptrStart = Virt->GetVirtHyp(var).data() + startVirt;
  const int *ptrConf = ConfirmedVirt[var].data() + startVirt;

  for (int k = startVirt; k >= 0; k--, ptrVirt--, ptrConf--) {
    if (k != 0) {
      if (*(ptrConf - 1) == 0)
        continue;
    } else {
      if (ConfBefFirstHyp[var] == 0)
        continue;
    }

    *ptrIn = *ptrVirt;

    Bpnn->ForwardOneExample1();

    newNetAns = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    if (newNetAns != netAns) {
      Descr->Insert(var, *ptrStart, indPat);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::Gl2(
    int var,
    int indPat,
    int startVirt,
    int netAns)

{
  int newNetAns;

  float *ptrIn = In + var;
  const float *ptrVirt = Virt->GetEpsGoLeft(var).data() + startVirt;
  const float *ptrStart = Virt->GetVirtHyp(var).data() + startVirt;
  const int *ptrConf = ConfirmedVirt[var].data() + startVirt;

  for (int k = startVirt; k >= 0; k--, ptrVirt--, ptrConf--) {
    if (k != 0) {
      if (*(ptrConf - 1) == 0)
        continue;
    } else {
      Descr->Insert(var, *ptrStart, indPat);
      break;
    }

    *ptrIn = *ptrVirt;

    Bpnn->ForwardOneExample1();

    newNetAns = Bpnn->Max(std::vector<float>(Out, Out + NbOut));

    if (newNetAns != netAns) {
      Descr->Insert(var, *ptrStart, indPat);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::OneExRealHyp(DataSet &data, int indPat)

{
  int indVirt;
  int netAns;
  float inv;

  Bpnn->ForwardOneExample1(data, indPat);
  netAns = ClassPatNet[indPat];
  In = data.GetExample(indPat);

  for (int v = 0; v < NbIn; v++) {
    inv = In[v];
    indVirt = Virt->KnotInd(v, inv);

    if (indVirt == -1)
      GoRight(v, indPat, 0, netAns);

    else if (indVirt == NbHyp - 1)
      GoLeft(v, indPat, NbHyp - 1, netAns);

    else {
      GoLeft(v, indPat, indVirt, netAns);
      GoRight(v, indPat, indVirt + 1, netAns);
    }

    In[v] = inv;
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetRealHyp(DataSet &data)

{
  int p;
  int modulo;
  const int nbEx = data.GetNbEx();

  cout << "*** ESTIMATING RELEVANCE OF DISCRIMINANT HYPER-PLANES ";
  cout << "WITH AVAILABLE EXAMPLES ...\n"
       << endl;

  if (nbEx >= 10)
    modulo = nbEx / 10;
  else
    modulo = 1;

  for (p = 0; p < nbEx; p++) {
    if (p % modulo == 0)
      cout << p << endl;

    OneExRealHyp(data, p);
  }

  cout << p << "\n\n\n";
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetCountPatDiscr(std::shared_ptr<StringInt> listPat, std::shared_ptr<Rule> r) const

{
  int a;
  int j;
  int t;
  int nbThres;
  int nbPatThres;
  int listPatThresVal;
  int val;
  float thres;

  OneVarThresDescr *varDescr;
  StringInt *listPatOneThres;

  int nbPat = listPat->GetNbEl();
  int nbRuleAnt = r->GetNbAnt();

  Descr->ResetAllCountPatDiscr();

  for (int v = 0; v < NbIn; v++) {
    varDescr = Descr->GetDescr(v);
    nbThres = varDescr->GetNbThres();

    for (t = 0, varDescr->GoToBeg(); t < nbThres; t++, varDescr->GoToNext()) {

      thres = varDescr->GetThres();

      for (a = 0, r->GoToBeg(); a < nbRuleAnt; a++, r->GoToNext()) {
        if ((v == r->GetVar()) && (thres == r->GetVal()))
          break;
      }

      if (a != nbRuleAnt) {
        continue;
      }

      listPatOneThres = varDescr->GetPtrPat();
      nbPatThres = listPatOneThres->GetNbEl();

      listPatOneThres->GoToBeg();
      for (int i = 0; i < nbPatThres; i++, listPatOneThres->GoToNext()) {
        listPatThresVal = listPatOneThres->GetVal();

        for (j = 0, listPat->GoToBeg(); j < nbPat; j++, listPat->GoToNext()) {
          val = listPat->GetVal();

          if (listPatThresVal == val)
            varDescr->IncCountPatDiscr();

          if (val >= listPatThresVal)
            break;
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////

std::shared_ptr<Ante> RealHyp::FindMostDiscrAnt(int sel) const

{
  int v;
  int t;
  int var;
  int oldCount;
  int newCount;
  int nbThres;
  float val;
  OneVarThresDescr *oneDescr;

  var = -1;
  val = -1;

  for (v = 0, oldCount = sel; v < NbIn; v++) {
    oneDescr = Descr->GetDescr(v);
    nbThres = oneDescr->GetNbThres();

    for (t = 0, oneDescr->GoToBeg(); t < nbThres; t++, oneDescr->GoToNext()) {
      newCount = oneDescr->GetCountPatDiscr();

      if (newCount > oldCount) {
        var = v;
        val = oneDescr->GetThres();
        oldCount = newCount;
      }
    }
  }
  auto ant = std::make_shared<Ante>(var, val, '?');

  return ant;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::DeepSearch(DataSet &data, std::shared_ptr<Rule> path, std::shared_ptr<StringInt> subSet)

{
  Rule newLeftPath;
  Rule newRightPath;

  std::shared_ptr<StringInt> newListPat = data.Select(path, subSet);

  if (newListPat->GetNbEl() == 0) {
    newListPat->Del();
    path->Del();

    return;
  }

  if (AreSameClass(newListPat, ClassPatNet) == 1) {
    newListPat->Del();
    SaveRule(path);
    path->Del();

    return;
  }

  SetCountPatDiscr(newListPat, path);

  std::shared_ptr<Ante> ant = FindMostDiscrAnt(0);
  int var = ant->GetVarAnte();
  float val = ant->GetValAnte();

  if (var < 0) {
    ant->DelAnte();
    newListPat->Del();
    DeepSearch2(data, path);
    path->Del();

    return;
  }

  (newLeftPath.Copy(path))->Insert(var, val, '<');
  (newRightPath.Copy(path))->Insert(var, val, '>');

  path->Del();
  ant->DelAnte();

  DeepSearch(data, std::make_shared<Rule>(newLeftPath), newListPat);
  DeepSearch(data, std::make_shared<Rule>(newRightPath), newListPat);

  newListPat->Del();
}

////////////////////////////////////////////////////////////////////////

int RealHyp::ComputeCorrect(
    std::shared_ptr<StringInt> listPatLeft,
    std::shared_ptr<StringInt> listPatRight)

{
  int p;
  int ind;
  int nbPat;
  int max;

  std::vector<int> classLeft(NbOut, 0);
  std::vector<int> classRight(NbOut, 0);

  int nbComp = NbOut * (NbOut - 1);
  std::vector<int> vectMax(nbComp);

  nbPat = listPatLeft->GetNbEl();

  for (p = 0, listPatLeft->GoToBeg(); p < nbPat; p++, listPatLeft->GoToNext())
    classLeft[ClassPatNet[listPatLeft->GetVal()]] += 1;

  nbPat = listPatRight->GetNbEl();

  for (p = 0, listPatRight->GoToBeg(); p < nbPat; p++, listPatRight->GoToNext())
    classRight[ClassPatNet[listPatRight->GetVal()]] += 1;

  for (p = 0, ind = 0; p < NbOut; p++) {
    for (int q = 0; q < NbOut; q++) {
      if (q == p)
        continue;

      vectMax[ind] = classLeft[p] + classRight[q];
      ind++;
    }
  }

  max = MaxOnPos(vectMax);

  return max;
}

////////////////////////////////////////////////////////////////////////

void RealHyp::SetCountPatDiscr2(DataSet &data, std::shared_ptr<Rule> r)

{
  int a;
  int t;
  int nbThres;
  static int nbCorrect;
  float thres;

  OneVarThresDescr *varDescr;
  std::shared_ptr<StringInt> newListPatLeft;
  std::shared_ptr<StringInt> newListPatRight;
  auto newLeftPath = std::make_shared<Rule>();
  auto newRightPath = std::make_shared<Rule>();

  Descr->ResetAllCountPatDiscr();
  int nbRuleAnt = r->GetNbAnt();

  for (int v = 0; v < NbIn; v++) {
    varDescr = Descr->GetDescr(v);
    nbThres = varDescr->GetNbThres();

    for (t = 0, varDescr->GoToBeg(); t < nbThres; t++, varDescr->GoToNext()) {
      thres = varDescr->GetThres();

      for (a = 0, r->GoToBeg(); a < nbRuleAnt; a++, r->GoToNext()) {
        if ((v == r->GetVar()) && (thres == r->GetVal()))
          break;
      }

      if (a != nbRuleAnt) {
        continue;
      }

      (newLeftPath->Copy(r))->Insert(v, thres, '<');  // to del
      (newRightPath->Copy(r))->Insert(v, thres, '>'); // to del

      newListPatLeft = data.Select(newLeftPath);   // to del
      newListPatRight = data.Select(newRightPath); // to del

      nbCorrect = ComputeCorrect(newListPatLeft, newListPatRight);
      varDescr->SetCountPat(nbCorrect);

      newLeftPath->Del();
      newRightPath->Del();

      newListPatLeft->Del();
      newListPatRight->Del();
    }
  }
}

////////////////////////////////////////////////////////////////////////

void RealHyp::DeepSearch2(DataSet &data, std::shared_ptr<Rule> path)

{
  Rule newLeftPath;
  Rule newRightPath;

  std::shared_ptr<StringInt> newListPat = data.Select(path);

  if (newListPat->GetNbEl() == 0) {
    newListPat->Del();
    path->Del();
    return;
  }

  if (AreSameClass(newListPat, ClassPatNet) == 1) {
    newListPat->Del();
    SaveRule(path);
    path->Del();

    return;
  }

  SetCountPatDiscr2(data, path);

  std::shared_ptr<Ante> ant = FindMostDiscrAnt(0);
  int var = ant->GetVarAnte();
  float val = ant->GetValAnte();

  if (var < 0) {
    Aborted = 1;
    ant->DelAnte();
    newListPat->Del();

    return;
  }

  (newLeftPath.Copy(path))->Insert(var, val, '<');
  (newRightPath.Copy(path))->Insert(var, val, '>');

  path->Del();
  ant->DelAnte();
  newListPat->Del();

  DeepSearch2(data, std::make_shared<Rule>(newLeftPath));
  DeepSearch2(data, std::make_shared<Rule>(newRightPath));
}

////////////////////////////////////////////////////////////////////////

void RealHyp::RuleExtraction(
    DataSet &data,
    const DataSet &train,
    const DataSet &trainClass,
    const DataSet &valid,
    const DataSet &validClass,
    const DataSet &test,
    const DataSet &testClass,
    const AttrName &attr,
    ostream &ruleFile)

{
  Rule empty;
  std::shared_ptr<StringInt> listAll;
  int nbAnt1;
  int nbAnt2;

  Aborted = 0;

  SavedRules = std::make_shared<RuleProcessing>(NbIn, NbHyp, data, ClassPatNet, Descr);

  cout << "*** BUILDING DECISION TREE ...\n"
       << endl;

  listAll = data.Select(std::make_shared<Rule>(empty));
  DeepSearch(data, std::make_shared<Rule>(empty), listAll);
  listAll->Del();
  if (Aborted) {
    cout << "*** TREE ABORTED !\n\n";
    cout << "*** TRYING AN ALTERNATIVE ALGORITHM \n"
         << endl;

    SavedRules->Del();
    Descr->Del();

    return;
  }

  SavedRules->Clean();

  if (SavedRules->CountAnt() == 0) {
    cout << "*** NO RULES !\n\n";
    cout << "--- Number of rules = 0\n";
    cout << "--- Number of antecedents = 0\n";
    cout << "--- Number of antecedents per rule = 0\n";
    cout << "--- Number of examples per rule = 0\n";

    cout << "*** NO RULES !\n\n";

    cout << "--- Number of rules = 0\n";
    cout << "--- Number of antecedents = 0\n";
    cout << "--- Number of antecedents per rule = 0\n";
    cout << "--- Number of examples per rule = 0" << endl;

    SavedRules->Del();

    Descr->Del();

    return;
  }
  cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n"
       << endl;
  SavedRules->MixPrune();

  cout << "\n\n*** EXPANDING RULES ...\n"
       << endl;
  SavedRules->EnlargeAndPrune();

  nbAnt1 = SavedRules->CountAnt();

  cout << "\n\n*** RETRYING RULE EXTRACTION ...\n"
       << endl;

  CleanRuleStruct clean(data, train, trainClass,
                        valid, validClass, test, testClass,
                        SavedRules, Bpnn, Out, NbOut);

  Descr->Del();
  SetConfirmedVirt2();
  SetRealHyp(data);

  SavedRules->Del();
  cout << "*** BUILDING DECISION TREE ...\n"
       << endl;

  listAll = data.Select(std::make_shared<Rule>(empty));
  DeepSearch(data, std::make_shared<Rule>(empty), listAll);
  listAll->Del();

  if (Aborted) {
    cout << "*** TREE ABORTED !\n\n";
    Aborted = 0;
    goto A;
  }

  SavedRules->Clean();

  cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n"
       << endl;
  SavedRules->MixPrune();

  cout << "\n\n*** EXPANDING RULES ...\n"
       << endl;
  SavedRules->EnlargeAndPrune();

  nbAnt2 = SavedRules->CountAnt();
  if (nbAnt2 < nbAnt1) {
    cout << "\n\n*** RULE SET IMPROVED BY " << nbAnt1 - nbAnt2;
    cout << " ANTECEDENTS" << endl;

    clean.Del();
    CleanRuleStruct clean2(data, train, trainClass,
                           valid, validClass, test, testClass,
                           SavedRules, Bpnn, Out, NbOut);
    cout << "\n\n*** WRITING IF-THEN RULES ...\n"
         << endl;

    if (attr.IsFileAttr()) {
      clean2.SetAttr(attr.GetListAttr());
      clean2.SetStrClass(attr.GetListClasses(), 0);
    } else {
      clean2.SetAttr();
      clean2.SetStrClass(0);
    }
    clean2.WriteRules(0, ruleFile);
  }

  else {
    cout << "\n\n*** RULE SET DOES NOT IMPROVE (" << nbAnt1 - nbAnt2 << ")\n";
  A:
    cout << "\n    (WRITING PREVIOUS ONE)\n";

    cout << "\n\n*** WRITING IF-THEN RULES ...\n\n";

    if (attr.IsFileAttr()) {
      clean.SetAttr(attr.GetListAttr());
      clean.SetStrClass(attr.GetListClasses(), 0);
    } else {
      clean.SetAttr();
      clean.SetStrClass(0);
    }

    clean.WriteRules(0, ruleFile);
  }
  SavedRules->Del();

  Descr->Del();
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp(
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers) : Bpnn(nn), NbBins(nbBins), NbIn(nbIn), Multiple(multiple), NbWeightLayers(nbWeightLayers)

{
  int p;

  NbHyp = (NbBins + 1) * Multiple;
  NbOut = (Bpnn->GetLayer(NbWeightLayers - 1))->GetNbUp();

  Bias = (Bpnn->GetLayer(0))->GetBias();
  Weights = (Bpnn->GetLayer(0))->GetWeights();
  Out = (Bpnn->GetLayer(NbWeightLayers - 1))->GetUp();

  Virt = std::make_shared<VirtualHyp>(NbBins, NbIn, Multiple, Bias, Weights);
  Descr = std::make_shared<ThresDescr>(NbIn);
  ClassPatNet.resize(data.GetNbEx());

  for (p = 0; p < data.GetNbEx(); p++) {
    Bpnn->ForwardOneExample1(data, p);
    ClassPatNet[p] = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  }

  ConfirmedVirt.resize(NbIn);
  ConfBefFirstHyp.resize(NbIn);

  for (p = 0; p < NbIn; p++)
    ConfirmedVirt[p].resize(NbHyp);

  SetConfirmedVirt(data);
  SetRealHyp(data);
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp(
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers,
    int /*dummy*/) : Bpnn(nn), NbBins(nbBins), NbIn(nbIn), Multiple(multiple), NbWeightLayers(nbWeightLayers)

{
  int p;

  NbHyp = (NbBins + 1) * Multiple;
  NbOut = (Bpnn->GetLayer(NbWeightLayers - 1))->GetNbUp();

  Bias = (Bpnn->GetLayer(0))->GetBias();
  Weights = (Bpnn->GetLayer(0))->GetWeights();
  Out = (Bpnn->GetLayer(NbWeightLayers - 1))->GetUp();

  Virt = std::make_shared<VirtualHyp>(NbBins, NbIn, Multiple, Bias, Weights);
  Descr = std::make_shared<ThresDescr>(NbIn);
  ClassPatNet.resize(data.GetNbEx());

  for (p = 0; p < data.GetNbEx(); p++) {
    Bpnn->ForwardOneExample1(data, p);
    ClassPatNet[p] = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  }

  ConfirmedVirt.resize(NbIn);
  ConfBefFirstHyp.resize(NbIn);

  for (p = 0; p < NbIn; p++)
    ConfirmedVirt[p].resize(NbHyp);
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp(
    std::shared_ptr<VirtualHyp> globalVirt,
    int nbNets,
    float *out,
    int nbOut,
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers) : Bpnn(nn), Virt(globalVirt), Out(out), NbBins(nbBins), NbIn(nbIn), Multiple(multiple), NbWeightLayers(nbWeightLayers), NbOut(nbOut)

{
  int p;

  NbHyp = (NbBins + 1) * Multiple * nbNets;

  Descr = std::make_shared<ThresDescr>(NbIn);
  ClassPatNet.resize(data.GetNbEx());

  for (p = 0; p < data.GetNbEx(); p++) {
    Bpnn->ForwardOneExample1(data, p);
    ClassPatNet[p] = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  }

  ConfirmedVirt.resize(NbIn);
  ConfBefFirstHyp.resize(NbIn);

  for (p = 0; p < NbIn; p++)
    ConfirmedVirt[p].resize(NbHyp);

  SetConfirmedVirt(data);
  SetRealHyp(data);
}

////////////////////////////////////////////////////////////////////////

RealHyp::RealHyp(
    std::shared_ptr<VirtualHyp> globalVirt,
    int nbNets,
    float *out,
    int nbOut,
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers,
    int /*dummy*/) : Bpnn(nn), Virt(globalVirt), Out(out), NbBins(nbBins), NbIn(nbIn), Multiple(multiple), NbWeightLayers(nbWeightLayers), NbOut(nbOut)

{
  int p;

  NbHyp = (NbBins + 1) * Multiple * nbNets;

  Descr = std::make_shared<ThresDescr>(NbIn);
  ClassPatNet.resize(data.GetNbEx());

  for (p = 0; p < data.GetNbEx(); p++) {
    Bpnn->ForwardOneExample1(data, p);
    ClassPatNet[p] = Bpnn->Max(std::vector<float>(Out, Out + NbOut));
  }

  ConfirmedVirt.resize(NbIn);
  ConfBefFirstHyp.resize(NbIn);

  for (p = 0; p < NbIn; p++)
    ConfirmedVirt[p].resize(NbHyp);
}

////////////////////////////////////////////////////////////////////////
