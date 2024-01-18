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
  cout << "fidexGloRules -T <train data file> -P <train prediction file> -C <train true class file, not mendatory if classes are specified in train data file> ";
  cout << "-W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f] ";
  cout << "-f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  cout << "-O <Rules output file> ";
  cout << "-M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
  cout << "-a <number of attributes>";
  cout << "-b <number of classes>";
  cout << "<Options>\n"
       << endl;

  cout << "Options are: \n"
       << endl;
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << endl;
  cout << "-N <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << endl;
  cout << "-A <file of attributes>" << endl;
  cout << "-r <file where you redirect console result>" << endl; // If we want to redirect console result to file
  cout << "-i <max iteration number (100 by default)>" << endl;
  cout << "-v <minimum covering number (2 by default)>" << endl;
  cout << "-d <dimension dropout parameter>" << endl;
  cout << "-h <hyperplan dropout parameter>" << endl;
  cout << "-m <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>" << endl;
  cout << "-Q <number of stairs in staircase activation function (50 by default)>" << endl;
  cout << "-t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << endl;
  cout << "-x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << endl;
  cout << "-p <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)>" << endl;
  cout << "-y <minimal rule fidelity accepted when generating a rule [0,1] (default=1)>" << endl;
  cout << "-z <seed (0=random, default)>";

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
 * @param dataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 */
// TODO: implement dicotomic min covering
void generateRules(vector<Rule> *rules, vector<int> *notCoveredSamples, DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int nbDatas = dataset->getNbSamples();
  int minNbCover = p->getInt(MIN_COVERING);
  int nbThreadsUsed = p->getInt(NB_THREADS_USED);

  int seed = p->getInt(SEED);
  if (seed == 0) {
    auto currentTime = high_resolution_clock::now();
    auto seedValue = currentTime.time_since_epoch().count();
    seed = seedValue;
  }

  mt19937 gen(seed);

#pragma omp parallel num_threads(nbThreadsUsed)
  {
    Rule rule;
    int cnt = 0;
    double t1, t2;
    bool ruleCreated;
    int counterFailed;
    int localNbProblems = 0;
    vector<Rule> localRules;
    vector<int>::iterator it;
    int localNbRulesNotFound = 0;
    int currentMinNbCov = minNbCover;
    Hyperspace hyperspace(hyperlocus);
    int threadId = omp_get_thread_num();
    float minFidelity = p->getFloat(MIN_FIDELITY);
    string consoleFile = p->getString(CONSOLE_FILE);
    int maxFailedAttempts = p->getInt(MAX_FAILED_ATTEMPTS);
    Fidex fidex = Fidex(dataset, p, &hyperspace);

#pragma omp critical
    {
      cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;
    }

    t1 = omp_get_wtime();

#pragma omp for
    for (int idSample = 0; idSample < nbDatas; idSample++) {
      ruleCreated = false;
      counterFailed = 0; // If we can't find a good rule after a lot of tries
      cnt += 1;

      if (omp_get_thread_num() == 0) {
        if (((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && consoleFile.empty()) {
#pragma omp critical
          {
            cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
            cout.flush();
          }
        }
      }

      currentMinNbCov = minNbCover;
      while (!ruleCreated) {
        ruleCreated = fidex.compute(rule, idSample, minFidelity, currentMinNbCov);

        if (currentMinNbCov >= 2) {
          currentMinNbCov -= 1;
        } else {
          counterFailed += 1;
        }

        if (counterFailed >= maxFailedAttempts) {
          localNbRulesNotFound += 1;

#pragma omp critical
          {
            // ignore samples that cannot be covered after "maxFailedAttempts" attempts
            it = find(notCoveredSamples->begin(), notCoveredSamples->end(), idSample);
            if (it != notCoveredSamples->end()) {
              notCoveredSamples->erase(it);
            }
          }
          break;
        }
      }

      if ((currentMinNbCov + 1) < minNbCover) {
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
          rules->insert(rules->end(), localRules.begin(), localRules.end());
          nbProblems += localNbProblems;
          nbRulesNotFound += localNbRulesNotFound;
        }
      }
    }
  } // end of parallel section

  cout << endl
       << rules->size() << " rules created." << endl
       << "Number of sample with lower covering than " << minNbCover << " is " << nbProblems << endl
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
 * @param dataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 * @return vector<Rule>
 */
vector<Rule> heuristic_1(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  vector<Rule> rules;
  vector<Rule> chosenRules;
  int nbDatas = dataset->getNbSamples();
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1
  generateRules(&rules, &notCoveredSamples, dataset, p, hyperlocus);

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
        bestCovering = difference.size();
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
 * @param dataset class containing all usable data to compute the algorithm.
 * @param p class containing all used defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles used to compute Fidex alorithm
 * @return vector<Rule>
 */
vector<Rule> heuristic_2(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  vector<Rule> rules;
  vector<Rule> chosenRules;
  int nbDatas = dataset->getNbSamples();
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

  // getting rules and not covered samples
  generateRules(&rules, &notCoveredSamples, dataset, p, hyperlocus);

  // sort rules by covering
  sort(rules.begin(), rules.end(), [&rules](Rule r1, Rule r2) {
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

// TODO: this heuristic must be discussed, I am wondering if this ever worked correctly once...
// TODO: complete comments
/**
 * @brief 3rd heuristic,
 *
 * @param dataset
 * @param p
 * @param hyperlocus
 * @return vector<Rule>
 */
vector<Rule> heuristic_3(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  Rule rule;
  int idSample;
  int nbRules = 0;
  bool ruleCreated;
  int nbProblems = 0;
  int currentMinNbCov;
  int nbRulesNotFound = 0;
  vector<Rule> chosenRules;
  vector<int>::iterator ite;
  int seed = p->getInt(SEED);
  vector<int> chosenRuleSamples;
  Hyperspace hyperspace(hyperlocus);
  int minNbCover = p->getInt(MIN_COVERING);
  int nbDatas = dataset->getDatas()->size();
  vector<int> notCoveredSamples(nbDatas);
  float minFidelity = p->getFloat(MIN_FIDELITY);
  Fidex fidex = Fidex(dataset, p, &hyperspace);
  string consoleFile = p->getString(CONSOLE_FILE);
  int maxFailedAttempts = p->getInt(MAX_FAILED_ATTEMPTS);

  if (seed == 0) {
    auto currentTime = high_resolution_clock::now();
    auto seedValue = currentTime.time_since_epoch().count();
    seed = seedValue;
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
    currentMinNbCov = minNbCover;
    ruleCreated = false;
    int counterFailed = 0; // If we can't find a good rule after a lot of tries

    while (!ruleCreated) {
      ruleCreated = fidex.compute(rule, idSample, minFidelity, currentMinNbCov);

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

      // TODO: this part was probably broken, now is kinda fixed but im unsure if it's working as originaly intended....
      notCoveredSamples.erase(
          remove_if(
              notCoveredSamples.begin(), notCoveredSamples.end(), [&chosenRuleSamples](int sample) {
                return find(chosenRuleSamples.begin(), chosenRuleSamples.end(), sample) != chosenRuleSamples.end();
                // find index of coveredSamples which is "sample" ("sample" is element of notCoveredSamples), find returns last if "sample" not found
                // -> Removes "sample" if it appears on coveredSamples (found before the end of coveredSamples)
              }));

      nbRules++;
    } else {
      notCoveredSamples.erase(notCoveredSamples.begin());
    }
  }

  cout << endl;

  // remove duplicates
  chosenRules.erase(unique(chosenRules.begin(), chosenRules.end()), chosenRules.end());

  cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
  if (nbRulesNotFound > 0) {
    cout << "Number of rules not found : " << nbRulesNotFound << endl;
  }

  return chosenRules;
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkParametersLogicValues(Parameters *p) {
  // setting default values
  p->setDefaultString(ROOT_FOLDER, "");
  p->setDefaultInt(NB_DIMLP_NETS, 1);
  p->setDefaultString(ATTRIBUTES_FILE, "");
  p->setDefaultString(CONSOLE_FILE, "");
  p->setDefaultInt(MAX_ITERATIONS, 100);
  p->setDefaultInt(MIN_COVERING, 2);
  p->setDefaultFloat(DROPOUT_DIM, 0.0f);
  p->setDefaultFloat(DROPOUT_HYP, 0.0f);
  p->setDefaultInt(MAX_FAILED_ATTEMPTS, 30);
  p->setDefaultInt(NB_QUANT_LEVELS, 50);
  p->setDefaultFloat(DECISION_THRESHOLD, -1.0f);
  p->setDefaultInt(INDEX_POSITIVE_CLASS, -1);
  p->setDefaultInt(NB_THREADS_USED, 1);
  p->setDefaultFloat(MIN_FIDELITY, 1.0f);
  // p->setDefaultFloat(HI_KNOT, 5);
  p->setDefaultInt(SEED, 0);
  p->setWeightsFiles(); // must be called to initialize

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p->assertStringExists(TRAIN_DATA_FILE);
  p->assertStringExists(TRAIN_DATA_PRED_FILE);
  p->assertStringExists(TRAIN_DATA_TRUE_CLASS_FILE);
  p->assertStringExists(RULES_FILE);

  // verifying logic between parameters, values range and so on...
  if (p->isStringSet(WEIGHTS_FILE) && p->isStringSet(INPUT_RULES_FILE)) {
    throw CommandArgumentException("Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
  }

  if (!p->isStringSet(WEIGHTS_FILE) && !p->isStringSet(INPUT_RULES_FILE)) {
    throw CommandArgumentException("A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
  }

  if (p->getInt(NB_QUANT_LEVELS) < 1) {
    throw CommandArgumentException("Number of stairs in staircase activation function must be strictly positive (>=1).");
  }

  if (p->getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Number of networks must be strictly positive (>=1).");
  }

  // if (p->getFloat(HI_KNOT) < 0.0f) { }

  if (!(p->getInt(HEURISTIC) > 0 && p->getInt(HEURISTIC) < 4)) {
    throw CommandArgumentException("Heuristic must be 1, 2 or 3.");
  }

  // TODO continue checking with -i (https://github.com/HES-XPLAIN/dimlpfidex/blob/eedb7a977c61d5ff0371f8e4d454d864c99b1e53/fidexGlo/cpp/src/fidexGloRulesFct.cpp)

  if (p->isFloatSet(DECISION_THRESHOLD) && !p->isIntSet(INDEX_POSITIVE_CLASS)) {
    throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t)");
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
tuple<double, double> writeRulesFile(string filename, const vector<Rule> rules, const vector<string> *attributes, const vector<string> *classes) {
  if (rules.empty()) {
    cout << "Warning: cannot write to file \"" << filename << "\", generated rules list is empty.";
    return make_tuple(0, 0);
  }

  int counter = 1;
  int nbRules = static_cast<int>(rules.size());
  double meanCovSize = 0;
  double meanNbAntecedents = 0;
  stringstream stream;
  ofstream file(filename);

  for (Rule r : rules) { // each rule
    meanCovSize += static_cast<double>(r.getCoveredSamples().size());
    meanNbAntecedents += static_cast<double>(r.getAntecedants().size());
    stream << "Rule " << counter++ << ": " << r.toString(attributes, classes);
    stream << endl;
  }

  meanCovSize /= nbRules;
  meanNbAntecedents /= nbRules;

  if (file.is_open()) {
    file << "Number of rules : " << nbRules
         << ", mean sample covering number per rule : " << formattingDoubleToString(meanCovSize)
         << ", mean number of antecedents per rule : " << formattingDoubleToString(meanNbAntecedents)
         << endl;

    if (attributes && !attributes->empty()) {
      file << "Attribute names are specified. ";
    } else {
      file << "Attribute names are not specified. "; // TODO : See if we can delete those lines in file and replace by a check to see if we have attribute names or not. cf. patterns in hyperLocus.cpp
    }
    file << endl;

    if (classes && !classes->empty()) {
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
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (getline(ss, s, delim)) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();

    if (nbParam < 2) {
      showRulesParams();
      exit(1);
    }

    // getting all program arguments from CLI
    unique_ptr<Parameters> params(new Parameters(commandList));
    checkParametersLogicValues(params.get());
    cout << *params;

    // Get console results to file
    string consoleFile = params->getString(CONSOLE_FILE);
    if (!consoleFile.empty()) {
      ofs.open(consoleFile);
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Import files
    cout << "Importing files..." << endl;

    std::unique_ptr<DataSetFid> trainDatas;
    if (!params->isStringSet(TRAIN_DATA_TRUE_CLASS_FILE)) {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE).c_str(),
                                      params->getString(TRAIN_DATA_PRED_FILE).c_str(),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(INDEX_POSITIVE_CLASS)));

      if (!trainDatas->getHasClasses()) {
        throw CommandArgumentException("The train true classes file has to be given with option -C or classes have to be given in the train data file.");
      }
    } else {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE).c_str(),
                                      params->getString(TRAIN_DATA_PRED_FILE).c_str(),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(INDEX_POSITIVE_CLASS),
                                      params->getString(TRAIN_DATA_TRUE_CLASS_FILE).c_str()));
    }

    int nbDatas = trainDatas->getNbSamples();

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;

    if (!params->getString(ATTRIBUTES_FILE).empty()) {
      trainDatas->setAttributes(params->getString(ATTRIBUTES_FILE).c_str(), params->getInt(NB_ATTRIBUTES), params->getInt(NB_CLASSES));
      attributeNames = (*trainDatas->getAttributeNames());
      hasClassNames = trainDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = (*trainDatas->getClassNames());
      }
    }

    cout << "Files imported" << endl
         << endl;

    // compute hyperspace

    cout << "Creation of hyperspace..." << endl;

    vector<vector<double>> matHypLocus;
    string weightsFile = params->getString(WEIGHTS_FILE);
    string attributesFile = params->getString(ATTRIBUTES_FILE);
    string inputRulesFile = params->getString(INPUT_RULES_FILE);
    vector<string> weightsFiles = params->getWeightsFiles();
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    int hiKnot = params->getInt(HI_KNOT);

    if (!weightsFile.empty()) {
      if (nbDimlpNets > 1) {
        cout << "\nParameters of hyperLocus :" << endl;
        cout << "\t- Number of stairs " << nbQuantLevels << endl;
        cout << "\t- Interval : [-" << hiKnot << "," << hiKnot << "]" << endl
             << endl;
        cout << "Computing all hyperlocuses..." << endl;
      }

      cout << "Size of weight files:" << weightsFiles.size() << endl;
      for (string weightsFile : weightsFiles) {
        vector<vector<double>> hypLocus;
        if (nbDimlpNets > 1) {
          hypLocus = calcHypLocus(weightsFile.c_str(), nbQuantLevels, hiKnot, false); // Get hyperlocus
        } else {
          hypLocus = calcHypLocus(weightsFile.c_str(), nbQuantLevels, hiKnot); // Get hyperlocus
        }
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end()); // Concatenate hypLocus to matHypLocus
      }
      if (nbDimlpNets > 1) {
        cout << "All hyperlocus created" << endl;
      }

    } else {
      if (!attributesFile.empty()) {
        matHypLocus = calcHypLocus(inputRulesFile.c_str(), nbAttributes, attributeNames);
      } else {
        matHypLocus = calcHypLocus(inputRulesFile.c_str(), nbAttributes);
      }
    }

    // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    int nbIn = static_cast<int>(matHypLocus.size());

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
    vector<Rule> generatedRules;
    int nbRules = 0;

    cout << "Computing fidex rules..." << endl
         << endl;

    switch (heuristic) {
    case 1:
      generatedRules = heuristic_1(trainDatas.get(), params.get(), matHypLocus);
      break;

    case 2:
      generatedRules = heuristic_2(trainDatas.get(), params.get(), matHypLocus);
      break;

    case 3:
      generatedRules = heuristic_3(trainDatas.get(), params.get(), matHypLocus);
      break;

    default:
      cout << "Heuristic not recognised." << endl;
      return -1;
      break;
    }

    nbRules = generatedRules.size();

    cout << "Global ruleset Computed." << endl;

    const auto end_h = high_resolution_clock::now();
    const duration<double> diff_h = end_h - start;

    cout << "\nheuristic #" << heuristic << " ended in "
         << diff_h.count() << " sec\n"
         << endl;

    cout << "Rules extraction..."
         << endl;

    sort(generatedRules.begin(), generatedRules.end(), [&generatedRules](Rule r1, Rule r2) {
      return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
    });

    tuple<double, double> stats = writeRulesFile(params->getString(RULES_FILE), generatedRules, &attributeNames, &classNames);

    cout << "Mean covering size per rule : " << get<0>(stats) << endl;
    cout << "Mean number of antecedents per rule : " << get<1>(stats) << endl;

    const auto end = high_resolution_clock::now();
    duration<double> diff = end - start;
    cout << "\nFull execution time = " << diff.count() << " sec" << endl;

    cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    cout.rdbuf(cout_buff);
    cerr << msg << endl;
    return -1;
  }

  return 0;
}

/* Exemples pour lancer le code :

./fidexGloRules -T datanormTrain -P dimlpDatanormTrain.out -C dataclass2Train -W dimlpDatanorm.wts -a 16 -b 2 -Q 50 -O globalRulesDatanorm.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesResult -S ../fidexGlo/datafiles

./fidexGloRules -T covidTrainData.txt -P covidTrainPred.out -C covidTrainClass.txt -W covid.wts -a 16 -b 2 -Q 50 -O globalRulesCovid.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesCovidResult -S ../dimlp/datafiles/covidDataset
./fidexGloRules -T spamTrainData.txt -P spamTrainPred.out -C spamTrainClass.txt -W spam.wts -a 16 -b 2 -Q 50 -O globalRulesSpam.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesSpamResult -S ../dimlp/datafiles/spamDataset
./fidexGloRules -T isoletTrainData.txt -P isoletTrainPredV2.out -C isoletTrainClass.txt -W isoletV2.wts -a 16 -b 2 -Q 50 5 -O globalRulesIsoletV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesIsoletResultV2 -S ../dimlp/datafiles/isoletDataset
./fidexGloRules -T Train/X_train.txt -P Train/pred_trainV2.out -C Train/y_train.txt -W HAPTV2.wts -a 16 -b 2 -Q 50 5 -O globalRulesHAPTV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesHAPTResultV2 -S ../dimlp/datafiles/HAPTDataset

*/
