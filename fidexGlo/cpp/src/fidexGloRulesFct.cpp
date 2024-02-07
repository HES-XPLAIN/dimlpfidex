#include "fidexGloRulesFct.h"

/**
 * @brief shows all program arguments and details into the terminal.
 *
 */
void showRulesParams() {
  cout << "\n-------------------------------------------------\n"
       << endl;

  cout << "Obligatory parameters : \n"
       << endl;
  cout << "fidexGloRules --train_data_file <train data file> --train_pred_file <train prediction file> --train_class_file <train true class file, not mendatory if classes are specified in train data file> ";
  cout << "--weights_file <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] ";
  cout << "--rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  cout << "--global_rules_outfile <Rules output file> ";
  cout << "--heuristic <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
  cout << "--nb_attributes <number of attributes> ";
  cout << "--nb_classes <number of classes> ";
  cout << "<Options>\n"
       << endl;

  cout << "Options are: \n"
       << endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << endl;
  cout << "--nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << endl;
  cout << "--attributes_file <file of attributes>" << endl;
  cout << "--console_file <file where you redirect console result>" << endl; // If we want to redirect console result to file
  cout << "--max_itertions <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)>" << endl;
  cout << "--min_covering <minimum covering number (2 by default)>" << endl;
  cout << "--dropout_dim <dimension dropout parameter>" << endl;
  cout << "--dropout_hyp <hyperplan dropout parameter>" << endl;
  cout << "--max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>" << endl;
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << endl;
  cout << "--decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << endl;
  cout << "--positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << endl;
  cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << endl;
  cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << endl;
  cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << endl;
  cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << endl;
  cout << "--nb_threads <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)>" << endl;
  cout << "--min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)>" << endl;
  cout << "--seed <seed (0=random, default)>";

  cout << "\n-------------------------------------------------\n"
       << endl;
}

/**
 * @brief executes fidex algorithm on all provided samples in the following way:
 *
 *    - Initialize the number of threads chosen by the user and splits evenly all samples to be covered between them.
 *    - Computes fidex until a rule is created or until the max failed attempts limit is reached
 *      -> For each failed attempt, the current minimal covering to be reached is lowered until the limit of 2 is reached.
 *      -> Each rule that have been found with a covering that is lower than the minimal covering setted by the used are counted.
 *      -> If the max failed attempts limit is reached, then the sample computed is removed from the not covered samples vector as it's considered as unreachable
 *    - The computed rule is added to the list of rules
 *
 *
 * @param rules Empty vector of rules to be filled once the function has finished
 * @param notCoveredSamples vector with all samples to be covered or ignored if the algorithm isn't able to find one in "maxFailedAttempts" attempts.
 * @param trainDataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 */
// TODO: implement dicotomic min covering
void generateRules(vector<Rule> &rules, vector<int> &notCoveredSamples, DataSetFid &trainDataset, Parameters &p, const vector<vector<double>> &hyperlocus) {
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int nbDatas = trainDataset.getNbSamples();
  int minCovering = p.getInt(MIN_COVERING);
  int nbThreadsUsed = p.getInt(NB_THREADS);

#pragma omp parallel num_threads(nbThreadsUsed)
  {
    Rule rule;
    double t1;
    double t2;
    int cnt = 0;
    bool ruleCreated;
    int counterFailed;
    int localNbProblems = 0;
    vector<Rule> localRules;
    vector<int>::iterator it;
    int localNbRulesNotFound = 0;
    Hyperspace hyperspace(hyperlocus);
    int currentMinCovering = minCovering;
    int threadId = omp_get_thread_num();
    float minFidelity = p.getFloat(MIN_FIDELITY);
    auto fidex = Fidex(trainDataset, p, hyperspace);

    string consoleFile = "";
    if (p.isStringSet(CONSOLE_FILE)) {
      consoleFile = p.getString(CONSOLE_FILE);
    }

    int maxFailedAttempts = p.getInt(MAX_FAILED_ATTEMPTS);

#pragma omp critical
    {
      cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;
    }

    t1 = omp_get_wtime();

#pragma omp for
    for (int idSample = 0; idSample < nbDatas; idSample++) {
      vector<int> &trainPreds = trainDataset.getPredictions();
      vector<vector<double>> &trainData = trainDataset.getDatas();
      vector<double> &mainSampleValues = trainData[idSample];
      int mainSamplePred = trainPreds[idSample];
      ruleCreated = false;
      counterFailed = 0; // If we can't find a good rule after a lot of tries
      cnt += 1;

      if (omp_get_thread_num() == 0 && ((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && consoleFile.empty()) {
#pragma omp critical
        {
          cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
          cout.flush();
        }
      }

      while (!ruleCreated) {
        ruleCreated = fidex.compute(rule, mainSampleValues, mainSamplePred, minFidelity, currentMinCovering);
        if (currentMinCovering >= 2) {
          currentMinCovering -= 1;
        } else {
          counterFailed += 1;
        }

        if (currentMinCovering < 2) {
          counterFailed += 1;
        }

        if (counterFailed >= maxFailedAttempts) {
          localNbRulesNotFound += 1;

#pragma omp critical
          {
            // ignore samples that cannot be covered after "maxFailedAttempts" attempts
            it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
            if (it != notCoveredSamples.end()) {
              notCoveredSamples.erase(it);
            }
          }
          break;
        }
      }

      if ((currentMinCovering + 1) < minCovering) {
        localNbProblems += 1;
      }

      if (ruleCreated) {
        localRules.push_back(rule);
      }
    }

    t2 = omp_get_wtime();

    // gathering local data to main process
#pragma omp for ordered
    for (int i = 0; i < nbThreadsUsed; i++) {
#pragma omp ordered
      {
        if (i == threadId) {
          cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << endl;
          rules.insert(rules.end(), localRules.begin(), localRules.end());
          nbProblems += localNbProblems;
          nbRulesNotFound += localNbRulesNotFound;
        }
      }
    }
  } // end of parallel section

  cout << endl
       << rules.size() << " rules created." << endl
       << "Number of sample with lower covering than " << minCovering << " is " << nbProblems << endl
       << "Number of rules not found is " << nbRulesNotFound << endl
       << "Fidex rules computed" << endl;
}

/**
 * @brief Filters the list of generated rules to remove duplicates and keep the rules that best fits and covers not covered samples.
 * This is performed in the following way:
 *
 *    - Duplicated rules are removed
 *    - While all samples are not covered, it checks every rule's covered samples vector and checks how many of the not covered ones are present in it.
 *    - If the rule covers samples that are not considered covered yet and covers more than the rule that is considered being the best, it's saved as the
 *      best rule to be added to the filtered ones.
 *    - When all the rules are checked, the best one is removed from the rules to check, it is saved in the chosen rules vector and the samples
 *      it covers are removed from the ones that are considered not covered.
 *
 *
 *
 * @param trainDataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 * @return vector<Rule>
 */
vector<Rule> heuristic_1(DataSetFid &trainDataset, Parameters &p, const vector<vector<double>> &hyperlocus) {
  vector<Rule> rules;
  vector<Rule> chosenRules;
  int nbDatas = trainDataset.getNbSamples();
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1
  generateRules(rules, notCoveredSamples, trainDataset, p, hyperlocus);

  cout << "Computing global ruleset..." << endl;

  // remove duplicates
  rules.erase(unique(rules.begin(), rules.end()), rules.end());

  // While there is some not covered samples
  vector<int>::iterator ite;
  vector<int> currentRuleSamples;

  while (!notCoveredSamples.empty()) {
    Rule bestRule;
    int bestRuleIndex = 0;
    int bestCovering = INT_MAX;
    vector<int> remainingSamples;
    vector<int> difference(notCoveredSamples.size());

    for (int i = 0; i < rules.size(); i++) {
      currentRuleSamples = rules[i].getCoveredSamples();

      ite = set_difference(notCoveredSamples.begin(),
                           notCoveredSamples.end(),
                           currentRuleSamples.begin(),
                           currentRuleSamples.end(),
                           difference.begin()); // vectors have to be sorted

      difference.resize(ite - difference.begin());

      if (difference.size() < bestCovering) {
        bestRuleIndex = i;
        bestRule = rules[i];
        remainingSamples = difference;
        bestCovering = static_cast<int>(difference.size());
      }
    }

    chosenRules.push_back(bestRule); // add best rule with maximum covering
    notCoveredSamples = remainingSamples;
    rules.erase(rules.begin() + bestRuleIndex); // Remove this rule
  }

  cout << chosenRules.size() << " rules selected." << endl;

  return chosenRules;
}

/**
 * @brief Faster version of the first heuristic, only difference is that it doesn't check every rule before choosing the best one.
 * It directly selects if one or more not covered sample is covered.
 *
 *
 * @param trainDataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 * @return vector<Rule>
 */
vector<Rule> heuristic_2(DataSetFid &trainDataset, Parameters &p, const vector<vector<double>> &hyperlocus) {
  vector<Rule> rules;
  vector<Rule> chosenRules;
  int nbDatas = trainDataset.getNbSamples();
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

  // getting rules and not covered samples
  generateRules(rules, notCoveredSamples, trainDataset, p, hyperlocus);

  // sort rules by covering
  sort(rules.begin(), rules.end(), [](const Rule &r1, const Rule &r2) {
    return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
  });

  // remove duplicates
  rules.erase(unique(rules.begin(), rules.end()), rules.end());

  // While there is some not covered samples
  int i = 0;
  vector<int>::iterator ite;
  vector<int> currentRuleSamples;

  while (!notCoveredSamples.empty()) {
    vector<int> difference(notCoveredSamples.size());
    currentRuleSamples = rules[i].getCoveredSamples();

    // Delete covered samples
    ite = set_difference(notCoveredSamples.begin(),
                         notCoveredSamples.end(),
                         currentRuleSamples.begin(),
                         currentRuleSamples.end(),
                         difference.begin()); // vectors have to be sorted

    difference.resize(ite - difference.begin());

    // If the rule covers a new sample
    if (difference.size() < notCoveredSamples.size()) {
      chosenRules.push_back(rules[i]); // add best rule with maximum covering
    }
    notCoveredSamples = difference;
    i += 1;
  }

  cout << chosenRules.size() << " rules selected." << endl;

  return chosenRules;
}

/**
 * @brief this heuristic generate rules for every sample and
 *
 * @param trainDataset
 * @param p
 * @param hyperlocus
 * @return vector<Rule>
 */
vector<Rule> heuristic_3(DataSetFid &trainDataset, Parameters &p, const vector<vector<double>> &hyperlocus) {
  Rule rule;
  int idSample;
  bool ruleCreated;
  int nbProblems = 0;
  int currentMinNbCov;
  int nbRulesNotFound = 0;
  vector<Rule> chosenRules;
  int seed = p.getInt(SEED);
  vector<int> chosenRuleSamples;
  Hyperspace hyperspace(hyperlocus);
  int minNbCover = p.getInt(MIN_COVERING);
  auto nbDatas = static_cast<int>(trainDataset.getDatas().size());
  vector<int> notCoveredSamples(nbDatas);
  float minFidelity = p.getFloat(MIN_FIDELITY);
  auto fidex = Fidex(trainDataset, p, hyperspace);
  string consoleFile = "";
  if (p.isStringSet(CONSOLE_FILE)) {
    consoleFile = p.getString(CONSOLE_FILE);
  }
  int maxFailedAttempts = p.getInt(MAX_FAILED_ATTEMPTS);

  if (seed == 0) {
    auto currentTime = high_resolution_clock::now();
    auto seedValue = currentTime.time_since_epoch().count();
    seed = static_cast<int>(seedValue);
  }

  mt19937 gen(seed);

  iota(begin(notCoveredSamples), end(notCoveredSamples), 0);      // Vector from 0 to nbDatas-1
  shuffle(begin(notCoveredSamples), end(notCoveredSamples), gen); //  Sort data randomly

  cout << "Computing rules..." << endl
       << endl;

  // While there is some not covered samples
  while (!notCoveredSamples.empty()) {

    if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && consoleFile.empty()) {
      cout << "Processing: " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%\r";
      cout.flush();
    }

    idSample = notCoveredSamples[0];
    vector<int> &trainPreds = trainDataset.getPredictions();
    vector<vector<double>> &trainData = trainDataset.getDatas();
    vector<double> &mainSampleValues = trainData[idSample];
    int mainSamplePred = trainPreds[idSample];
    currentMinNbCov = minNbCover;
    ruleCreated = false;
    int counterFailed = 0; // If we can't find a good rule after a lot of tries

    while (!ruleCreated) {
      ruleCreated = fidex.compute(rule, mainSampleValues, mainSamplePred, minFidelity, currentMinNbCov);

      if (currentMinNbCov >= 2) {
        currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
      } else {
        counterFailed += 1;
      }
      if (counterFailed >= maxFailedAttempts) {
        nbRulesNotFound += 1;
        break;
      }
      if (currentMinNbCov + 1 < minNbCover) {
        nbProblems += 1;
      }
    }

    if (ruleCreated) {
      chosenRules.push_back(rule); // We add the new rule
      chosenRuleSamples = rule.getCoveredSamples();

      notCoveredSamples.erase(
          remove_if(notCoveredSamples.begin(), notCoveredSamples.end(), [&chosenRuleSamples](int sample) {
            return find(chosenRuleSamples.begin(), chosenRuleSamples.end(), sample) != chosenRuleSamples.end();
            // find index of coveredSamples which is "sample" ("sample" is element of notCoveredSamples), find returns last if "sample" not found
            // -> Removes "sample" if it appears on coveredSamples (found before the end of coveredSamples)
          }),
          notCoveredSamples.end());
    } else {
      notCoveredSamples.erase(notCoveredSamples.begin());
    }
  }

  cout << endl;

  // remove duplicates
  chosenRules.erase(unique(chosenRules.begin(), chosenRules.end()), chosenRules.end());

  cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl
       << "Number of rules not found : " << nbRulesNotFound << endl
       << chosenRules.size() << " rules selected." << endl;

  return chosenRules;
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkRulesParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultInt(NB_DIMLP_NETS, 1);
  p.setDefaultInt(MAX_ITERATIONS, 10);
  p.setDefaultInt(MIN_COVERING, 2);
  p.setDefaultFloat(DROPOUT_DIM, 0.0f);
  p.setDefaultFloat(DROPOUT_HYP, 0.0f);
  p.setDefaultInt(MAX_FAILED_ATTEMPTS, 30);
  p.setDefaultInt(NB_QUANT_LEVELS, 50);
  p.setDefaultFloat(DECISION_THRESHOLD, -1.0f);
  p.setDefaultInt(POSITIVE_CLASS_INDEX, -1);
  p.setDefaultInt(NB_THREADS, 1);
  p.setDefaultFloat(MIN_FIDELITY, 1.0f);
  p.setDefaultFloat(HI_KNOT, 5.0f);
  p.setDefaultInt(SEED, 0);
  if (p.isStringSet(WEIGHTS_FILE)) {
    p.setWeightsFiles(); // must be called to initialize
  }

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(TRAIN_PRED_FILE);
  p.assertStringExists(GLOBAL_RULES_OUTFILE);
  p.assertIntExists(HEURISTIC);

  // verifying logic between parameters, values range and so on...

  if (p.getInt(NB_ATTRIBUTES) < 1) {
    throw CommandArgumentException("Error : Number of attributes must be strictly positive (>=1).");
  }

  if (p.getInt(NB_CLASSES) < 1) {
    throw CommandArgumentException("Error : Number of classes must be strictly positive (>=1).");
  }

  if (p.isStringSet(WEIGHTS_FILE) && p.isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : Do not specify both a weight file and an rules input file. Choose one of them.");
  }

  if (!p.isStringSet(WEIGHTS_FILE) && !p.isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
  }

  if (p.getInt(NB_THREADS) < 1 || p.getInt(NB_THREADS) > omp_get_max_threads()) {
    throw CommandArgumentException("Error : Number threads must be between 1 and #CPU cores of your machine (which is " + to_string(omp_get_max_threads()) + ")");
  }

  if (p.getInt(NB_QUANT_LEVELS) < 1) {
    throw CommandArgumentException("Error : Number of stairs in staircase activation function must be strictly positive (>=1).");
  }

  if (p.getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of networks must be strictly positive (>=1).");
  }

  if (!(p.getInt(HEURISTIC) > 0 && p.getInt(HEURISTIC) < 4)) {
    throw CommandArgumentException("Error : Heuristic must be 1, 2 or 3.");
  }

  if (p.getInt(MAX_ITERATIONS) < 1) {
    throw CommandArgumentException("Error : Maximum number of iterations must be strictly positive (>=1).");
  }

  if (p.getInt(MIN_COVERING) < 1) {
    throw CommandArgumentException("Error : Minimium covering must be strictly positive (>=1).");
  }

  if (p.getFloat(DROPOUT_DIM) < 0.0f || p.getFloat(DROPOUT_DIM) > 1.0f) {
    throw CommandArgumentException("Error : Dropout dim must be between [0.0, 1.0].");
  }

  if (p.getFloat(DROPOUT_HYP) < 0.0f || p.getFloat(DROPOUT_HYP) > 1.0f) {
    throw CommandArgumentException("Error : Dropout hyp must be between [0.0, 1.0].");
  }

  if (p.getInt(MAX_FAILED_ATTEMPTS) < 0) {
    throw CommandArgumentException("Error : Maximum number of failed attempts has to be positive (>=0)");
  }

  if (p.getFloat(MIN_FIDELITY) < 0.0f || p.getFloat(MIN_FIDELITY) > 1.0f) {
    throw CommandArgumentException("Error : Minimum fidelity has to be between [0.0, 1.0]");
  }

  if ((p.getFloat(DECISION_THRESHOLD) < 0.0f || p.getFloat(DECISION_THRESHOLD) > 1.0f) && p.getFloat(DECISION_THRESHOLD) != -1.0f) {
    throw CommandArgumentException("Error : Decision threshold must be between [0.0, 1.0].");
  }

  if (p.getInt(POSITIVE_CLASS_INDEX) < 0 && p.getInt(POSITIVE_CLASS_INDEX) != -1) {
    throw CommandArgumentException("Error : Positive class index must be positive (>=0)");
  }

  if (p.getInt(POSITIVE_CLASS_INDEX) >= p.getInt(NB_CLASSES)) {
    throw CommandArgumentException("Error : The index of positive class cannot be greater or equal to the number of classes (" + to_string(p.getInt(NB_CLASSES)) + ").");
  }

  if (p.getFloat(DECISION_THRESHOLD) != -1 && p.getInt(POSITIVE_CLASS_INDEX) == -1) {
    throw CommandArgumentException("Error : The positive class index has to be given with option --positive_class_index if the decision threshold is given (--decision_threshold)");
  }

  if (p.getInt(SEED) < 0) {
    throw CommandArgumentException("Error : Minimium covering must be null or positive (>=0).");
  }

  // ----------------------------------------------------------------------

  // Check denormalization parameters

  // If normalizationIndices were not specified, it's all attributes
  if (!p.isStringSet(NORMALIZATION_FILE) && !p.isIntVectorSet(NORMALIZATION_INDICES) && p.isDoubleVectorSet(MUS)) {
    vector<int> normalizationIndicesTemp;
    for (int i = 0; i < p.getInt(NB_ATTRIBUTES); ++i) {
      normalizationIndicesTemp.push_back(i);
    }
    p.setIntVector(NORMALIZATION_INDICES, normalizationIndicesTemp);
  }

  // Check if mus and sigmas are both given or both not
  if ((p.isDoubleVectorSet(MUS) || p.isDoubleVectorSet(SIGMAS)) &&
      !(p.isDoubleVectorSet(MUS) && p.isDoubleVectorSet(SIGMAS))) {
    throw CommandArgumentException("Error : One of Mus(--mus) and sigmas(--sigmas) is given but not the other.");
  }

  if (p.isStringSet(NORMALIZATION_FILE) && p.isDoubleVectorSet(MUS) || p.isStringSet(NORMALIZATION_FILE) && p.isIntVectorSet(NORMALIZATION_INDICES)) {
    throw CommandArgumentException("Error : normlization file (--normalization_file) and mus or normalizationIndices (--normalization_indices) are both given.");
  }

  // Mus, sigmas and normalizationIndices must have the same size and not be empty
  if (p.isDoubleVectorSet(MUS) && (p.getDoubleVector(MUS).size() != p.getDoubleVector(SIGMAS).size() || p.getDoubleVector(MUS).size() != p.getIntVector(NORMALIZATION_INDICES).size() || p.getDoubleVector(MUS).empty())) {
    throw CommandArgumentException("Error : mus (--mus), sigmas (--sigmas) and normalization indices (--normalization_indices) don't have the same size or are empty.");
  }

  // Check normalizationIndices
  if (p.isIntVectorSet(NORMALIZATION_INDICES)) {
    vector<int> tempVect = p.getIntVector(NORMALIZATION_INDICES);
    std::set<int> uniqueIndices(tempVect.begin(), tempVect.end());
    if (uniqueIndices.size() != p.getIntVector(NORMALIZATION_INDICES).size() ||
        *std::max_element(uniqueIndices.begin(), uniqueIndices.end()) >= p.getInt(NB_ATTRIBUTES) ||
        *std::min_element(uniqueIndices.begin(), uniqueIndices.end()) < 0) {
      throw CommandArgumentException("Error : parameter normalization indices (--normalization_indices) must be a list composed of integers between [0, nb_attributes-1] without repeted elements.");
    }
  }
}

/**
 * @brief writes a list of rules into a given file. Returns tuple of two integers representing the mean covering size and the mean number of antecedants.
 *
 * @param filename name of the file to be written/overwritten.
 * @param rules list of Rules object to be written in "filename".
 * @param hasConfidence whether or not Rule's confidence is written in file.
 * @param attributes list of the attributes names, used to write Rules's Antecedants with attributes explicit name instead of a "X" variable.
 * @param classes list of class names, used to write Rule's class with class explicit name instead of its numerical representation.
 * @return tuple<double, double>
 */
tuple<double, double> writeRulesFile(const string &filename, const vector<Rule> &rules, const vector<string> &attributes, const vector<string> &classes) {
  if (rules.empty()) {
    cout << "Warning: cannot write to file \"" << filename << "\", generated rules list is empty.";
    return make_tuple(0, 0);
  }

  int counter = 1;
  auto nbRules = static_cast<int>(rules.size());
  double meanCovSize = 0;
  double meanNbAntecedents = 0;
  stringstream stream;
  ofstream file(filename);

  for (Rule r : rules) { // each rule
    meanCovSize += static_cast<double>(r.getCoveredSamples().size());
    meanNbAntecedents += static_cast<double>(r.getAntecedants().size());
    counter++;
    stream << "Rule " << counter << ": " << r.toString(attributes, classes);
    stream << endl;
  }

  meanCovSize /= nbRules;
  meanNbAntecedents /= nbRules;

  if (file.is_open()) {
    file << "Number of rules : " << nbRules
         << ", mean sample covering number per rule : " << formattingDoubleToString(meanCovSize)
         << ", mean number of antecedents per rule : " << formattingDoubleToString(meanNbAntecedents)
         << endl;

    if (!attributes.empty()) {
      file << "Attribute names are specified. ";
    } else {
      file << "Attribute names are not specified. "; // TODO : See if we can delete those lines in file and replace by a check to see if we have attribute names or not. cf. patterns in hyperLocus.cpp
    }
    file << endl;

    if (!classes.empty()) {
      file << "Class names are specified.";
    } else {
      file << "Class names are not specified.";
    }

    file << endl
         << endl
         << stream.str();

    file.close();

  } else {
    throw CannotOpenFileError("Error : Couldn't open rules extraction file \"" + filename + "\".");
  }

  return make_tuple(meanCovSize, meanNbAntecedents);
}

/**
 * @brief Computes the fidex global algorithm.
 *
 * @param command list of arguments included by the user.
 * @return int
 */
int fidexGloRules(const string &command) {
  // Save buffer where we output results
  ofstream ofs;
  streambuf *cout_buff = cout.rdbuf(); // Save old buf

  try {
    // Parsing the command
    vector<string> commandList;
    string s;
    stringstream ss(command);

    while (getline(ss, s, ' ')) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2) {
      showRulesParams();
      return -1;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      }

      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2]));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("JSON config file name/path is invalid");
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList));
    }

    // getting all program arguments from CLI
    checkRulesParametersLogicValues(*params);
    cout << *params;

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Import files
    cout << "Importing files..." << endl;

    std::unique_ptr<DataSetFid> trainDatas;
    if (!params->isStringSet(TRAIN_CLASS_FILE)) {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE),
                                      params->getString(TRAIN_PRED_FILE),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(POSITIVE_CLASS_INDEX)));

      if (!trainDatas->getHasClasses()) {
        throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
      }
    } else {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE),
                                      params->getString(TRAIN_PRED_FILE),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(POSITIVE_CLASS_INDEX),
                                      params->getString(TRAIN_CLASS_FILE)));
    }

    int nbDatas = trainDatas->getNbSamples();

    if (params->getInt(MIN_COVERING) > nbDatas) {
      throw CommandArgumentException("Error : invalide type for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
    }

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;

    if (params->isStringSet(ATTRIBUTES_FILE)) {
      trainDatas->setAttributes(params->getString(ATTRIBUTES_FILE), params->getInt(NB_ATTRIBUTES), params->getInt(NB_CLASSES));
      attributeNames = trainDatas->getAttributeNames();
      hasClassNames = trainDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = trainDatas->getClassNames();
      }
    }

    vector<int> normalizationIndices;
    vector<double> mus;
    vector<double> sigmas;

    // Get mus, sigmas and normalizationIndices from normalizationFile for denormalization :
    if (params->isStringSet(NORMALIZATION_FILE)) {
      auto results = parseNormalizationStats(params->getString(NORMALIZATION_FILE), params->getInt(NB_ATTRIBUTES), attributeNames);
      normalizationIndices = std::get<0>(results);
      mus = std::get<2>(results);
      sigmas = std::get<3>(results);
      params->setIntVector(NORMALIZATION_INDICES, normalizationIndices);
      params->setDoubleVector(MUS, mus);
      params->setDoubleVector(SIGMAS, sigmas);
    }

    cout << "Files imported" << endl
         << endl;

    // compute hyperspace

    cout << "Creation of hyperspace..." << endl;

    vector<vector<double>> matHypLocus;
    string weightsFile;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFile = params->getString(WEIGHTS_FILE);
    }
    string attributesFile;
    if (params->isStringSet(ATTRIBUTES_FILE)) {
      attributesFile = params->getString(ATTRIBUTES_FILE);
    }
    string inputRulesFile;
    if (params->isStringSet(RULES_FILE)) {
      inputRulesFile = params->getString(RULES_FILE);
    }
    vector<string> weightsFiles;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFiles = params->getWeightsFiles();
    }
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    float hiKnot = params->getFloat(HI_KNOT);

    if (params->isStringSet(WEIGHTS_FILE)) {
      if (nbDimlpNets > 1) {
        cout << "\nParameters of hyperLocus :" << endl;
        cout << "\t- Number of stairs " << nbQuantLevels << endl;
        cout << "\t- Interval : [-" << hiKnot << "," << hiKnot << "]" << endl
             << endl;
        cout << "Computing all hyperlocuses..." << endl;
      }

      cout << "Size of weight files:" << weightsFiles.size() << endl;
      for (string wf : weightsFiles) {
        vector<vector<double>> hypLocus;
        if (nbDimlpNets > 1) {
          hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot, false); // Get hyperlocus
        } else {
          hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot); // Get hyperlocus
        }
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end()); // Concatenate hypLocus to matHypLocus
      }
      if (nbDimlpNets > 1) {
        cout << "All hyperlocus created" << endl;
      }

    } else {
      if (params->isStringSet(ATTRIBUTES_FILE)) {
        matHypLocus = calcHypLocus(inputRulesFile, nbAttributes, attributeNames);
      } else {
        matHypLocus = calcHypLocus(inputRulesFile, nbAttributes);
      }
    }

    // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    auto nbIn = static_cast<int>(matHypLocus.size());

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributes != 0) {
      throw InternalError("Error : the size of hyperLocus - " + to_string(nbIn) + " is not a multiple of the number of attributs - " + to_string(nbAttributes));
    }

    cout << "Hyperspace created." << endl
         << endl;

    // Samples not yet covered by any rules
    vector<int> notCoveredSamples(nbDatas);
    iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1
    vector<Rule> chosenRules;                                  // antecedents, cover vector, class, rule accuracy, rule confidence

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Initialize random number generator

    const auto start = high_resolution_clock::now();
    int heuristic = params->getInt(HEURISTIC);
    auto trainDataset = *trainDatas.get();
    auto parameters = *params.get();
    vector<Rule> generatedRules;

    cout << "Computing fidex rules..." << endl
         << endl;

    switch (heuristic) {
    case 1:
      generatedRules = heuristic_1(trainDataset, parameters, matHypLocus);
      break;

    case 2:
      generatedRules = heuristic_2(trainDataset, parameters, matHypLocus);
      break;

    case 3:
      generatedRules = heuristic_3(trainDataset, parameters, matHypLocus);
      break;

    default:
      cout << "Heuristic not recognised." << endl;
      return -1;
      break;
    }

    cout << "Global ruleset Computed." << endl;

    const auto end_h = high_resolution_clock::now();
    const duration<double> diff_h = end_h - start;

    cout << "\nheuristic #" << heuristic << " ended in "
         << diff_h.count() << " sec\n"
         << endl;

    cout << "Rules extraction..."
         << endl;

    sort(generatedRules.begin(), generatedRules.end(), [](const Rule &r1, const Rule &r2) {
      return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
    });

    tuple<double, double> stats = writeRulesFile(params->getString(GLOBAL_RULES_OUTFILE), generatedRules, attributeNames, classNames);

    cout << "Mean covering size per rule : " << get<0>(stats) << endl;
    cout << "Mean number of antecedents per rule : " << get<1>(stats) << endl;

    const auto end = high_resolution_clock::now();
    duration<double> diff = end - start;
    cout << "\nFull execution time = " << diff.count() << " sec" << endl;

    cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }

  return 0;
}

/* Exemples pour lancer le code :

./fidexGloRules --train_data_file datanormTrain --train_pred_file dimlpDatanormTrain.out --train_class_file dataclass2Train --weights_file dimlpDatanorm.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesDatanorm.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesResult --root_folder ../fidexGlo/datafiles

./fidexGloRules --train_data_file covidTrainData.txt --train_pred_file covidTrainPred.out --train_class_file covidTrainClass.txt --weights_file covid.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesCovid.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesCovidResult --root_folder ../dimlp/datafiles/covidDataset
./fidexGloRules --train_data_file spamTrainData.txt --train_pred_file spamTrainPred.out --train_class_file spamTrainClass.txt --weights_file spam.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesSpam.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesSpamResult --root_folder ../dimlp/datafiles/spamDataset
./fidexGloRules --train_data_file isoletTrainData.txt --train_pred_file isoletTrainPredV2.out --train_class_file isoletTrainClass.txt --weights_file isoletV2.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 5 --global_rules_outfile globalRulesIsoletV2.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesIsoletResultV2 --root_folder ../dimlp/datafiles/isoletDataset
./fidexGloRules --train_data_file Train/X_train.txt --train_pred_file Train/pred_trainV2.out --train_class_file Train/y_train.txt --weights_file HAPTV2.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 5 --global_rules_outfile globalRulesHAPTV2.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesHAPTResultV2 --root_folder ../dimlp/datafiles/HAPTDataset

*/
