#include "realHyp.h"
#include "cleanRS.h"
#include "misc.h"

////////////////////////////////////////////////////////////////////////

/**
 * @brief Finds the maximum value in a vector of integers.
 *
 * @param vec The vector of integers.
 * @return The maximum value in the vector.
 */
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

/**
 * @brief Finds the index of the maximum value in an array of integers.
 *
 * @param vec The array of integers.
 * @param nbEl The number of elements in the array.
 * @return The index of the maximum value in the array.
 */
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

/**
 * @brief Sets confirmed virtual hyperplanes based on the dataset.
 *
 * @param data The dataset used for confirming virtual hyperplanes.
 */
void RealHyp::SetConfirmedVirt(DataSet &data)

{
  int nbPat = data.GetNbEx();
  int *hyp;
  int h;
  int v;
  int indVirt;
  const float *pat;

  std::cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES ...\n"
            << std::endl;

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
        std::cout << "*** ELSE WARNING !\n"
                  << std::endl;
    }
  }

  int count = 0;

  for (v = 0; v < NbIn; v++) {
    hyp = ConfirmedVirt[v].data();

    for (h = 0; h < NbHyp; h++, hyp++)
      if (*hyp == 1)
        count++;
  }

  std::cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n"
            << std::endl;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Sets the ConfirmedVirt array based on the saved rules.
 *
 * This function processes the saved rules to determine which virtual
 * hyperplanes are relevant and sets the corresponding entries in the
 * ConfirmedVirt array.
 */
void RealHyp::SetConfirmedVirt2()

{
  const int nbRules = SavedRules->GetNbRules();
  std::shared_ptr<DimlpRule> rule;
  int *hyp;
  int a;
  int h;
  int r;
  int v;
  int var;
  int indVirt;
  int nbAnt;
  float val;

  std::cout << "\n\n*** EXCLUDING UNRELEVANT HYPER-PLANES FROM RULES ...\n"
            << std::endl;

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
          std::cout << "Warning ... " << var << " " << val << "" << std::endl;
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

  std::cout << "*** RELEVANT VIRTUAL HYPER-PLANES = " << count << "\n"
            << std::endl;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Goes right in the decision tree to identify the discriminant hyperplane.
 *
 * @param var The variable index.
 * @param indPat The index of the pattern.
 * @param startVirt The starting index for virtual hyperplanes.
 * @param netAns The network's answer for the pattern.
 */
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

/**
 * @brief Adjusts the input variable to the right of a virtual hyperplane and checks for classification changes.
 *
 * This function iterates through the virtual hyperplanes to the right of a given starting index for a specific
 * variable. It adjusts the variable's value to each hyperplane's position, and after each adjustment, it forwards
 * the example through the neural network to check if the classification changes. If a change is detected, the
 * hyperplane is marked as relevant.
 *
 * @param var The variable index to adjust.
 * @param indPat The index of the pattern (example) being processed.
 * @param startVirt The starting index of the virtual hyperplane.
 * @param netAns The network's initial classification result.
 */
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

/**
 * @brief Goes left in the decision tree to identify the discriminant hyperplane.
 *
 * @param var The variable index.
 * @param indPat The index of the pattern.
 * @param startVirt The starting index for virtual hyperplanes.
 * @param netAns The network's answer for the pattern.
 */
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

/**
 * @brief Adjusts the input variable to the left of a virtual hyperplane and checks for classification changes.
 *
 * This function iterates through the virtual hyperplanes to the left of a given starting index for a specific
 * variable. It adjusts the variable's value to each hyperplane's position, and after each adjustment, it forwards
 * the example through the neural network to check if the classification changes. If a change is detected, the
 * hyperplane is marked as relevant.
 *
 * @param var The variable index to adjust.
 * @param indPat The index of the pattern (example) being processed.
 * @param startVirt The starting index of the virtual hyperplane.
 * @param netAns The network's initial classification result.
 */
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

/**
 * @brief Processes a single example to estimate the relevance of discriminant hyperplanes.
 *
 * @param data The dataset containing the example.
 * @param indPat The index of the pattern in the dataset.
 */
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

/**
 * @brief Sets real hyperplanes based on the dataset.
 *
 * @param data The dataset used for setting real hyperplanes.
 */
void RealHyp::SetRealHyp(DataSet &data)

{
  int p;
  int modulo;
  const int nbEx = data.GetNbEx();

  std::cout << "*** ESTIMATING RELEVANCE OF DISCRIMINANT HYPER-PLANES ";
  std::cout << "WITH AVAILABLE EXAMPLES ...\n"
            << std::endl;

  if (nbEx >= 10)
    modulo = nbEx / 10;
  else
    modulo = 1;

  for (p = 0; p < nbEx; p++) {
    if (p % modulo == 0)
      std::cout << p << std::endl;

    OneExRealHyp(data, p);
  }

  std::cout << p << "\n\n"
            << std::endl;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Sets the count of patterns that are discriminated by a given rule.
 *
 * @param listPat The list of patterns.
 * @param r The rule being evaluated.
 */
void RealHyp::SetCountPatDiscr(std::shared_ptr<StringInt> listPat, std::shared_ptr<DimlpRule> r) const

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

/**
 * @brief Finds the most discriminant antecedent.
 *
 * @param sel The selection threshold for discriminant antecedents.
 * @return A shared pointer to the most discriminant antecedent.
 */
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

/**
 * @brief Recursively performs a deep search for rules in the dataset.
 *
 * @param data The dataset being processed.
 * @param path The current rule path.
 * @param subSet The subset of patterns being evaluated.
 */
void RealHyp::DeepSearch(DataSet &data, std::shared_ptr<DimlpRule> path, std::shared_ptr<StringInt> subSet)

{
  DimlpRule newLeftPath;
  DimlpRule newRightPath;
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

  DeepSearch(data, std::make_shared<DimlpRule>(newLeftPath), newListPat);
  DeepSearch(data, std::make_shared<DimlpRule>(newRightPath), newListPat);

  newListPat->Del();
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Computes the number of correct classifications for a given rule.
 *
 * @param listPatLeft The list of patterns classified to the left.
 * @param listPatRight The list of patterns classified to the right.
 * @return The number of correct classifications.
 */
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

/**
 * @brief Sets the count of patterns that are discriminated by a given rule.
 *
 * This function is a variant that processes the dataset directly.
 *
 * @param data The dataset being processed.
 * @param r The rule being evaluated.
 */
void RealHyp::SetCountPatDiscr2(DataSet &data, std::shared_ptr<DimlpRule> r)

{
  int a;
  int t;
  int nbThres;
  static int nbCorrect;
  float thres;

  OneVarThresDescr *varDescr;
  std::shared_ptr<StringInt> newListPatLeft;
  std::shared_ptr<StringInt> newListPatRight;
  auto newLeftPath = std::make_shared<DimlpRule>();
  auto newRightPath = std::make_shared<DimlpRule>();

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

/**
 * @brief Recursively performs a deep search for rules in the dataset.
 *
 * This function is a variant that processes the entire dataset.
 *
 * @param data The dataset being processed.
 * @param path The current rule path.
 */
void RealHyp::DeepSearch2(DataSet &data, std::shared_ptr<DimlpRule> path)

{
  DimlpRule newLeftPath;
  DimlpRule newRightPath;

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

  DeepSearch2(data, std::make_shared<DimlpRule>(newLeftPath));
  DeepSearch2(data, std::make_shared<DimlpRule>(newRightPath));
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Extracts rules from the dataset and writes them to the provided output stream.
 *
 * This method performs the rule extraction process using the specified datasets. It builds a decision tree,
 * prunes the rules, and then writes the extracted rules in an IF-THEN format to the provided output stream.
 * The method can optionally denormalize the values using the provided means, standard deviations, and normalization indices.
 *
 * The process involves the following steps:
 * 1. Build an initial decision tree from the dataset.
 * 2. Prune antecedents and rules to simplify the rule set.
 * 3. Optionally retry rule extraction with expanded rules.
 * 4. Write the final set of rules to the output stream.
 *
 * @param data The dataset used for rule extraction.
 * @param train Training dataset.
 * @param trainClass Training dataset classes.
 * @param valid Validation dataset.
 * @param validClass Validation dataset classes.
 * @param test Testing dataset.
 * @param testClass Testing dataset classes.
 * @param attr Attribute names.
 * @param ruleFile Output stream for writing rules.
 * @param mus Means for normalization (optional).
 * @param sigmas Standard deviations for normalization (optional).
 * @param normalizationIndices Indices for normalization (optional).
 */
void RealHyp::RuleExtraction(
    DataSet &data,
    const DataSet &train,
    const DataSet &trainClass,
    const DataSet &valid,
    const DataSet &validClass,
    const DataSet &test,
    const DataSet &testClass,
    const AttrName &attr,
    std::ostream &ruleFile,
    const std::vector<double> &mus,
    const std::vector<double> &sigmas,
    const std::vector<int> &normalizationIndices)

{
  DimlpRule empty;
  std::shared_ptr<StringInt> listAll;
  int nbAnt1;
  int nbAnt2;

  Aborted = 0;

  SavedRules = std::make_shared<RuleProcessing>(NbIn, NbHyp, data, ClassPatNet, Descr);
  std::cout << "*** BUILDING DECISION TREE ...\n"
            << std::endl;

  listAll = data.Select(std::make_shared<DimlpRule>(empty));
  DeepSearch(data, std::make_shared<DimlpRule>(empty), listAll);
  listAll->Del();
  if (Aborted) {
    std::cout << "*** TREE ABORTED !\n"
              << std::endl;
    std::cout << "*** TRYING AN ALTERNATIVE ALGORITHM \n"
              << std::endl;

    SavedRules->Del();
    Descr->Del();

    return;
  }

  SavedRules->Clean();
  std::cout << "4" << std::endl;
  if (SavedRules->CountAnt() == 0) {
    std::cout << "*** NO RULES !\n"
              << std::endl;
    std::cout << "--- Number of rules = 0" << std::endl;
    std::cout << "--- Number of antecedents = 0" << std::endl;
    std::cout << "--- Number of antecedents per rule = 0" << std::endl;
    std::cout << "--- Number of examples per rule = 0" << std::endl;

    std::cout << "*** NO RULES !\n"
              << std::endl;

    std::cout << "--- Number of rules = 0" << std::endl;
    std::cout << "--- Number of antecedents = 0" << std::endl;
    std::cout << "--- Number of antecedents per rule = 0" << std::endl;
    std::cout << "--- Number of examples per rule = 0" << std::endl;

    SavedRules->Del();

    Descr->Del();

    return;
  }
  std::cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n"
            << std::endl;
  SavedRules->MixPrune();

  std::cout << "\n\n*** EXPANDING RULES ...\n"
            << std::endl;
  SavedRules->EnlargeAndPrune();

  nbAnt1 = SavedRules->CountAnt();

  std::cout << "\n\n*** RETRYING RULE EXTRACTION ...\n"
            << std::endl;

  CleanRuleStruct clean(data, train, trainClass,
                        valid, validClass, test, testClass,
                        SavedRules, Bpnn, Out, NbOut);

  Descr->Del();
  SetConfirmedVirt2();
  SetRealHyp(data);

  SavedRules->Del();
  std::cout << "*** BUILDING DECISION TREE ...\n"
            << std::endl;

  listAll = data.Select(std::make_shared<DimlpRule>(empty));
  DeepSearch(data, std::make_shared<DimlpRule>(empty), listAll);
  listAll->Del();

  if (Aborted) {
    std::cout << "*** TREE ABORTED !\n"
              << std::endl;
    Aborted = 0;
    goto A;
  }

  SavedRules->Clean();

  std::cout << "\n\n*** PRUNING ANTECEDENTS AND PRUNING RULES ...\n"
            << std::endl;
  SavedRules->MixPrune();

  std::cout << "\n\n*** EXPANDING RULES ...\n"
            << std::endl;
  SavedRules->EnlargeAndPrune();

  nbAnt2 = SavedRules->CountAnt();
  if (nbAnt2 < nbAnt1) {
    std::cout << "\n\n*** RULE SET IMPROVED BY " << nbAnt1 - nbAnt2;
    std::cout << " ANTECEDENTS" << std::endl;

    clean.Del();
    CleanRuleStruct clean2(data, train, trainClass,
                           valid, validClass, test, testClass,
                           SavedRules, Bpnn, Out, NbOut);
    std::cout << "\n\n*** WRITING IF-THEN RULES ...\n"
              << std::endl;

    if (attr.IsFileAttr()) {
      clean2.SetAttr(attr.GetListAttr());
      clean2.SetStrClass(attr.GetListClasses(), 0);
    } else {
      clean2.SetAttr();
      clean2.SetStrClass(0);
    }
    if (mus.empty()) {
      clean2.WriteRules(0, ruleFile);
    } else {
      clean2.WriteRules(0, ruleFile, mus, sigmas, normalizationIndices);
    }
  }

  else {
    std::cout << "\n\n*** RULE SET DOES NOT IMPROVE (" << nbAnt1 - nbAnt2 << ")" << std::endl;
  A:
    std::cout << "\n    (WRITING PREVIOUS ONE)" << std::endl;

    std::cout << "\n\n*** WRITING IF-THEN RULES ...\n"
              << std::endl;

    if (attr.IsFileAttr()) {
      clean.SetAttr(attr.GetListAttr());
      clean.SetStrClass(attr.GetListClasses(), 0);
    } else {
      clean.SetAttr();
      clean.SetStrClass(0);
    }

    if (mus.empty()) {
      clean.WriteRules(0, ruleFile);
    } else {
      clean.WriteRules(0, ruleFile, mus, sigmas, normalizationIndices);
    }
  }
  SavedRules->Del();

  Descr->Del();
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a RealHyp object and sets up hyperplanes based on the provided dataset and neural network.
 *
 * @param data The dataset used for rule extraction.
 * @param nn The neural network used for rule extraction.
 * @param nbBins The number of bins for virtual hyperplanes.
 * @param nbIn The number of input variables.
 * @param multiple The multiplicity factor for hyperplanes.
 * @param nbWeightLayers The number of layers in the neural network.
 */
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

/**
 * @brief Constructs a RealHyp object with additional parameters for hyperplanes.
 *
 * @param data The dataset used for rule extraction.
 * @param nn The neural network used for rule extraction.
 * @param nbBins The number of bins for virtual hyperplanes.
 * @param nbIn The number of input variables.
 * @param multiple The multiplicity factor for hyperplanes.
 * @param nbWeightLayers The number of layers in the neural network.
 * @param dummy A dummy parameter for distinguishing constructors.
 */
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
/**
 * @brief Constructs a RealHyp object with a shared virtual hyperplane representation.
 *
 * @param globalVirt Shared virtual hyperplane representation.
 * @param nbNets Number of neural networks.
 * @param out Output values from the neural network.
 * @param nbOut Number of output classes.
 * @param data The dataset used for rule extraction.
 * @param nn The neural network used for rule extraction.
 * @param nbBins The number of bins for virtual hyperplanes.
 * @param nbIn The number of input variables.
 * @param multiple The multiplicity factor for hyperplanes.
 * @param nbWeightLayers The number of layers in the neural network.
 */
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

/**
 * @brief Constructs a RealHyp object with additional parameters for hyperplanes and a shared virtual hyperplane representation.
 *
 * @param globalVirt Shared virtual hyperplane representation.
 * @param nbNets Number of neural networks.
 * @param out Output values from the neural network.
 * @param nbOut Number of output classes.
 * @param data The dataset used for rule extraction.
 * @param nn The neural network used for rule extraction.
 * @param nbBins The number of bins for virtual hyperplanes.
 * @param nbIn The number of input variables.
 * @param multiple The multiplicity factor for hyperplanes.
 * @param nbWeightLayers The number of layers in the neural network.
 * @param dummy A dummy parameter for distinguishing constructors.
 */
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
