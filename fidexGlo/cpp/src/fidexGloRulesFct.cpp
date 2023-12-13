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

    if (attributes) {
      file << "Attributes name are specified.";
    } else {
      file << "Attributes name are not specified.";
    }

    if (classes) {
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

// void heuristic_1(DataSetFid *dataset, vector<vector<double>> hyperlocus, int nbThreadsUsed, int minCovering) {
//   cout << "Optimal FidexGlo" << endl
//        << endl;
//   int nbThreads = omp_get_num_procs();
//   // Get the rule for each data sample from fidex
//   cout << "Computing fidex rules..." << endl
//        << endl;
//   cout << nbThreads << " CPU cores available" << endl
//        << endl;
//   vector<Rule> rules;
//   int nbProblems = 0;
//   int nbRulesNotFound = 0;
//   vector<int> samplesNotFound;

// #pragma omp parallel num_threads(nbThreadsUsed)
//   {
//     int threadId = omp_get_thread_num();

// #pragma omp critical
//     cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;

//     Rule rule;
//     vector<Rule> localRules;
//     Hyperspace localHyperspace(hyperlocus);
//     double tStart, tEnd;
//     auto exp = FidexAlgo();
//     bool ruleCreated;
//     int nbDatas = dataset->getDatas()->size();
//     int counterFailed;
//     int currentMinNbCov = minCovering;
//     int localNbRulesNotFound = 0;
//     int localNbProblems = 0;
//     int localMinNbCover = 0;
//     int cnt = 0;
//     vector<int>::iterator it;
//     Antecedant a;

//     tStart = omp_get_wtime();

// #pragma omp for
//     for (int idSample = 0; idSample < nbDatas; idSample++) {
//       ruleCreated = false;
//       counterFailed = 0; // If we can't find a good rule after a lot of tries
//       cnt += 1;

//       if (omp_get_thread_num() == 0 && ((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && !consoleFileInit) {
//         cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
//         std::cout.flush();
//       }

//       while (!ruleCreated) {
//         ruleCreated = exp.fidex(rule,
//                                 dataset,
//                                 &localHyperspace,
//                                 idSample,
//                                 nbIn,
//                                 itMax,
//                                 currentMinNbCov,
//                                 minFidelity,
//                                 dropoutDimParam,
//                                 dropoutHypParam,
//                                 gen);

//         if (currentMinNbCov >= 2) {
//           currentMinNbCov -= 1;
//         } else {
//           counterFailed += 1;
//         }

//         if (counterFailed >= maxFailedAttempts) {
//           localNbRulesNotFound += 1;

// #pragma omp critical
//           {
//             it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
//             if (it != notCoveredSamples.end()) {
//               notCoveredSamples.erase(it);
//             }
//           }
//           break;
//         }
//       }

//       if (currentMinNbCov + 1 < minNbCover) {
//         localNbProblems += 1;
//       }
//       localRules.push_back(rule);

//       if (idSample == nbDatas - 1)
//         t2 = omp_get_wtime();
//     }

// // gathering local data to main process
// #pragma omp critical
//     {
//       cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << endl;
//       rules.insert(rules.end(), localRules.begin(), localRules.end());
//       nbProblems += localNbProblems;
//       nbRulesNotFound += localNbRulesNotFound;
//     }
//   } // end of parallel section

//   cout << rules.size() << " rules created." << endl;
//   cout << "Number of sample with lower covering than " << minNbCover << " is " << nbProblems << endl;
//   cout << "Number of rules not found is " << nbRulesNotFound << endl;
//   cout << "Fidex rules computed" << endl;
//   cout << "Computing global ruleset..." << endl;

//   nbRules = 0;

//   // While there is some not covered samples
//   Rule currentRule;
//   vector<int>::iterator it;
//   vector<int> currentRuleSamples;
//   vector<int> newNotCoveredSamples;
//   vector<int> bestNewNotCoveredSamples;

//   while (!notCoveredSamples.empty()) {
//     // Get rule that covers the most new samples
//     int bestRule = -1;
//     int bestCovering = INT_MAX;
//     int currentCovering; // Size of new covering if we choose this rule
//     for (int r = 0; r < rules.size(); r++) {
//       newNotCoveredSamples = notCoveredSamples;
//       // Remove samples that are in current covering

//       currentRuleSamples = rules[r].getCoveredSamples();

//       it = set_difference(newNotCoveredSamples.begin(),
//                           newNotCoveredSamples.end(),
//                           currentRuleSamples.begin(),
//                           currentRuleSamples.end(),
//                           newNotCoveredSamples.begin()); // vectors have to be sorted

//       newNotCoveredSamples.resize(it - newNotCoveredSamples.begin());
//       currentCovering = static_cast<int>(newNotCoveredSamples.size());

//       if (currentCovering < bestCovering) {
//         bestRule = r;
//         bestCovering = currentCovering;
//         bestNewNotCoveredSamples = newNotCoveredSamples;
//       }
//     }

//     currentRule = rules[bestRule];
//     notCoveredSamples = bestNewNotCoveredSamples; // Delete new covered samples
//     nbRules += 1;
//     chosenRules.push_back(currentRule);    // add best rule with maximum covering
//     rules.erase(rules.begin() + bestRule); // Remove this rule
//     if (rules.size() < 1)
//       break;
//   }
// }

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

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    size_t nbParam = commandList.size();

    // Parameters declaration

    int seed = 0;

    string trainDataFile;          // Train data
    string trainDataFilePred;      // Train class predictions from dimlp
    string trainDataFileTrueClass; // Train true classes
    string weightsFile;
    vector<string> weightsFiles;
    string inputRulesFile; // Rule file to be converted to hyperlocus, replace weight file
    string rulesFile;
    string consoleFile;
    string rootFolder;
    string attributFile; // attribut file

    int nbDimlpNets = 1; // Number of networks. 1 means no bagging

    int nbQuantLevels = 50; // Number of steps of the step function
    double hiKnot = 5;      // High end of the interval for each dimension, a hyperplan can't be after

    int heuristic = -1;
    bool heuristicInit = false;

    int itMax = 10;     // We stop if we have more than itMax iterations (impossible to have a rule with more than itMax antecedents)
    int minNbCover = 2; // Minimum size of covering that we ask
    double dropoutHyp = 0.5;
    double dropoutDim = 0.5;
    int maxFailedAttempts = 30;
    int nbThreadsUsed = 1;
    double minFidelity = 1;

    bool hasDecisionThreshold = false;
    double decisionThreshold = -1;
    bool hasIndexPositiveClass = false;
    int indexPositiveClass = -1;

    int nbRules; // Number of rules created

    // Import parameters

    if (nbParam <= 1) {
      showRulesParams();
      return 0;
    }

    int p = 1; // We skip "fidexGloRules"
    while (p < nbParam) {
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        char option = commandList[p - 1][1];
        const char *arg = &(commandList[p])[0];
        const char *lastArg = &(commandList[p - 1])[0];
        switch (option) { // Get letter after the -

        case 'T':
          trainDataFile = arg; // Parameter after -T
          break;

        case 'P':
          trainDataFilePred = arg;
          break;

        case 'C':
          trainDataFileTrueClass = arg;
          break;

        case 'W': {
          weightsFile = arg;
        } break;

        case 'f': {
          inputRulesFile = arg;
        } break;

        case 'N':
          if (CheckPositiveInt(arg))
            nbDimlpNets = atoi(arg);
          else
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");

          break;

        case 'Q':
          if (CheckPositiveInt(arg)) {
            nbQuantLevels = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'O':
          rulesFile = arg;
          break;

        case 'r':
          consoleFile = arg;
          break;

        case 'A':
          attributFile = arg;
          break;

        case 'S':
          rootFolder = arg;
          break;

        case 'M':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1 && atoi(arg) <= 3) {
            heuristic = atoi(arg);
            heuristicInit = true;
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", heuristic must be an integer between 1 and 3(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo)");
          }
          break;

        case 'i':
          if (CheckPositiveInt(arg)) {
            itMax = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'v':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
            minNbCover = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", strictly positive integer requested");
          }
          break;

        case 'p':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
            if (atoi(arg) <= omp_get_num_procs()) {
              nbThreadsUsed = atoi(arg);
            }
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", zero or positive integer requested");
          }
          break;

        case 'y':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            minFidelity = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'd':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutDim = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'h':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutHyp = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'm':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            maxFailedAttempts = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", strictly positive integer requested");
          }
          break;

        case 't':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            hasDecisionThreshold = true;
            decisionThreshold = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'x':
          if (CheckPositiveInt(arg)) {
            hasIndexPositiveClass = true;
            indexPositiveClass = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'z':
          if (CheckPositiveInt(arg))
            seed = atoi(arg);
          else
            throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");

          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option : " + string(lastArg));
        }
      }

      p++;
    }

    // Fill weights vector
    if (!weightsFile.empty()) {
      if (nbDimlpNets == 1) {
        weightsFiles.push_back(weightsFile);
      } else {
        for (int n = 0; n < nbDimlpNets; n++) {
          weightsFiles.push_back(weightsFile + to_string(n + 1) + ".wts");
        }
      }
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    string root = "";
    if (!rootFolder.empty()) {
#if defined(__unix__) || defined(__APPLE__)
      root = rootFolder + "/";
#elif defined(_WIN32)
      root = rootFolder + "\\";
#endif
    }

    if (!trainDataFile.empty()) {
      trainDataFile = root + trainDataFile;
    } else {
      throw CommandArgumentException("The train data file has to be given with option -T");
    }

    if (!trainDataFilePred.empty()) {
      trainDataFilePred = root + trainDataFilePred;
    } else {
      throw CommandArgumentException("The train prediction file has to be given with option -P");
    }

    if (!trainDataFileTrueClass.empty()) {
      trainDataFileTrueClass = root + trainDataFileTrueClass;
    } else {
      throw CommandArgumentException("The train true classes file has to be given with option -C");
    }

    if (!rulesFile.empty()) {
      rulesFile = root + rulesFile;
    } else {
      throw CommandArgumentException("The output rules file has to be given with option -O");
    }

    if (!weightsFile.empty()) {
      for (int i = 0; i < weightsFiles.size(); i++) {
        weightsFiles[i] = root + weightsFiles[i];
      }
    }

    if (!inputRulesFile.empty()) {
      inputRulesFile = root + inputRulesFile;
    }

    if (weightsFile.empty() && inputRulesFile.empty()) {
      throw CommandArgumentException("A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
    } else if (!weightsFile.empty() && !inputRulesFile.empty()) {
      throw CommandArgumentException("Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
    }

    if (!consoleFile.empty()) {
      consoleFile = root + consoleFile;
    }

    if (!attributFile.empty()) {
      attributFile = root + attributFile;
    }

    if (hasDecisionThreshold && !hasIndexPositiveClass) {
      throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t)");
    }

    // ----------------------------------------------------------------------

    if (!heuristicInit) {
      throw CommandArgumentException("The heuristic(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo) has to be given with option -M");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (!consoleFile.empty()) {
      ofs.open(consoleFile);
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // ----------------------------------------------------------------------

    cout << "\nParameters :" << endl;
    cout << "\t- Max iteration number : " << itMax << endl;
    cout << "\t- Min size of covering : " << minNbCover << endl;

    if (dropoutDim > 0.0001f) {
      cout << "\t- Dimension dropout of " << dropoutDim << " is used" << endl;
    } else {
      cout << "\t- No dimension dropout used" << endl;
    }

    if (dropoutHyp > 0.0001f) {
      cout << "\t- Hyperplan dropout of " << dropoutHyp << " is used" << endl;
    } else {
      cout << "\t- No hyperplan dropout" << endl;
    }
    cout << endl;

    // Import files
    cout << "Importing files..." << endl;

    unique_ptr<DataSetFid> trainDatas(new DataSetFid("trainDatas from FidexGloRules",
                                                     trainDataFile.c_str(),
                                                     trainDataFilePred.c_str(),
                                                     hasDecisionThreshold,
                                                     decisionThreshold,
                                                     indexPositiveClass,
                                                     trainDataFileTrueClass.c_str()));

    vector<vector<double>> *trainData = trainDatas->getDatas();
    vector<int> *trainPreds = trainDatas->getPredictions();

    vector<vector<double>> *trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();

    vector<int> *trainTrueClass = trainDatas->getClasses();

    int nbDatas = trainDatas->getNbSamples();
    int nbAttributs = trainDatas->getNbAttributes();

    if (minNbCover > nbDatas) {
      throw CommandArgumentException("Error : invalid type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
    }

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (!attributFile.empty()) {
      trainDatas->setAttribute(attributFile.c_str());
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

    if (!weightsFile.empty()) {
      if (nbDimlpNets > 1) {
        cout << "\nParameters of hyperLocus :"
             << endl;
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
    if (seed == 0) {
      auto currentTime = high_resolution_clock::now();
      auto seedValue = currentTime.time_since_epoch().count();
      seed = static_cast<unsigned int>(seedValue);
    }
    cout << "Seed used: " << seed << endl;
    mt19937 gen(seed);

    // First heuristic : optimal (slower)

    const auto start = high_resolution_clock::now();
    if (heuristic == 1) {
      cout << "Optimal FidexGlo" << endl
           << endl;

      int nbThreads = omp_get_num_procs();

      // Get the rule for each data sample from fidex
      cout << "Computing fidex rules..." << endl
           << endl;
      cout << nbThreads << " CPU cores available" << endl
           << endl;

      vector<Rule> rules;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      vector<int> samplesNotFound;

#pragma omp parallel num_threads(nbThreadsUsed)
      {

        // declaring thread internal variables
        int threadId = omp_get_thread_num();

#pragma omp critical
        cout << "Thread #" << threadId << " initialized, please wait for it to be done." << endl;

        Rule rule;
        vector<Rule> localRules;
        Hyperspace localHyperspace(matHypLocus);
        auto exp = FidexAlgo();
        bool ruleCreated;
        int counterFailed;
        int idSample;
        int currentMinNbCov = minNbCover;
        int localNbRulesNotFound = 0;
        int localNbProblems = 0;
        int localMinNbCover = 0;
        int cnt = 0;
        vector<int>::iterator it;
        Antecedant a;

        t1 = omp_get_wtime();

#pragma omp for
        for (idSample = 0; idSample < nbDatas; idSample++) {
          ruleCreated = false;
          counterFailed = 0; // If we can't find a good rule after a lot of tries
          cnt += 1;

          if (omp_get_thread_num() == 0 && ((nbDatas / nbThreadsUsed) / 100) != 0 && (idSample % int((nbDatas / nbThreadsUsed) / 100)) == 0 && consoleFile.empty()) {
            cout << "Processing : " << int((double(idSample) / (nbDatas / nbThreadsUsed)) * 100) << "%\r";
            std::cout.flush();
          }

          while (!ruleCreated) {
            ruleCreated = exp.fidex(rule,
                                    trainDatas.get(),
                                    &localHyperspace,
                                    idSample,
                                    nbIn,
                                    itMax,
                                    currentMinNbCov,
                                    minFidelity,
                                    dropoutDim,
                                    dropoutHyp,
                                    gen);

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

          if (idSample == nbDatas - 1)
            t2 = omp_get_wtime();
        }

// gathering local data to main process
#pragma omp critical
        {
          cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << endl;
          rules.insert(rules.end(), localRules.begin(), localRules.end());
          nbProblems += localNbProblems;
          nbRulesNotFound += localNbRulesNotFound;
        }
      } // end of parallel section

      cout << rules.size() << " rules created." << endl;
      cout << "Number of sample with lower covering than " << minNbCover << " is " << nbProblems << endl;
      cout << "Number of rules not found is " << nbRulesNotFound << endl;
      cout << "Fidex rules computed" << endl;
      cout << "Computing global ruleset..." << endl;

      nbRules = 0;

      // While there is some not covered samples
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

      cout << "Global ruleset Computed" << endl
           << endl;

      cout << "We created " << nbRules << " rules." << endl
           << endl;

      const auto end_h1 = high_resolution_clock::now();
      const duration<double> diff = end_h1 - start;

      cout << "\nTime first heuristic = " << diff.count() << " sec\n"
           << endl;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Second heuristic : fast
    float temps2;
    clock_t d1;
    clock_t d2;
    Hyperspace hyperspace(matHypLocus);

    d1 = clock();
    if (heuristic == 2) {
      cout << "Fast FidexGlo" << endl
           << endl;

      vector<Rule> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      int currentMinNbCov;
      Rule rule;
      auto exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      cout << "Computing fidex rules..." << endl
           << endl;
      for (int idSample = 0; idSample < nbDatas; idSample++) {

        if (int(nbDatas / 100) != 0 && idSample % int(nbDatas / 100) == 0 && consoleFile.empty()) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%\r";
          cout.flush();
        }
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        int counterFailed = 0; // If we can't find a good rule after a lot of tries
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule,
                                  trainDatas.get(),
                                  &hyperspace,
                                  idSample,
                                  nbIn,
                                  itMax,
                                  currentMinNbCov,
                                  minFidelity,
                                  dropoutDim,
                                  dropoutHyp,
                                  gen);

          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          } else {
            counterFailed += 1;
          }
          if (counterFailed >= maxFailedAttempts) {
            nbRulesNotFound += 1;
            auto it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
            if (it != notCoveredSamples.end()) {
              notCoveredSamples.erase(it);
            }
            break;
          }
        }
        if (currentMinNbCov + 1 < minNbCover) {
          nbProblems += 1;
        }
        if (ruleCreated) {
          rules.push_back(rule);
        }
      }

      cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
      if (nbRulesNotFound > 0) {
        cout << "Number of rules not found : " << nbRulesNotFound << endl;
      }

      cout << "Fidex rules computed" << endl
           << endl;

      cout << "Computing global ruleset..." << endl
           << endl;

      // Sort the rules(dataIds) depending of their covering size
      vector<int> dataIds = notCoveredSamples;
      sort(dataIds.begin(), dataIds.end(), [&rules](int ruleBest, int ruleWorst) {
        return rules[ruleWorst].getCoveredSamples().size() < rules[ruleBest].getCoveredSamples().size();
      });

      nbRules = 0;

      // While there is some not covered samples
      Rule currentRule;
      auto ancienNotCoveringSize = static_cast<int>(notCoveredSamples.size());
      int dataId = 0;

      vector<int>::iterator ite;
      while (!notCoveredSamples.empty()) {
        currentRule = rules[dataIds[dataId]];

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

      cout << "Global ruleset Computed" << endl
           << endl;

      cout << "We created " << nbRules << " rules." << endl
           << endl;

      d2 = clock();
      temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;
      cout << "\nTime second heuristic = " << temps2 << " sec\n"
           << endl;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Third heuristic : Very fast
    float temps3;
    clock_t e1;
    clock_t e2;

    e1 = clock();

    if (heuristic == 3) {
      cout << "Very fast FidexGlo" << endl
           << endl;

      //  Sort data randomly
      shuffle(begin(notCoveredSamples), end(notCoveredSamples), gen);

      nbRules = 0;
      int idSample;
      bool ruleCreated;
      int currentMinNbCov;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      Rule rule; // Ex: ([X2<3.5 X3>=4], covering, class, accuracy, confidence)
      auto exp = FidexAlgo();

      cout << "Computing rules..." << endl
           << endl;

      // While there is some not covered samples
      while (!notCoveredSamples.empty()) {

        if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && consoleFile.empty()) {
          cout << "Processing : " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%r";
          cout.flush();
        }

        idSample = notCoveredSamples[0];
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        int counterFailed = 0; // If we can't find a good rule after a lot of tries
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule,
                                  trainDatas.get(),
                                  &hyperspace,
                                  idSample,
                                  nbIn,
                                  itMax,
                                  currentMinNbCov,
                                  minFidelity,
                                  dropoutDim,
                                  dropoutHyp,
                                  gen);

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

      cout << "We created " << nbRules << " rules." << endl
           << endl;

      e2 = clock();
      temps3 = (float)(e2 - e1) / CLOCKS_PER_SEC;
      cout << "\nTime third heuristic = " << temps3 << " sec\n"
           << endl;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    cout << "Rules extraction..."
         << endl
         << endl;

    sort(chosenRules.begin(), chosenRules.end(), [&chosenRules](Rule r1, Rule r2) {
      return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
    });

    tuple<int, int> stats = writeRulesFile(rulesFile, chosenRules, &attributeNames, &classNames);

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
