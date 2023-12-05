#include "fidexGloRulesFct.h"

using namespace std;
using namespace FidexGloNameSpace;

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
  cout << "-Q <number of stairs in staircase activation function (50 by default)>" << endl;
  cout << "-t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << endl;
  cout << "-x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << endl;
  cout << "-z <seed (0=random, default)>";

  cout << "\n-------------------------------------------------\n"
       << endl;
}

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

    string trainDataFileTemp; // Train data
    bool trainDataFileInit = false;
    string trainDataFilePredTemp; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    string trainDataFileTrueClassTemp; // Train true classes
    bool trainDataFileTrueClassInit = false;

    string weightsFileTemp;
    vector<string> weightsFilesTemp;
    string inputRulesFileTemp; // Rule file to be converted to hyperlocus, replace weight file
    bool inputRulesFileInit = false;

    bool weightsFileInit = false;
    string rulesFileTemp;
    bool rulesFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;

    int nbDimlpNets = 1; // Number of networks. 1 means no bagging

    int nbQuantLevels = 50; // Number of steps of the step function
    double hiKnot = 5;      // High end of the interval for each dimension, a hyperplan can't be after

    int heuristic = -1;
    bool heuristicInit = false;

    int itMax = 100;         // We stop if we have more than itMax iterations
    int minNbCover = 2;      // Minimum size of covering that we ask
    bool dropoutHyp = false; // We dropout a bunch of hyperplans each iteration (could accelerate the processus)
    double dropoutHypParam = 0.5;
    bool dropoutDim = false; // We dropout a bunch of dimensions each iteration (could accelerate the processus)
    double dropoutDimParam = 0.5;

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
          trainDataFileTemp = arg; // Parameter after -T
          trainDataFileInit = true;
          break;

        case 'P':
          trainDataFilePredTemp = arg;
          trainDataFilePredInit = true;
          break;

        case 'C':
          trainDataFileTrueClassTemp = arg;
          trainDataFileTrueClassInit = true;
          break;

        case 'W': {
          weightsFileTemp = arg;
          weightsFileInit = true;
        } break;

        case 'f': {
          inputRulesFileTemp = arg;
          inputRulesFileInit = true;
        } break;

        case 'N':
          if (CheckPositiveInt(arg))
            nbDimlpNets = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested");

          break;

        case 'Q':
          if (CheckPositiveInt(arg)) {
            nbQuantLevels = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'O':
          rulesFileTemp = arg;
          rulesFileInit = true;
          break;

        case 'r':
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case 'A':
          attributFileTemp = arg;
          attributFileInit = true;
          break;

        case 'S':
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case 'M':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1 && atoi(arg) <= 3) {
            heuristic = atoi(arg);
            heuristicInit = true;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", heuristic must be an integer between 1 and 3(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo)");
          }
          break;

        case 'i':
          if (CheckPositiveInt(arg)) {
            itMax = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'v':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
            minNbCover = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested");
          }
          break;

        case 'd':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutDimParam = atof(arg);
            dropoutDim = true; // We dropout a bunch of dimensions each iteration (accelerate the processus)
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'h':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutHypParam = atof(arg);
            dropoutHyp = true; // We dropout a bunch of hyperplans each iteration (accelerate the processus)
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 't':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            hasDecisionThreshold = true;
            decisionThreshold = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested");
          }
          break;

        case 'x':
          if (CheckPositiveInt(arg)) {
            hasIndexPositiveClass = true;
            indexPositiveClass = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested");
          }
          break;

        case 'z':
          if (CheckPositiveInt(arg))
            seed = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested");

          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option : " + string(lastArg));
        }
      }

      p++;
    }

    // Fill weights vector
    if (weightsFileInit) {
      if (nbDimlpNets == 1) {
        weightsFilesTemp.push_back(weightsFileTemp);
      } else {
        for (int n = 0; n < nbDimlpNets; n++) {
          weightsFilesTemp.push_back(weightsFileTemp + to_string(n + 1) + ".wts");
        }
      }
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *trainDataFile = nullptr;
    const char *trainDataFilePred = nullptr;
    const char *trainDataFileTrueClass = nullptr;
    const char *rulesFile = nullptr;
    vector<const char *> weightsFiles;
    const char *inputRulesFile = nullptr;
    const char *consoleFile = nullptr;
    const char *attributFile = nullptr;

    string root = "";
    if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
      root = rootFolderTemp + "/";
#elif defined(_WIN32)
      root = rootFolderTemp + "\\";
#endif
    }

    if (trainDataFileInit) {
      trainDataFileTemp = root + trainDataFileTemp;
      trainDataFile = &trainDataFileTemp[0];
    }

    if (trainDataFilePredInit) {
      trainDataFilePredTemp = root + trainDataFilePredTemp;
      trainDataFilePred = &trainDataFilePredTemp[0];
    }

    if (trainDataFileTrueClassInit) {
      trainDataFileTrueClassTemp = root + trainDataFileTrueClassTemp;
      trainDataFileTrueClass = &trainDataFileTrueClassTemp[0];
    }

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      rulesFile = &rulesFileTemp[0];
    }

    if (weightsFileInit) {
      for (int i; i < weightsFilesTemp.size(); i++) {
        weightsFilesTemp[i] = root + weightsFilesTemp[i];
        weightsFiles.push_back(&weightsFilesTemp[i][0]);
      }
    }

    if (inputRulesFileInit) {
      inputRulesFileTemp = root + inputRulesFileTemp;
      inputRulesFile = &inputRulesFileTemp[0];
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }

    if (attributFileInit) {
      attributFileTemp = root + attributFileTemp;
      attributFile = &attributFileTemp[0];
    }

    if (hasDecisionThreshold && !hasIndexPositiveClass) {
      throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t)");
    }

    // ----------------------------------------------------------------------

    if (!trainDataFileInit) {
      throw CommandArgumentException("The train data file has to be given with option -T");
    }
    if (!trainDataFilePredInit) {
      throw CommandArgumentException("The train prediction file has to be given with option -P");
    }
    if (!trainDataFileTrueClassInit) {
      throw CommandArgumentException("The train true classes file has to be given with option -C");
    }
    if (!weightsFileInit && !inputRulesFileInit) {
      throw CommandArgumentException("A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
    } else if (weightsFileInit && inputRulesFileInit) {
      throw CommandArgumentException("Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
    }
    if (!rulesFileInit) {
      throw CommandArgumentException("The output rules file has to be given with option -O");
    }
    if (!heuristicInit) {
      throw CommandArgumentException("The heuristic(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo) has to be given with option -M");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // ----------------------------------------------------------------------

    cout << "\nParameters :\n"
         << endl;
    cout << "- Max iteration number : " << itMax << endl;
    cout << "- Min size of covering : " << minNbCover << endl;
    if (dropoutDim) {
      cout << "- We use a dimension dropout of " << dropoutDimParam << endl
           << endl;
    } else {
      cout << "- We don't use dimension dropout\n"
           << endl;
    }
    if (dropoutHyp) {
      cout << "- We use a hyperplan dropout of " << dropoutHypParam << endl
           << endl;
    } else {
      cout << "- We don't use hyperplan dropout\n"
           << endl;
    }

    // Import files

    cout << "Import files..." << endl;

    unique_ptr<DataSetFid> trainDatas(new DataSetFid(trainDataFile, trainDataFilePred, hasDecisionThreshold, decisionThreshold, indexPositiveClass, trainDataFileTrueClass));

    vector<vector<double>> *trainData = trainDatas->getDatas();
    vector<int> *trainPreds = trainDatas->getPredictions();

    vector<vector<double>> *trainOutputValuesPredictions = nullptr;
    bool hasConfidence;
    if (trainDatas->hasConfidence()) {
      trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
      hasConfidence = true;
    } else {
      hasConfidence = false;
    }

    vector<int> *trainTrueClass = trainDatas->getTrueClasses();

    const auto nbDatas = static_cast<int>((*trainData).size());
    const auto nbAttributs = static_cast<int>((*trainData)[0].size());
    const auto nbClass = trainDatas->getNbClasses();

    if (indexPositiveClass >= nbClass) {
      throw CommandArgumentException("Error : parameter positive_index(-x) has to be a positive integer smaller than " + to_string(nbClass));
    }

    if ((*trainPreds).size() != nbDatas || (*trainTrueClass).size() != nbDatas) {
      throw FileFormatError("All the train files need to have the same amount of datas");
    }

    if (minNbCover > nbDatas) {
      throw CommandArgumentException("Error : invalide type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
    }

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (attributFileInit) {
      unique_ptr<Attribute> attributesData(new Attribute(attributFile));
      attributeNames = (*attributesData->getAttributes());
      if (attributeNames.size() < nbAttributs) {
        throw FileContentError("Error : in file " + string(attributFile) + ", there is not enough attribute names");
      } else if (attributeNames.size() == nbAttributs) {
        hasClassNames = false;
      } else if (attributeNames.size() != nbAttributs + nbClass) {
        throw FileContentError("Error : in file " + string(attributFile) + ", there is not the good amount of attribute and class names");
      } else {
        hasClassNames = true;
        auto firstEl = attributeNames.end() - nbClass;
        auto lastEl = attributeNames.end();
        classNames.insert(classNames.end(), firstEl, lastEl);
        attributeNames.erase(firstEl, lastEl);
      }
    }

    cout << "Files imported" << endl
         << endl;

    // compute hyperspace

    cout << "Creation of hyperspace..." << endl;

    vector<vector<double>> matHypLocus;

    if (weightsFileInit) {
      if (nbDimlpNets > 1) {
        cout << "\nParameters of hyperLocus :\n"
             << endl;
        cout << "- Number of stairs " << nbQuantLevels << endl;
        cout << "- Interval : [-" << hiKnot << "," << hiKnot << "]" << endl
             << endl;
        cout << "Computation of all hyperlocus" << endl;
      }
      for (const auto &weightsFile : weightsFiles) {
        vector<vector<double>> hypLocus;
        if (nbDimlpNets > 1) {
          hypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot, false); // Get hyperlocus
        } else {
          hypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
        }
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end()); // Concatenate hypLocus to matHypLocus
      }
      if (nbDimlpNets > 1) {
        cout << "All hyperlocus created" << endl;
      }
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, nbAttributs);
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
      auto currentTime = chrono::high_resolution_clock::now();
      auto seedValue = currentTime.time_since_epoch().count();
      seed = static_cast<unsigned int>(seedValue);
    }
    mt19937 gen(seed);

    // First heuristic : optimal (slower)
    float temps1;
    clock_t c1;
    clock_t c2;

    c1 = clock();
    if (heuristic == 1) {
      cout << "Optimal FidexGlo" << endl
           << endl;

      vector<Rule> rules;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      int nbThreads = omp_get_num_procs();
      int usedThreads = nbThreads;

      // Get the rule for each data sample from fidex
      cout << "Computing fidex rules..." << endl
           << endl;
      cout << nbThreads << " CPU cores available" << endl
           << endl;

#pragma omp parallel num_threads(usedThreads)
      {

        // declaring thread internal variables
        int threadId = omp_get_thread_num();
        int startIndex = (nbDatas / usedThreads) * threadId;
        int endIndex = ((threadId + 1) < usedThreads ? startIndex + nbDatas / usedThreads : nbDatas);

        // creating subvectors of data, in order to avoid concurrent accesses with shared memory data
        // vector<vector<double>> localTrainData(trainData->begin() + startIndex, trainData->begin() + endIndex);
        // vector<int> localTrainPreds(trainPreds->begin() + startIndex, trainPreds->begin() + endIndex);
        // vector<int> localTrainTrueClass(trainTrueClass->begin() + startIndex, trainTrueClass->begin() + endIndex);

#pragma omp critical
        {
          cout << "Thread #" << threadId << " working on " << (endIndex - startIndex) << " values. Interval is [" << startIndex << ":" << endIndex << "]" << endl;
          // cout << "Local vector size check:" << endl;
          // cout << "   localTrainData size:      " << localTrainData.size() << endl;
          // cout << "   localTrainPreds size:     " << localTrainPreds.size() << endl;
          // cout << "   localTrainTrueClass size: " << localTrainTrueClass.size() << endl
          //  << endl;
        }

        Rule rule; // Ex: ([X2<3.5 X3>=4], covering, class)
        vector<Rule> localRules;
        Hyperspace localHyperspace(matHypLocus);
        auto exp = FidexAlgo();
        bool ruleCreated;
        int counterFailed;
        int idSample;
        int currentMinNbCov = minNbCover;
        int localNbRulesNotFound = 0;
        int localNbDatas = endIndex - startIndex;
        int localNbProblems = 0;
        int localMinNbCover = 0;
        vector<int>::iterator it;
        Antecedant a;
        double t1;
        double t2;

        t1 = omp_get_wtime();

#pragma omp for
        for (idSample = 0; idSample < nbDatas; idSample++) {
          // #pragma omp critical
          //           {
          //             // cout << "T" << threadId << " [" << startIndex << " to " << endIndex << "]" << endl;
          //             cout << "T" << threadId << " " << idSample << endl;
          //           }

          ruleCreated = false;
          counterFailed = 0; // If we can't find a good rule after a lot of tries

          while (!ruleCreated) {
            ruleCreated = exp.fidex(
                rule,
                trainData,
                trainPreds,
                hasConfidence,
                trainOutputValuesPredictions,
                trainTrueClass,
                &(*trainData)[idSample],
                (*trainPreds)[idSample],
                &localHyperspace,
                nbIn,
                nbAttributs,
                itMax,
                currentMinNbCov,
                dropoutDim,
                dropoutDimParam,
                dropoutHyp,
                dropoutHypParam,
                gen);

            if (currentMinNbCov >= 2) {
              currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
            } else {
              counterFailed += 1;
            }

            if (counterFailed >= 10) {
              localNbRulesNotFound += 1;

              // notCoveredSamples is shared
#pragma omp critical
              {
                it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
                if (it != notCoveredSamples.end()) {
                  notCoveredSamples.erase(it);
                }
              }
              cout << "break" << endl;
              break;
            }
          }

          if (currentMinNbCov + 1 < minNbCover) {
            localNbProblems += 1;
          }
          // cout << rule << endl;
          localRules.push_back(rule);
          // cout << "Done" << endl;
          if (idSample > nbDatas - 2)
            t2 = omp_get_wtime();
        }

// gathering local data to main process
#pragma omp critical
        {
          cout << "T" << threadId << " ended in " << (t2 - t1) << " seconds." << endl;
          cout << "Local rule size: (#T" << threadId << ") " << localRules.size() << endl;
          for (Rule r : localRules) {
            cout << r << endl;
            for (int cs : r.getCoveredSamples()) {
              cout << cs << ", ";
            }
            cout << endl
                 << "-----------------------" << endl;
          }
          cout << endl;
          rules.insert(rules.end(), localRules.begin(), localRules.end());
          cout << "Current rules size: " << rules.size() << endl;
          nbProblems += localNbProblems;
          nbRulesNotFound += localNbRulesNotFound;
          cout << "Thread #" << threadId << " is done." << endl;
        }
      } // end of parallel section

      cout << "Total rules size: " << rules.size() << endl;
      cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
      if (nbRulesNotFound > 0) {
        cout << "Number of rules not found : " << nbRulesNotFound << endl;
      }

      cout << "Fidex rules computed" << endl;
      cout << "Computing global ruleset..." << endl;

      nbRules = 0;

      // While there is some not covered samples
      Rule currentRule;
      vector<int>::iterator ite;
      vector<int> currentRuleSamples;
      vector<int> newNotCoveredSamples;
      vector<int> bestNewNotCoveredSamples;

      while (!notCoveredSamples.empty()) {
        // Get rule that covers the most new samples
        int bestRule = -1;
        int bestCovering = INT_MAX;
        int currentCovering; // Size of new covering if we choose this rule
        for (int r = 0; r < rules.size(); r++) {
          // cout << "r: " << r << " rules.size(): " << rules.size() << endl;
          newNotCoveredSamples = notCoveredSamples;
          // Remove samples that are in current covering

          currentRuleSamples = rules[r].getCoveredSamples();

          ite = set_difference(newNotCoveredSamples.begin(),
                               newNotCoveredSamples.end(),
                               currentRuleSamples.begin(),
                               currentRuleSamples.end(),
                               newNotCoveredSamples.begin()); // vectors have to be sorted

          newNotCoveredSamples.resize(ite - newNotCoveredSamples.begin());
          currentCovering = static_cast<int>(newNotCoveredSamples.size());

          // cout << "New not covered samples size: " << currentCovering << endl;
          // cout << currentCovering << " <? " << bestCovering << endl;

          if (currentCovering < bestCovering) {
            bestRule = r;
            bestCovering = currentCovering;
            bestNewNotCoveredSamples = newNotCoveredSamples;
            // cout << "\033[1;31m"
            //  << "Best rule: " << bestRule << " best covering: " << bestCovering << " new NCS size: " << bestNewNotCoveredSamples.size() << "\033[0m" << endl;
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

      c2 = clock();
      temps1 = (float)(c2 - c1) / CLOCKS_PER_SEC;
      cout << "\nTime first heuristic = " << temps1 << " sec\n"
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

        if (int(nbDatas / 100) != 0 && idSample % int(nbDatas / 100) == 0 && consoleFileInit == false) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%\r";
          cout.flush();
        }
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        int counterFailed = 0; // If we can't find a good rule after a lot of tries
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule, trainData, trainPreds, hasConfidence, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], &hyperspace, nbIn, nbAttributs, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam, gen);
          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          } else {
            counterFailed += 1;
          }
          if (counterFailed >= 30) {
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
        ite = set_difference(notCoveredSamples.begin(), notCoveredSamples.end(), currentRule.getCoveredSamples().begin(), currentRule.getCoveredSamples().end(), notCoveredSamples.begin()); // vectors have to be sorted
        notCoveredSamples.resize(ite - notCoveredSamples.begin());

        // If the rule coveres a new sample
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

        if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && consoleFileInit == false) {
          cout << "Processing : " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%r";
          cout.flush();
        }

        idSample = notCoveredSamples[0];
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        int counterFailed = 0; // If we can't find a good rule after a lot of tries
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule, trainData, trainPreds, hasConfidence, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], &hyperspace, nbIn, nbAttributs, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam, gen);
          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          } else {
            counterFailed += 1;
          }
          if (counterFailed >= 30) {
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

    cout << "Rules extraction..." << endl
         << endl;

    double meanCovSize = 0;
    double meanNbAntecedents = 0;

    // Export rules
    string inequality;
    string line;
    vector<string> lines;
    // Sort the rules(RulesIds) depending of their covering size
    vector<int> RulesIds(nbRules);           // Rules indexes
    iota(begin(RulesIds), end(RulesIds), 0); // Vector from 0 to nbRules-1
    sort(RulesIds.begin(), RulesIds.end(), [&chosenRules](int ruleBest, int ruleWorst) {
      return chosenRules[ruleWorst].getCoveredSamples().size() < chosenRules[ruleBest].getCoveredSamples().size();
    });

    lines.emplace_back("\n");
    for (int c = 0; c < nbRules; c++) { // each rule
      int r = RulesIds[c];
      meanCovSize += chosenRules[r].getCoveredSamples().size();
      meanNbAntecedents += chosenRules[r].getAntecedants().size();
      line = "Rule " + to_string(c + 1) + ": ";

      for (Antecedant a : chosenRules[r].getAntecedants()) { // each antecedant
        if (a.getInequality()) {                             // check inequality
          inequality = ">=";
        } else {
          inequality = "<";
        }
        if (attributFileInit) {
          line += attributeNames[a.getAttribute()];
        } else {
          line += "X" + to_string(a.getAttribute());
        }
        line += inequality + to_string(a.gethyperlocus()) + " ";
      }

      // class of the rule
      if (hasClassNames) {
        line += "-> " + classNames[chosenRules[r].getOutputClass()];
      } else {
        line += "-> class " + to_string(chosenRules[r].getOutputClass());
      }

      line += "\n";
      lines.push_back(line);
      line = "Train Covering size : " + to_string(chosenRules[r].getCoveredSamples().size()) + "\n"; // Covering size
      lines.push_back(line);
      line = "Train Fidelity : 1\n"; // Rule fidelity
      lines.push_back(line);
      line = "Train Accuracy : " + to_string(chosenRules[r].getAccuracy()) + "\n"; // Rule accuracy
      lines.push_back(line);

      if (hasConfidence) {
        line = "Train Confidence : " + to_string(chosenRules[r].getConfidence()) + "\n"; // Rule confidence
      } else {
        line = "\n";
      }

      lines.push_back(line);
      lines.emplace_back("\n");
    }

    meanCovSize /= nbRules;
    meanNbAntecedents /= nbRules;

    ofstream file2(rulesFile);

    if (file2.is_open()) {
      // We add to the file the number of rules, the mean covering per rule and the mean number of antecedents per rule
      string startLine = "Number of rules : " + to_string(nbRules) + ", mean sample covering number per rule : " + to_string(meanCovSize) + ", mean number of antecedents per rule : " + to_string(meanNbAntecedents) + "\n";
      file2 << startLine;
      string secondLine = "The name of the attributes and classes are not specified\n";
      if (attributFileInit) {
        secondLine = "The name of the attributes is specified\n";
        if (hasClassNames) {
          secondLine = "The name of the attributes and classes are specified\n";
        }
      }
      file2 << secondLine;
      for (const auto &l : lines) {
        file2 << l;
      }

      file2.close();

    } else {
      throw CannotOpenFileError("Error : Couldn't open rules extraction file " + string(rulesFile) + ".");
    }

    cout << "Mean covering size per rule : " << meanCovSize << endl;
    cout << "Mean number of antecedents per rule : " << meanNbAntecedents << endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    cout << "\nFull execution time = " << temps << " sec" << endl;

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
