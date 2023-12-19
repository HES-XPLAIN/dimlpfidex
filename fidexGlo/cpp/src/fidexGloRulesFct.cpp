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
  cout << "fidexGloRules -T <train dataset file> -P <train prediction file> -C <train true class file> ";
  cout << "-W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f] ";
  cout << "-f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  cout << "-O <Rules output file> ";
  cout << "-M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
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

mt19937 generateRandom(int seed) {
  auto currentTime = high_resolution_clock::now();
  auto seedValue = currentTime.time_since_epoch().count();
  mt19937 random(seedValue);

  return random;
}

// TODO: comment this
vector<Rule> heuristic_1(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  mt19937 gen;
  int nbDatas = dataset->getNbSamples();
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int seed = p->getInt(SEED);
  int minNbCover = p->getInt(MIN_COVERING);
  int nbThreadsUsed = p->getInt(NB_THREADS_USED);
  vector<Rule> rules;
  vector<Rule> chosenRules;
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

  cout << omp_get_num_procs() << " CPU cores available" << endl
       << endl;

  if (seed == 0) {
    gen = generateRandom(seed);
  } else {
    gen = mt19937(seed);
  }

#pragma omp parallel num_threads(nbThreadsUsed)
  {
    // declaring thread internal variables
    int threadId = omp_get_thread_num();

#pragma omp critical
    {
      cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;
    }

    Rule rule;
    vector<Rule> localRules;
    auto exp = FidexAlgo();
    bool ruleCreated;
    int counterFailed;
    double t1, t2;
    double minFidelity = p->getFloat(MIN_FIDELITY);

    int cnt = 0;
    int localNbRulesNotFound = 0;
    int localNbProblems = 0;
    int localMinNbCover = 0;
    int currentMinNbCov = minNbCover;
    int maxFailedAttempts = p->getInt(MAX_FAILED_ATTEMPTS);
    Hyperspace hyperspace(hyperlocus);
    vector<int>::iterator it;

    t1 = omp_get_wtime();

#pragma omp for
    for (int idSample = 0; idSample < nbDatas; idSample++) {
      ruleCreated = false;
      counterFailed = 0; // If we can't find a good rule after a lot of tries
      cnt += 1;

      if (omp_get_thread_num() == 0) {
        if (((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && !p->isStringSet(CONSOLE_FILE)) {
#pragma omp critical
          {
            cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
            std::cout.flush();
          }
        }
      }

      while (!ruleCreated) {
        ruleCreated = exp.fidex(rule, dataset, p, &hyperspace, idSample, minFidelity, gen);

        if (currentMinNbCov >= 2) {
          currentMinNbCov -= 1;
        } else {
          counterFailed += 1;
        }

        if (counterFailed >= maxFailedAttempts) {
          localNbRulesNotFound += 1;

#pragma omp critical
          {
            it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
            if (it != notCoveredSamples.end()) {
              notCoveredSamples.erase(it);
            }
          }
          break;
        }
      }

      if (currentMinNbCov + 1 < minNbCover) {
        localNbProblems += 1;
      }
      localRules.push_back(rule);
    }
    t2 = omp_get_wtime();

// gathering local data to main process
#pragma omp critical
    {
      cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << endl;
      rules.insert(rules.end(), localRules.begin(), localRules.end());
      nbProblems += localNbProblems;
      nbRulesNotFound += localNbRulesNotFound;
    }
  } // end of parallel section

  cout << rules.size() << " rules created." << endl
       << endl;
  cout << "Number of sample with lower covering than " << minNbCover << " is " << nbProblems << endl;
  cout << "Number of rules not found is " << nbRulesNotFound << endl;
  cout << "Fidex rules computed" << endl;
  cout << "Computing global ruleset..." << endl;

  // While there is some not covered samples
  int nbRules = 0;
  Rule currentRule;
  vector<int>::iterator it;
  vector<int> currentRuleSamples;
  vector<int> newNotCoveredSamples;
  vector<int> bestNewNotCoveredSamples;

  while (!notCoveredSamples.empty()) {
    // Get rule that covers the most new samples
    int bestRule = -1;
    int bestCovering = INT_MAX;
    int currentCovering; // Size of new covering if we choose this rule
    for (int r = 0; r < rules.size(); r++) {
      newNotCoveredSamples = notCoveredSamples;
      // Remove samples that are in current covering

      currentRuleSamples = rules[r].getCoveredSamples();

      it = set_difference(newNotCoveredSamples.begin(),
                          newNotCoveredSamples.end(),
                          currentRuleSamples.begin(),
                          currentRuleSamples.end(),
                          newNotCoveredSamples.begin()); // vectors have to be sorted

      newNotCoveredSamples.resize(it - newNotCoveredSamples.begin());
      currentCovering = static_cast<int>(newNotCoveredSamples.size());

      if (currentCovering < bestCovering) {
        bestRule = r;
        bestCovering = currentCovering;
        bestNewNotCoveredSamples = newNotCoveredSamples;
      }
    }

    currentRule = rules[bestRule];
    notCoveredSamples = bestNewNotCoveredSamples; // Delete new covered samples
    nbRules += 1;
    chosenRules.push_back(currentRule);    // add best rule with maximum covering
    rules.erase(rules.begin() + bestRule); // Remove this rule
    if (rules.size() < 1)
      break;
  }

  cout << chosenRules.size() << " rules selected." << endl;

  return chosenRules;
}

// TODO: comment this
// TODO: remove duplicated code (merge with heurisitic 1)
vector<Rule> heuristic_2(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  mt19937 gen;
  int nbDatas = dataset->getDatas()->size();
  int nbThreads = omp_get_num_procs();
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int seed = p->getInt(SEED);
  int minNbCover = p->getInt(MIN_COVERING);
  int nbThreadsUsed = p->getInt(NB_THREADS_USED);
  vector<Rule> rules;
  vector<Rule> chosenRules;
  vector<int> samplesNotFound;
  vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

  cout << nbThreads << " CPU cores available" << endl
       << endl;

  if (seed == 0) {
    gen = generateRandom(seed);
  } else {
    gen = mt19937(seed);
  }

#pragma omp parallel num_threads(nbThreadsUsed)
  {
    // declaring thread internal variables
    int threadId = omp_get_thread_num();

#pragma omp critical
    {
      cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;
    }

    Rule rule;
    vector<Rule> localRules;
    auto exp = FidexAlgo();
    bool ruleCreated;
    int counterFailed;
    int currentMinNbCov = minNbCover;
    int maxFailedAttempts = p->getInt(MAX_FAILED_ATTEMPTS);
    int localNbRulesNotFound = 0;
    int localNbProblems = 0;
    int localMinNbCover = 0;
    int cnt = 0;
    double t1, t2;
    float minFidelity = p->getFloat(MIN_FIDELITY);
    Hyperspace hyperspace(hyperlocus);
    vector<int>::iterator it;

    t1 = omp_get_wtime();

#pragma omp for
    for (int idSample = 0; idSample < nbDatas; idSample++) {
      cnt += 1;

      if (omp_get_thread_num() == 0) {
        if (((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && !p->isStringSet(CONSOLE_FILE)) {
#pragma omp critical
          {
            cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
            std::cout.flush();
          }
        }
      }

      currentMinNbCov = minNbCover;
      ruleCreated = false;
      int counterFailed = 0; // If we can't find a good rule after a lot of tries

      while (!ruleCreated) {
        ruleCreated = exp.fidex(rule, dataset, p, &hyperspace, idSample, minFidelity, gen);

        if (currentMinNbCov >= 2) {
          currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
        } else {
          counterFailed += 1;
        }
        if (counterFailed >= maxFailedAttempts) {
          localNbRulesNotFound += 1;
#pragma omp critical
          {
            it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
            if (it != notCoveredSamples.end()) {
              notCoveredSamples.erase(it);
            }
          }
          break;
        }
      }
      if (currentMinNbCov + 1 < minNbCover) {
        localNbProblems += 1;
      }
      localRules.push_back(rule);
    }

    t2 = omp_get_wtime();

#pragma omp critical
    {
      cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << endl;
      rules.insert(rules.end(), localRules.begin(), localRules.end());
      nbProblems += localNbProblems;
      nbRulesNotFound += localNbRulesNotFound;
    }
  } // end of parallel section

  cout << rules.size() << " rules created." << endl
       << endl;
  cout << "Number of sample with lower covering than " << minNbCover << " is " << nbProblems << endl;
  cout << "Number of rules not found is " << nbRulesNotFound << endl;
  cout << "Fidex rules computed." << endl;
  cout << "Computing global ruleset..." << endl;
  cout << "\nNumber of sample with lower covering than " << minNbCover << ": " << nbProblems << endl;
  cout << "Number of rules not found: " << nbRulesNotFound << endl;

  // Sort the rules(dataIds) depending of their covering size
  vector<int> dataIds = notCoveredSamples;
  sort(rules.begin(), rules.end(), [&rules](Rule r1, Rule r2) {
    return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
  });

  // While there is some not covered samples
  int dataId = 0;
  int nbRules = 0;
  Rule currentRule;
  vector<int>::iterator ite;
  int ancienNotCoveringSize = static_cast<int>(notCoveredSamples.size());

  while (!notCoveredSamples.empty()) {
    currentRule = rules[dataId];

    // Delete covered samples
    ite = set_difference(notCoveredSamples.begin(),
                         notCoveredSamples.end(),
                         currentRule.getCoveredSamples().begin(),
                         currentRule.getCoveredSamples().end(),
                         notCoveredSamples.begin()); // vectors have to be sorted

    notCoveredSamples.resize(ite - notCoveredSamples.begin());

    // If the rule covers a new sample
    if (ancienNotCoveringSize > notCoveredSamples.size()) {
      nbRules += 1;
      chosenRules.push_back(currentRule); // add best rule with maximum covering
    }
    ancienNotCoveringSize = static_cast<int>(notCoveredSamples.size());

    dataId += 1;
  }

  cout << chosenRules.size() << " rules selected." << endl;

  return chosenRules;
}

// TODO: comment this
vector<Rule> heuristic_3(DataSetFid *dataset, Parameters *p, vector<vector<double>> hyperlocus) {
  Rule rule;
  mt19937 gen;
  int idSample;
  int currentMinNbCov;
  bool ruleCreated;
  auto exp = FidexAlgo();
  int nbRules = 0;
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int seed = p->getInt(SEED);
  int nbDatas = dataset->getDatas()->size();
  int minNbCover = p->getInt(MIN_COVERING);
  int maxFailedAttempts = p->getInt(MAX_FAILED_ATTEMPTS);
  float minFidelity = p->getFloat(MIN_FIDELITY);
  Hyperspace hyperspace(hyperlocus);
  vector<int> notCoveredSamples(nbDatas);
  vector<Rule> chosenRules;

  if (seed == 0) {
    gen = generateRandom(seed);
  } else {
    gen = mt19937(seed);
  }

  iota(begin(notCoveredSamples), end(notCoveredSamples), 0);      // Vector from 0 to nbDatas-1
  shuffle(begin(notCoveredSamples), end(notCoveredSamples), gen); //  Sort data randomly

  cout << "Computing rules..." << endl
       << endl;

  // While there is some not covered samples
  while (!notCoveredSamples.empty()) {

    if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && !p->isStringSet(CONSOLE_FILE)) {
      cout << "Processing: " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%\r";
      cout.flush();
    }

    idSample = notCoveredSamples[0];
    currentMinNbCov = minNbCover;
    ruleCreated = false;
    int counterFailed = 0; // If we can't find a good rule after a lot of tries
    while (!ruleCreated) {
      ruleCreated = exp.fidex(rule, dataset, p, &hyperspace, idSample, minFidelity, gen);

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

      // Delete covered samples
      notCoveredSamples.erase(
          remove_if(
              notCoveredSamples.begin(), notCoveredSamples.end(), [&rule](int x) {
                return find(rule.getCoveredSamples().begin(), rule.getCoveredSamples().end(), x) != rule.getCoveredSamples().end();
                // find index of coveredSamples which is x (x is element of notCoveredSamples), find returns last if x not found
                // -> Removes x if it appears on coveredSamples (found before the end of coveredSamples)
              }),
          notCoveredSamples.end());

      nbRules++;
    } else {
      notCoveredSamples.erase(notCoveredSamples.begin());
    }
  }

  cout << endl;

  cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
  if (nbRulesNotFound > 0) {
    cout << "Number of rules not found : " << nbRulesNotFound << endl;
  }

  return chosenRules;
}

// TODO: this is probably not relevant enymore (existing checks are made by the Parameter class, BUT LOGIC CHECKS ARE NOT)
// void checkParameters(Parameters *p) {
//   if (!p->getTrainDataFile().empty()) {
//     throw CommandArgumentException("The train data file has to be given with option -T");
//   }

//   if (!p->getTrainDataFilePred().empty()) {
//     throw CommandArgumentException("The train prediction file has to be given with option -P");
//   }

//   if (p->getTrainDataFileTrueClass().empty()) {
//     throw CommandArgumentException("The train true classes file has to be given with option -C");
//   }

//   if (p->getRulesFile().empty()) {
//     throw CommandArgumentException("The output rules file has to be given with option -O");
//   }

//   if (p->getWeightsFile().empty() && p->getInputRulesFile().empty()) {
//     throw CommandArgumentException("A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
//   } else if (!p->getWeightsFile().empty() && !p->getInputRulesFile().empty()) {
//     throw CommandArgumentException("Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
//   }

//   if (p->getHasDecisionThreshold() && !p->getHasIndexPositiveClass()) {
//     throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t)");
//   }

//   if (!p->getHeuristic()) {
//     throw CommandArgumentException("The heuristic(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo) has to be given with option -M");
//   }
// }

/**
 * @brief writes a list of rules into a given file. Returns tuple of two integers representing the mean covering size and the mean number of antecedants.
 *
 * @param filename name of the file to be written/overwritten.
 * @param rules list of Rules object to be written in "filename".
 * @param hasConfidence whether or not Rule's confidence is written in file.
 * @param attributes list of the attributes names, used to write Rules's Antecedants with attributes explicit name instead of a "X" variable.
 * @param classes list of class names, used to write Rule's class with class explicit name instead of its numerical representation.
 * @return tuple<int, int>
 */
tuple<int, int> writeRulesFile(string filename, const vector<Rule> rules, const vector<string> *attributes, const vector<string> *classes) {
  if (rules.empty()) {
    cout << "Warning: cannot write to file \"" << filename << "\", generated rules list is empty.";
    return make_tuple(0, 0);
  }

  int counter = 1;
  int nbRules = rules.size();
  double meanCovSize = 0;
  double meanNbAntecedents = 0;
  stringstream stream;
  ofstream file(filename);

  for (Rule r : rules) { // each rule
    meanCovSize += r.getCoveredSamples().size();
    meanNbAntecedents += r.getAntecedants().size();
    stream << "Rule " << counter++ << ": " << r.toString(attributes, classes);
    stream << endl;
  }

  meanCovSize /= nbRules;
  meanNbAntecedents /= nbRules;

  if (file.is_open()) {
    file << "Number of rules : " << nbRules
         << ", mean sample covering number per rule : " << meanCovSize
         << ", mean number of antecedents per rule : " << meanNbAntecedents
         << endl;

    if (attributes && !attributes->empty()) {
      file << "Attributes name are specified.";
    } else {
      file << "Attributes name are not specified.";
    }

    if (classes && !classes->empty()) {
      file << "Classes name are specified.";
    } else {
      file << "Classes name are not specified.";
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

    if (nbParam <= 1) {
      showRulesParams();
      return 0;
    }

    // getting all program arguments from CLI
    unique_ptr<Parameters> params(new Parameters(commandList));

    // Fill weights vector
    if (!params->getWeightsFiles().empty()) {
      if (params->getInt(NB_DIMLP_NETS) == 1) {
        params->addWeightsFile(params->getString(WEIGHTS_FILE));
      } else {
        for (int n = 0; n < params->getInt(NB_DIMLP_NETS) == 1; n++) {
          params->addWeightsFile(params->getString(WEIGHTS_FILE) + to_string(n + 1) + ".wts");
        }
      }
    }

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // cout << *params;

    // Import files
    cout << "Importing files..." << endl;

    //! WARNING: if decision threshold is not set, this crashes
    unique_ptr<DataSetFid> trainDatas(new DataSetFid("trainDatas from FidexGloRules",
                                                     params->getString(TRAIN_DATA_FILE).c_str(),
                                                     params->getString(TRAIN_DATA_PRED_FILE).c_str(),
                                                     params->isFloatSet(DECISION_THRESHOLD),
                                                     params->getFloat(DECISION_THRESHOLD),
                                                     params->getInt(INDEX_POSITIVE_CLASS),
                                                     params->getString(TRAIN_DATA_TRUE_CLASS_FILE).c_str()));

    int nbDatas = trainDatas->getNbSamples();
    int nbAttributs = trainDatas->getNbAttributes();

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;

    if (!params->getString(ATTRIBUTES_FILE).empty()) {
      trainDatas->setAttribute(params->getString(ATTRIBUTES_FILE).c_str());
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
    string attributFile = params->getString(ATTRIBUTES_FILE);
    string inputRulesFile = params->getString(INPUT_RULES_FILE);
    vector<string> weightsFiles = params->getWeightsFiles();
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    int hiKnot = params->getInt(HI_KNOT);

    if (!weightsFile.empty()) {
      if (nbDimlpNets > 1) {
        cout << "\nParameters of hyperLocus :" << endl;
        cout << "\t- Number of stairs " << nbQuantLevels << endl;
        cout << "\t- Interval : [-" << hiKnot << "," << hiKnot << "]" << endl
             << endl;
        cout << "Computing all hyperlocuses..." << endl;
      }

      for (const auto &weightsFile : weightsFiles) {
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
      if (!attributFile.empty()) {
        matHypLocus = calcHypLocus(inputRulesFile.c_str(), nbAttributs, attributeNames);
      } else {
        matHypLocus = calcHypLocus(inputRulesFile.c_str(), nbAttributs);
      }
    }

    // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    const auto nbIn = static_cast<int>(matHypLocus.size());

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0) {
      throw InternalError("Error : the size of hyperLocus - " + to_string(nbIn) + " is not a multiple of the number of attributs - " + to_string(nbAttributs));
    }

    cout << "Hyperspace created" << endl
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

    cout << "Global ruleset Computed" << endl
         << endl;

    cout << nbRules << " rules selected." << endl
         << endl;

    const auto end_h = high_resolution_clock::now();
    const duration<double> diff_h = end_h - start;

    cout << "\nheuristic #" << heuristic << " ended in "
         << diff_h.count() << " sec\n"
         << endl;

    cout << "Rules extraction..."
         << endl
         << endl;

    sort(generatedRules.begin(), generatedRules.end(), [&generatedRules](Rule r1, Rule r2) {
      return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
    });

    tuple<int, int> stats = writeRulesFile(params->getString(RULES_FILE), generatedRules, &attributeNames, &classNames);

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

./fidexGloRules -T datanormTrain -P dimlpDatanormTrain.out -C dataclass2Train -W dimlpDatanorm.wts -Q 50 -O globalRulesDatanorm.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesResult -S ../fidexGlo/datafiles

./fidexGloRules -T covidTrainData.txt -P covidTrainPred.out -C covidTrainClass.txt -W covid.wts -Q 50 -O globalRulesCovid.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesCovidResult -S ../dimlp/datafiles/covidDataset
./fidexGloRules -T spamTrainData.txt -P spamTrainPred.out -C spamTrainClass.txt -W spam.wts -Q 50 -O globalRulesSpam.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesSpamResult -S ../dimlp/datafiles/spamDataset
./fidexGloRules -T isoletTrainData.txt -P isoletTrainPredV2.out -C isoletTrainClass.txt -W isoletV2.wts -Q 50 5 -O globalRulesIsoletV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesIsoletResultV2 -S ../dimlp/datafiles/isoletDataset
./fidexGloRules -T Train/X_train.txt -P Train/pred_trainV2.out -C Train/y_train.txt -W HAPTV2.wts -Q 50 5 -O globalRulesHAPTV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesHAPTResultV2 -S ../dimlp/datafiles/HAPTDataset

*/
