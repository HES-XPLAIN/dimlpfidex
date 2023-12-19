#include "fidexGloFct.h"

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;

  std::cout << "fidexGlo -S <test sample(s) data file with data and prediction(if no -p)> ";
  std::cout << "-R <ruleset input file> ";
  std::cout << "-a <number of attributes>";
  std::cout << "-b <number of classes>";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "-F <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "-A <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it" << std::endl;
  std::cout << "-p <test prediction file> if given, -S needs to have only test datas" << std::endl;
  std::cout << "-O <Explanation(s) output file>" << std::endl;
  std::cout << "-r <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "-t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "-x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << std::endl;
  std::cout << "-w <with Fidex if not rule is given (False by default)>" << std::endl;
  std::cout << "-M <minimal version, only correct activated rules, launch Fidex when no such rule is found(if with fidex) (False by default)>" << std::endl
            << std::endl;
  std::cout << "If using fidex :" << std::endl;
  std::cout << "Obligatory :" << std::endl;
  std::cout << "-T <train dataset file>" << std::endl;
  std::cout << "-P <train prediction file>" << std::endl;
  std::cout << "-C <train true class file>" << std::endl;
  std::cout << "-c <test true class file>" << std::endl;
  std::cout << "-W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f] " << std::endl;
  std::cout << "-f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] " << std::endl;
  std::cout << "Options :" << std::endl;
  std::cout << "-N <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << std::endl;
  std::cout << "-i <max iteration number (100 by default)>" << std::endl;
  std::cout << "-v <minimum covering number (2 by default)>" << std::endl;
  std::cout << "-y <decrement by 1 the min covering number each time the minimal covering size is not reached (False by default)>" << std::endl;
  std::cout << "-m <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>" << std::endl;
  std::cout << "-q <minimum Fidelity to obtain when computing a rule (1 by default)>" << std::endl;
  std::cout << "-d <dimension dropout parameter (None by default)>" << std::endl;
  std::cout << "-h <hyperplan dropout parameter (None by default)>" << std::endl;
  std::cout << "-Q <number of stairs in staircase activation function (50 by default)>" << std::endl;
  std::cout << "-z <seed (0=random, default)>";

  std::cout << "\n-------------------------------------------------\n"
            << std::endl;
}

void launchFidex(std::vector<std::string> &lines, const std::string &fidexCommand, const std::string &ruleFile) {
  std::cout << "\nWe launch Fidex." << std::endl;
  lines.emplace_back("\nWe launch Fidex.\n");
  lines.emplace_back("Local rule :\n");
  fidex(fidexCommand);
  // Get rule
  fstream ruleF;
  ruleF.open(ruleFile, ios::in); // Read data file
  if (ruleF.fail()) {
    throw FileNotFoundError("Error : file " + std::string(ruleFile) + " not found.");
  }
  string line;
  getline(ruleF, line);
  getline(ruleF, line);
  while (!ruleF.eof()) {
    getline(ruleF, line);
    lines.emplace_back(line);
  }
  ruleF.close();
}

void outputDataSample(const std::string &outFile, const std::vector<double> &data, const std::vector<double> &prediction, int classe, int nbClass) {
  ofstream outputFile(outFile);
  if (outputFile.is_open()) {
    for (const auto &d : data) {
      outputFile << d << " ";
    }
    outputFile << std::endl;
    for (const auto &p : prediction) {
      outputFile << p << " ";
    }
    outputFile << std::endl;
    for (int i = 0; i < nbClass; i++) {
      outputFile << (i == classe ? "1 " : "0 ");
    }
    outputFile.close();
  } else {
    throw CannotOpenFileError("Error : Couldn't open dataSample file " + std::string(outFile) + ".");
  }
}

int fidexGlo(const string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
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
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    std::size_t nbParam = commandList.size();

    // Parameters declaration

    string testSamplesDataFileTemp;
    bool testSamplesDataFileInit = false;
    string testSamplesPredFileTemp; // Test predictions
    bool testSamplesPredFileInit = false;
    string rulesFileTemp;
    bool rulesFileInit = false;
    string explanationFileTemp;
    bool explanationFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;

    int nb_attributes = -1;
    int nb_classes = -1;

    bool hasDecisionThreshold = false;
    double decisionThreshold = -1;
    bool hasIndexPositiveClass = false;
    int indexPositiveClass = -1;

    // Fidex parameters
    bool withFidex = false;
    bool minimalVersion = false;

    string trainDataFile; // Train data
    bool trainDataFileInit = false;
    string trainDataFilePred; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    string trainDataFileTrueClass; // Train true classes
    bool trainDataFileTrueClassInit = false;
    string testTrueClassFileTemp; // Test true classes
    bool testTrueClassFileInit = false;

    std::string weightsFile;
    bool weightsFileInit = false;
    string inputRulesFile; // Rule file to be converted to hyperlocus, replace weight file
    bool inputRulesFileInit = false;

    int seed;
    bool seedInit = false;
    int itMax; // We stop if we have more than itMax iterations
    bool itMaxInit = false;
    int minNbCover; // Minimum size of covering that we ask
    bool minNbCoverInit = false;
    bool minCoverStrategy = false; // Decresase by 1 the minNbCover each time maximal fidelity is not achieved
    bool minCoverStrategyInit = false;
    int maxFailedAttempts = 30; // Maxamum number of attemps when minNbCover = 1
    bool maxFailedAttemptsInit = false;
    double minFidelity = 1; // Minimum Fidelity to obtain when computing a rule
    bool minFidelityInit = false;
    bool dropoutHyp = false; // We dropout a bunch of hyperplans each iteration (could accelerate the processus)
    double dropoutHypParam;
    bool dropoutDim = false; // We dropout a bunch of dimensions each iteration (could accelerate the processus)
    double dropoutDimParam;
    int nbDimlpNets;
    bool nbDimlpNetsInit = false;
    int nbQuantLevels;
    bool nbQuantLevelsInit = false;

    // Import parameters

    if (nbParam <= 1) {
      showParams();
      return 0;
    }

    int p = 1; // We skip "fidexGlo"
    while (p < nbParam) {
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        char option = commandList[p - 1][1];
        const char *arg = &(commandList[p])[0];
        const char *lastArg = &(commandList[p - 1])[0];
        string stringArg = arg;

        switch (option) { // Get letter after the -
        case 'S':
          testSamplesDataFileTemp = arg;
          testSamplesDataFileInit = true;
          break;

        case 'p':
          testSamplesPredFileTemp = arg;
          testSamplesPredFileInit = true;
          break;

        case 'R':
          rulesFileTemp = arg;
          rulesFileInit = true;
          break;

        case 'a':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            nb_attributes = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested.");
          }
          break;

        case 'b':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            nb_classes = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested.");
          }
          break;

        case 'O':
          explanationFileTemp = arg;
          explanationFileInit = true;
          break;

        case 'r':
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case 'A':
          attributFileTemp = arg;
          attributFileInit = true;
          break;

        case 'F':
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case 't':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            hasDecisionThreshold = true;
            decisionThreshold = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested.");
          }
          break;

        case 'x':
          if (CheckPositiveInt(arg)) {
            hasIndexPositiveClass = true;
            indexPositiveClass = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested.");
          }
          break;

        case 'w':
          if (checkBool(arg)) {
            std::transform(stringArg.begin(), stringArg.end(), stringArg.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            withFidex = (stringArg == "true" || stringArg == "1") ? true : false;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", boolean requested.");
          }
          break;

        case 'M':
          if (checkBool(arg)) {
            std::transform(stringArg.begin(), stringArg.end(), stringArg.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            minimalVersion = (stringArg == "true" || stringArg == "1") ? true : false;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", boolean requested.");
          }
          break;

        case 'T':
          trainDataFile = arg; // Parameter after -T
          trainDataFileInit = true;
          break;

        case 'P':
          trainDataFilePred = arg;
          trainDataFilePredInit = true;
          break;

        case 'C':
          trainDataFileTrueClass = arg;
          trainDataFileTrueClassInit = true;
          break;

        case 'c':
          testTrueClassFileTemp = arg;
          testTrueClassFileInit = true;
          break;

        case 'W': {
          weightsFile = arg;
          weightsFileInit = true;
        } break;

        case 'f': {
          inputRulesFile = arg;
          inputRulesFileInit = true;
        } break;

        case 'N':
          if (CheckPositiveInt(arg)) {
            nbDimlpNets = atoi(arg);
            nbDimlpNetsInit = true;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", positive integer requested.");
          }

          break;

        case 'Q':
          if (CheckPositiveInt(arg)) {
            nbQuantLevelsInit = true;
            nbQuantLevels = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", positive integer requested.");
          }
          break;

        case 'i':
          if (CheckPositiveInt(arg)) {
            itMaxInit = true;
            itMax = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested.");
          }
          break;

        case 'v':
          if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
            minNbCoverInit = true;
            minNbCover = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", integer strictly greater than 1 requested.");
          }
          break;

        case 'y':
          if (std::strcmp(arg, "true") == 0 || std::strcmp(arg, "True") == 0 || std::strcmp(arg, "1") == 0) {
            minCoverStrategy = true;
          } else if (std::strcmp(arg, "false") == 0 || std::strcmp(arg, "False") == 0 || std::strcmp(arg, "0") == 0) {
            minCoverStrategy = false;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", boolean requested.");
          }
          minCoverStrategyInit = true;
          break;

        case 'm':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            maxFailedAttempts = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested.");
          }
          maxFailedAttemptsInit = true;
          break;

        case 'q':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            minFidelity = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested.");
          }
          minFidelityInit = true;
          break;

        case 'd':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutDimParam = atof(arg);
            dropoutDim = true; // We dropout a bunch of dimensions each iteration (accelerate the processus)
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested.");
          }
          break;

        case 'h':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            dropoutHypParam = atof(arg);
            dropoutHyp = true; // We dropout a bunch of hyperplans each iteration (accelerate the processus)
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested.");
          }
          break;

        case 'z':
          if (CheckPositiveInt(arg)) {
            seed = atoi(arg);
            seedInit = true;
          } else
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested.");

          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option: " + std::string(&option, 1) + ".");
        }
      }

      p++;
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *testSamplesDataFile = nullptr;
    const char *testSamplesPredFile = nullptr;
    const char *rulesFile = nullptr;
    const char *explanationFile = nullptr;
    const char *consoleFile = nullptr;
    const char *attributFile = nullptr;
    const char *testTrueClassFile = nullptr;

    string root = "";
    if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
      root = rootFolderTemp + "/";
#elif defined(_WIN32)
      root = rootFolderTemp + "\\";
#endif
    }

    if (testSamplesDataFileInit) {
      testSamplesDataFileTemp = root + testSamplesDataFileTemp;
      testSamplesDataFile = &testSamplesDataFileTemp[0];
    }

    if (testSamplesPredFileInit) {
      testSamplesPredFileTemp = root + testSamplesPredFileTemp;
      testSamplesPredFile = &testSamplesPredFileTemp[0];
    }

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      rulesFile = &rulesFileTemp[0];
    }

    if (explanationFileInit) {
      explanationFileTemp = root + explanationFileTemp;
      explanationFile = &explanationFileTemp[0];
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }
    string attributFileTmp;
    if (attributFileInit) {
      attributFileTmp = root + attributFileTemp;
      attributFile = &attributFileTmp[0];
    }
    if (withFidex && testTrueClassFileInit) {
      testTrueClassFileTemp = root + testTrueClassFileTemp;
      testTrueClassFile = &testTrueClassFileTemp[0];
    }

    if (hasDecisionThreshold && !hasIndexPositiveClass) {
      throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t).");
    }

    // ----------------------------------------------------------------------
    if (!testSamplesDataFileInit) {
      throw CommandArgumentException("The test samples data file has to be given with option -S.");
    }
    if (!rulesFileInit) {
      throw CommandArgumentException("The rules file has to be given with option -R.");
    }
    if (nb_attributes == -1) {
      throw CommandArgumentException("The number of attributes has to be given with option -a.");
    }
    if (nb_classes == -1) {
      throw CommandArgumentException("The number of classes has to be given with option -b.");
    }
    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    std::cout << "Import datas..." << endl
              << endl;

    // Get test data
    vector<int> testSamplesPreds;
    vector<vector<double>> testSamplesValues;
    vector<vector<double>> testSamplesOutputValuesPredictions;

    std::unique_ptr<DataSetFid> testDatas;
    if (!testSamplesPredFileInit) { // If we have only one test data file with data and prediction
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, nb_attributes, nb_classes, decisionThreshold, indexPositiveClass));
      testSamplesValues = (*testDatas->getDatas());
      testSamplesPreds = (*testDatas->getPredictions());
      testSamplesOutputValuesPredictions = (*testDatas->getOutputValuesPredictions());

    } else { // We have a different file for test predictions
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, testSamplesPredFile, nb_attributes, nb_classes, decisionThreshold, indexPositiveClass));
      testSamplesValues = (*testDatas->getDatas());
      testSamplesPreds = (*testDatas->getPredictions());
      testSamplesOutputValuesPredictions = (*testDatas->getOutputValuesPredictions());
    }

    int nbSamples = testDatas->getNbSamples();

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (attributFileInit) {
      testDatas->setAttributes(attributFile, nb_attributes, nb_classes);
      attributeNames = (*testDatas->getAttributeNames());
      hasClassNames = testDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = (*testDatas->getClassNames());
      }
    }

    // If we use Fidex, we generate fidex command
    std::string fidexCommand = "";
    std::vector<int> testSamplesClasses;
    std::string fidexRuleFile = "fidexRule.txt";
    if (withFidex) {
      // Check if we have the obligatory parameters
      if (!trainDataFileInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). The train data file has to be given with option -T.");
      }
      if (!trainDataFilePredInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). The train prediction file has to be given with option -P.");
      }
      if (!trainDataFileTrueClassInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). The train true classes file has to be given with option -C.");
      }
      if (!testTrueClassFileInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). The test true classes file has to be given with option -c.");
      }
      if (!weightsFileInit && !inputRulesFileInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f.");
      } else if (weightsFileInit && inputRulesFileInit) {
        throw CommandArgumentException("Error : You specified that you wanted to use Fidex (-w). Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
      }

      // Get test class data :
      (testDatas->setClassFromFile(testTrueClassFile, nb_classes));
      testSamplesClasses = (*testDatas->getClasses());

      fidexCommand += "fidex -T " + trainDataFile + " -P " + trainDataFilePred + " -C " + trainDataFileTrueClass;
      fidexCommand += " -a " + std::to_string(nb_attributes) + " -b " + std::to_string(nb_classes);
      if (inputRulesFileInit) {
        fidexCommand += " -f " + inputRulesFile;
      } else {
        fidexCommand += " -W " + weightsFile;
      }
      if (nbQuantLevelsInit) {
        fidexCommand += " -Q " + std::to_string(nbQuantLevels);
      }
      if (itMaxInit) {
        fidexCommand += " -i " + std::to_string(itMax);
      }
      if (minNbCoverInit) {
        fidexCommand += " -v " + std::to_string(minNbCover);
      }
      if (minCoverStrategyInit) {
        fidexCommand += " -y " + std::to_string(minCoverStrategy);
      }
      if (maxFailedAttemptsInit) {
        fidexCommand += " -m " + std::to_string(maxFailedAttempts);
      }
      if (minFidelityInit) {
        fidexCommand += " -M " + std::to_string(minFidelity);
      }
      if (seedInit) {
        fidexCommand += " -z " + std::to_string(seed);
      }
      if (attributFileInit) {
        fidexCommand += " -A " + attributFileTemp;
      }
      if (dropoutDim) {
        fidexCommand += " -d " + std::to_string(dropoutDimParam);
      }
      if (dropoutHyp) {
        fidexCommand += " -h " + std::to_string(dropoutHypParam);
      }
      if (hasDecisionThreshold) {
        fidexCommand += " -t " + std::to_string(decisionThreshold);
      }
      if (hasIndexPositiveClass) {
        fidexCommand += " -x " + std::to_string(indexPositiveClass);
      }
      if (nbDimlpNetsInit) {
        fidexCommand += " -N " + std::to_string(nbDimlpNets);
      }
      fidexCommand += " -O " + fidexRuleFile + " -R " + rootFolderTemp;
    }

    // Get rules
    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;
    lines.emplace_back("Global statistics of the rule set : ");
    vector<string> stringRules;

    getRules(rules, lines, stringRules, rulesFile, attributFileInit, attributeNames, hasClassNames, classNames);

    std::cout << "Files imported" << endl
              << endl;
    if (nbSamples > 1) {
      std::cout << "Find explanation for each sample..." << endl
                << endl;
    }

    if (hasDecisionThreshold) {
      std::string classDecision;
      if (attributFileInit) {
        classDecision = classNames[indexPositiveClass];
      } else {
        classDecision = std::to_string(indexPositiveClass);
      }
      lines.emplace_back("Using a decision threshold of " + formattingDoubleToString(decisionThreshold) + " for class " + classDecision + "\n");
    }

    lines.emplace_back("\n--------------------------------------------------------------------\n");
    std::cout << "\n--------------------------------------------------------------------" << std::endl;

    // we search explanation for each sample

    std::string testSampleDataFile = "testSampleData.txt";
    if (nbSamples == 1) {
      lines.emplace_back("Explanation for the sample :\n");
      std::cout << "Explanation for the sample :" << std::endl
                << std::endl;
    }
    int nb_fidex = 0; // Number of times Fidex is used
    bool launchingFidex;
    for (int currentSample = 0; currentSample < nbSamples; currentSample++) {
      launchingFidex = false;
      if (nbSamples > 1) {
        lines.push_back("Explanation for sample " + std::to_string(currentSample) + " :\n");
        std::cout << "Explanation for sample " << std::to_string(currentSample) << " :" << std::endl
                  << std::endl;
      }
      int currentPredId = testSamplesPreds[currentSample];
      std::string currentPred;
      if (attributFileInit) {
        currentPred = classNames[currentPredId];
      } else {
        currentPred = std::to_string(currentPredId);
      }
      lines.emplace_back("The model predict class " + currentPred + " with probability " + std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) + "\n");
      std::cout << "The model predict class " << currentPred << " with probability " << std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) << std::endl
                << std::endl;
      // Find rules activated by this sample
      vector<int> activatedRules;
      getActivatedRules(activatedRules, &rules, &testSamplesValues[currentSample]);
      // Check which rules are correct
      vector<int> correctRules;
      vector<int> notcorrectRules;
      bool notShowUncorrectRules = false;
      if (activatedRules.empty()) { // If there is no activated rule
        std::cout << "There is no rule activated" << std::endl;
        std::cout << "We couldn't find any global explanation for this sample." << std::endl; // There is no explanation, we choose the model decision
        std::cout << "We choose the model prediction." << std::endl;
        std::cout << "The predicted class is " << std::to_string(testSamplesPreds[currentSample]) << std::endl;
        lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
        lines.emplace_back("We choose the model prediction.");
        lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
        if (withFidex) {
          outputDataSample(root + testSampleDataFile, testSamplesValues[currentSample], testSamplesOutputValuesPredictions[currentSample], testSamplesClasses[currentSample], nb_classes);
          fidexCommand += " -S " + testSampleDataFile;
          launchingFidex = true;
          nb_fidex += 1;
        }

      } else { // There is some activated rules
        for (int v : activatedRules) {
          if (get<2>(rules[v]) == testSamplesPreds[currentSample]) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          } else {
            notcorrectRules.push_back(v);
          }
        }
        if (correctRules.empty()) { // If there is no correct rule
          int ancientClass = get<2>(rules[activatedRules[0]]);
          bool allSameClass = true; // Check if all the rules choose the same class
          for (int v : activatedRules) {
            if (get<2>(rules[v]) != ancientClass) {
              allSameClass = false;
              break;
            }
          }
          if (allSameClass && !minimalVersion) {
            notShowUncorrectRules = true;
            if (activatedRules.size() > 1) {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found " << std::to_string(activatedRules.size()) << " rules with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            } else {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found 1 rule with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.emplace_back("R" + std::to_string(v + 1) + ": " + stringRules[activatedRules[v]]);
              std::cout << "R" << std::to_string(v + 1) << ": " << stringRules[activatedRules[v]] << std::endl;
            }
          } else {
            if (minimalVersion) {
              std::cout << "There is no correct activated rule for this sample." << std::endl;
            } else {
              std::cout << "There is no correct rule for this sample." << std::endl;
            }

            std::cout << "We couldn't find any global explanation for this sample." << std::endl; // There is no explanation, we choose the model decision
            std::cout << "We choose the model prediction." << std::endl;
            std::cout << "The predicted class is " << std::to_string(testSamplesPreds[currentSample]) << std::endl;
            lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
            lines.emplace_back("We choose the model prediction.");
            lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
            if (withFidex) {
              outputDataSample(testSampleDataFile, testSamplesValues[currentSample], testSamplesOutputValuesPredictions[currentSample], testSamplesClasses[currentSample], nb_classes);
              fidexCommand += " -S " + testSampleDataFile;
              launchingFidex = true;
              nb_fidex += 1;
            }
          }

        } else { // There is an explanation which is caracterised by the correct rules
          if (correctRules.size() > 1) {
            lines.emplace_back("We have found " + std::to_string(correctRules.size()) + " global rules explaining the model prediction :\n"); // There is no explanation, we choose the model decision
            std::cout << "We have found " << std::to_string(correctRules.size()) << " global rules explaining the model prediction :" << std::endl
                      << std::endl; // There is no explanation, we choose the model decision
          } else {
            lines.emplace_back("We have found 1 global rule explaining the model prediction :\n"); // There is no explanation, we choose the model decision
            std::cout << "We have found 1 global rule explaining the model prediction :" << std::endl
                      << std::endl; // There is no explanation, we choose the model decision
          }
          for (int c = 0; c < correctRules.size(); c++) {
            lines.emplace_back("R" + std::to_string(c + 1) + ": " + stringRules[correctRules[c]]);
            std::cout << "R" << std::to_string(c + 1) << ": " << stringRules[correctRules[c]] << std::endl;
          }
        }
      }
      if (minimalVersion) {
        notShowUncorrectRules = true;
      }
      if (!notShowUncorrectRules) {
        if (!notcorrectRules.empty()) {
          lines.emplace_back("\nActivated rules without correct decision class :");
          std::cout << "\nActivated rules without correct decision class :" << std::endl;
          for (int n = 0; n < notcorrectRules.size(); n++) {
            lines.emplace_back("F" + std::to_string(n + 1) + ": " + stringRules[notcorrectRules[n]]);
            std::cout << "F" << std::to_string(n + 1) + ": " << stringRules[notcorrectRules[n]] << std::endl;
          }
        } else {
          lines.emplace_back("\nThere is no uncorrect rules.");
          std::cout << "\nThere is no uncorrect rules." << std::endl;
        }
      }

      if (launchingFidex) {
        launchFidex(lines, fidexCommand, root + fidexRuleFile);
      }

      lines.emplace_back("\n--------------------------------------------------------------------\n");
      std::cout << "\n--------------------------------------------------------------------" << std::endl;
    }

    if (withFidex) {
      double fidex_mean;
      if (nb_fidex == 0) {
        fidex_mean = 0;
      } else {
        fidex_mean = (static_cast<double>(nb_fidex) / static_cast<double>(nbSamples)) * 100;
      }

      lines.emplace_back("\nFidex is used for " + std::to_string(fidex_mean) + "% of samples.");
      std::cout << "\nFidex is used for " + std::to_string(fidex_mean) + "% of samples." << std::endl;

      // Remove temporary files
      std::remove((root + testSampleDataFile).c_str());
      std::remove((root + fidexRuleFile).c_str());
    }

    // Output global explanation result
    if (explanationFileInit) {
      ofstream outputFile(explanationFile);
      if (outputFile.is_open()) {
        for (const auto &line : lines) {
          outputFile << line << "" << std::endl;
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + std::string(explanationFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    cerr << msg << endl;
    return -1;
  }

  return 0;
}

// Exemple pour lancer le code : ./fidexGlo -S datanormTest -p predTest.out -R globalRulesDatanorm.txt -a 16 -b 2 -O explanation.txt -F ../fidexGlo/datafiles -w true -T datanormTrain -P predTrain.out -C dataclass2Train -c dataclass2Test -W weights.wts
