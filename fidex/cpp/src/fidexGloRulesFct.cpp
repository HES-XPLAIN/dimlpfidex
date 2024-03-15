#include "fidexGloRulesFct.h"

/**
 * @brief shows all program arguments and details into the terminal.
 *
 */
void showRulesParams() {
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are localised with respect to root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--train_data_file <str>", "Train data file");
  printOptionDescription("--train_pred_file <str>", "Train prediction file");
  printOptionDescription("--train_class_file <str>", "Train true class file, not mandatory if classes are specified in train data file");
  printOptionDescription("--weights_file <str>", "Weights file when not using bagging (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--weights_generic_filename <str>", "Weights file in case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Rules file to be converted to hyperlocus (not mandatory if a weights file or a weights_generic_filename is given)");
  printOptionDescription("--global_rules_outfile <str>", "Rules output file>");
  printOptionDescription("--heuristic <int [1,3]>", "Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--nb_dimlp_nets <int [1,inf[>", "Number of networks for bagging, necessary to use bagging");
  printOptionDescription("--attributes_file <str>", "File of attributes");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--max_iterations <int [1,inf[>", "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum covering number (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Dimension dropout parameter (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Hyperplan dropout parameter (default: 0.0)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
  printOptionDescription("--decision_threshold <float [0,1]>", "Decision threshold for predictions, you need to specify the index of positive class if you want to use it");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of positive class for the usage of decision threshold, index starts at 0");
  printOptionDescription("--normalization_file <str>", "File containing the mean and std of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]inf,inf[>>", "Mean or median of each attribute index to denormalize in the rules");
  printOptionDescription("--sigmas <list<float ]inf,inf[>>", "Standard deviation of each attribute index to denormalize in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");
  printOptionDescription("--nb_threads <int [1,nb_cores]>", "Number of threads used for computing the algorithm, 1=sequential execution (default: 1)");
  printOptionDescription("--seed <int [0,inf[>", "Seed, 0=random (default: 0)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "fidex.fidexGloRules(\"--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --weights_file weights.wts --nb_attributes 16 --nb_classes 2 --heuristic 1 --global_rules_outfile globalRules.rls --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
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
    int localNbProblems = 0;
    vector<Rule> localRules;
    vector<int>::iterator it;
    int localNbRulesNotFound = 0;
    Hyperspace hyperspace(hyperlocus);
    int threadId = omp_get_thread_num();
    auto fidex = Fidex(trainDataset, p, hyperspace, false);

    string consoleFile = "";
    if (p.isStringSet(CONSOLE_FILE)) {
      consoleFile = p.getString(CONSOLE_FILE);
    }

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
      cnt += 1;

      if (omp_get_thread_num() == 0 && ((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && consoleFile.empty()) {
#pragma omp critical
        {
          cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
          cout.flush();
        }
      }

      ruleCreated = fidex.launchFidex(rule, mainSampleValues, mainSamplePred, -1);

      if (!ruleCreated) {
        localNbRulesNotFound += 1;
#pragma omp critical
        {
          // ignore samples that cannot be covered after "maxFailedAttempts" attempts
          it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
          if (it != notCoveredSamples.end()) {
            notCoveredSamples.erase(it);
          }
        }
      } else {
        localRules.push_back(rule);
      }

      if (rule.getCoveringSize() < minCovering) {
        localNbProblems += 1;
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
  int nbRulesNotFound = 0;
  vector<Rule> chosenRules;
  int seed = p.getInt(SEED);
  vector<int> chosenRuleSamples;
  Hyperspace hyperspace(hyperlocus);
  int minNbCover = p.getInt(MIN_COVERING);
  auto nbDatas = static_cast<int>(trainDataset.getDatas().size());
  vector<int> notCoveredSamples(nbDatas);
  auto fidex = Fidex(trainDataset, p, hyperspace, false);
  string consoleFile = "";
  if (p.isStringSet(CONSOLE_FILE)) {
    consoleFile = p.getString(CONSOLE_FILE);
  }

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

    ruleCreated = fidex.launchFidex(rule, mainSampleValues, mainSamplePred, -1);

    if (!ruleCreated) {
      nbRulesNotFound += 1;
    }
    if (rule.getCoveringSize() < minNbCover) {
      nbProblems += 1;
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
  p.setDefaultNbQuantLevels();
  p.setDefaultDecisionThreshold();
  p.setDefaultFidex();
  p.setDefaultInt(NB_THREADS, 1);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(TRAIN_PRED_FILE);
  p.assertStringExists(GLOBAL_RULES_OUTFILE);
  p.assertIntExists(HEURISTIC);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersFidex();
  p.checkParametersDecisionThreshold();
  p.checkParametersNormalization();

  if (p.getInt(NB_THREADS) < 1 || p.getInt(NB_THREADS) > omp_get_max_threads()) {
    throw CommandArgumentException("Error : Number threads must be between 1 and #CPU cores of your machine (which is " + to_string(omp_get_max_threads()) + ")");
  }

  if (!(p.getInt(HEURISTIC) > 0 && p.getInt(HEURISTIC) < 4)) {
    throw CommandArgumentException("Error : Heuristic must be 1, 2 or 3.");
  }
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
    vector<string> commandList = {"fidexGloRules"};
    string s;
    stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showRulesParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, WEIGHTS_FILE, WEIGHTS_GENERIC_FILENAME, RULES_FILE, GLOBAL_RULES_OUTFILE,
                                              HEURISTIC, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE, CONSOLE_FILE,
                                              MAX_ITERATIONS, MIN_COVERING, DROPOUT_DIM, DROPOUT_HYP, MAX_FAILED_ATTEMPTS, NB_QUANT_LEVELS,
                                              DECISION_THRESHOLD, POSITIVE_CLASS_INDEX, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES,
                                              NB_THREADS, COVERING_STRATEGY, MIN_FIDELITY, LOWEST_MIN_FIDELITY, NB_DIMLP_NETS, SEED};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }
      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("JSON config file name/path is invalid");
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkRulesParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

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
    if (params->isStringSet(WEIGHTS_GENERIC_FILENAME)) {
      weightsFiles = params->getWeightsFiles();
    }
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    float hiKnot = params->getFloat(HI_KNOT);

    if (params->isStringSet(WEIGHTS_FILE)) {
      matHypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
    } else if (params->isStringSet(WEIGHTS_GENERIC_FILENAME)) {
      std::cout << "\nParameters of hyperLocus :\n"
                << std::endl;
      std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
      std::cout << "- Interval : [-" << hiKnot << "," << hiKnot << "]" << std::endl
                << std::endl;
      std::cout << "Computation of all hyperlocus" << std::endl;
      for (string wf : weightsFiles) {
        std::vector<std::vector<double>> hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot, false); // Get hyperlocus
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end());                    // Concatenate hypLocus to matHypLocus
      }
      std::cout << "All hyperlocus created" << std::endl;
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, *trainDatas);
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

./fidexGloRules --train_data_file datanormTrain --train_pred_file dimlpDatanormTrain.out --train_class_file dataclass2Train --weights_file dimlpDatanorm.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesDatanorm.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesResult --root_folder ../fidex/datafiles

./fidexGloRules --train_data_file covidTrainData.txt --train_pred_file covidTrainPred.out --train_class_file covidTrainClass.txt --weights_file covid.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesCovid.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesCovidResult --root_folder ../dimlp/datafiles/covidDataset
./fidexGloRules --train_data_file spamTrainData.txt --train_pred_file spamTrainPred.out --train_class_file spamTrainClass.txt --weights_file spam.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 --global_rules_outfile globalRulesSpam.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesSpamResult --root_folder ../dimlp/datafiles/spamDataset
./fidexGloRules --train_data_file isoletTrainData.txt --train_pred_file isoletTrainPredV2.out --train_class_file isoletTrainClass.txt --weights_file isoletV2.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 5 --global_rules_outfile globalRulesIsoletV2.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesIsoletResultV2 --root_folder ../dimlp/datafiles/isoletDataset
./fidexGloRules --train_data_file Train/X_train.txt --train_pred_file Train/pred_trainV2.out --train_class_file Train/y_train.txt --weights_file HAPTV2.wts --nb_attributes 16 --nb_classes 2 --nb_quant_levels 50 5 --global_rules_outfile globalRulesHAPTV2.txt --heuristic 1 --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --console_file rulesHAPTResultV2 --root_folder ../dimlp/datafiles/HAPTDataset

*/
