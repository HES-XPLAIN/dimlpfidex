#include "fidexGloRulesFct.h"

using namespace std;

void showRulesParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidexGloRules -T <train dataset file> -P <train prediction file> -C <train true class file> ";
  std::cout << "-W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f] ";
  std::cout << "-f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  std::cout << "-O <Rules output file> ";
  std::cout << "-M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  std::cout << "-N <number of networks for bagging, 1 means no bagging, necessary to use bagging>";
  std::cout << "-A <file of attributes>\n";
  std::cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  std::cout << "-i <max iteration number>\n";
  std::cout << "-v <minimum covering number>\n";
  std::cout << "-d <dimension dropout parameter>\n";
  std::cout << "-h <hyperplan dropout parameter>\n";
  std::cout << "-Q <number of stairs in staircase activation function (50 by default)>\n";
  std::cout << "-I <high side of the interval (5 by default)>\n";
  std::cout << "-z <seed (0=ranodom)>";

  std::cout << "\n-------------------------------------------------\n\n";
}

int fidexGloRules(const string &command) {
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
    size_t nbParam = commandList.size();

    // Parameters declaration

    int seed = 0;

    string trainDataFileTemp; // Train data
    bool trainDataFileInit = false;
    string trainDataFilePredTemp; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    string trainDataFileTrueClassTemp; // Train true classes
    bool trainDataFileTrueClassInit = false;

    std::string weightsFileTemp;
    std::vector<std::string> weightsFilesTemp;
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
            return -1;

          break;

        case 'Q':
          if (CheckPositiveInt(arg)) {
            nbQuantLevels = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", positive integer requested");
          }
          break;

        case 'I':
          if (CheckFloatFid(arg) && atof(arg) > 0) {
            hiKnot = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", strictly positive float requested");
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
          weightsFilesTemp.push_back(weightsFileTemp + std::to_string(n + 1) + ".wts");
        }
      }
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *trainDataFile = nullptr;
    const char *trainDataFilePred = nullptr;
    const char *trainDataFileTrueClass = nullptr;
    const char *rulesFile = nullptr;
    std::vector<const char *> weightsFiles;
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
    std::ofstream ofs;
    std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    std::cout << "\nParameters :\n\n";
    std::cout << "- Max iteration number : " << itMax << endl;
    std::cout << "- Min size of covering : " << minNbCover << endl;
    if (dropoutDim) {
      std::cout << "- We use a dimension dropout of " << dropoutDimParam << endl
                << endl;
    } else {
      std::cout << "- We don't use dimension dropout\n\n";
    }
    if (dropoutHyp) {
      std::cout << "- We use a hyperplan dropout of " << dropoutHypParam << endl
                << endl;
    } else {
      std::cout << "- We don't use hyperplan dropout\n\n";
    }

    // Import files

    std::cout << "Import files..." << endl;

    std::unique_ptr<DataSetFid> trainDatas(new DataSetFid(trainDataFile, trainDataFilePred, trainDataFileTrueClass));

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
      std::unique_ptr<Attribute> attributesData(new Attribute(attributFile));
      attributeNames = (*attributesData->getAttributes());
      if (attributeNames.size() < nbAttributs) {
        throw FileContentError("Error : in file " + std::string(attributFile) + ", there is not enough attribute names");
      } else if (attributeNames.size() == nbAttributs) {
        hasClassNames = false;
      } else if (attributeNames.size() != nbAttributs + nbClass) {
        throw FileContentError("Error : in file " + std::string(attributFile) + ", there is not the good amount of attribute and class names");
      } else {
        hasClassNames = true;
        auto firstEl = attributeNames.end() - nbClass;
        auto lastEl = attributeNames.end();
        classNames.insert(classNames.end(), firstEl, lastEl);
        attributeNames.erase(firstEl, lastEl);
      }
    }

    std::cout << "Files imported" << endl
              << endl;

    // compute hyperspace

    std::cout << "Creation of hyperspace..." << endl;

    std::vector<std::vector<double>> matHypLocus;

    if (weightsFileInit) {
      for (const auto &weightsFile : weightsFiles) {
        std::vector<std::vector<double>> hypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end());                      // Concatenate hypLocus to matHypLocus
      }
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, nbAttributs);
    }

    FidexGloNameSpace::Hyperspace hyperspace(matHypLocus); // Initialize hyperbox and get hyperplans

    const auto nbIn = static_cast<int>(hyperspace.getHyperLocus().size()); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributs));
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    // Samples not yet covered by any rules
    vector<int> notCoveredSamples(nbDatas);
    std::iota(std::begin(notCoveredSamples), std::end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

    vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> chosenRules; // antecedents, cover vector, class, rule accuracy, rule confidence

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Initialize random number generator

    if (seed == 0) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      auto seedValue = currentTime.time_since_epoch().count();
      seed = static_cast<unsigned int>(seedValue);
    }
    std::mt19937 gen(seed);

    // First heuristic : optimal (slower)
    float temps1;
    clock_t c1;
    clock_t c2;

    c1 = clock();
    if (heuristic == 1) {
      std::cout << "Optimal FidexGlo" << endl
                << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      auto exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl
                << endl;

      for (int idSample = 0; idSample < nbDatas; idSample++) {

        if (int(nbDatas / 100) != 0 && idSample % int(nbDatas / 100) == 0 && consoleFileInit == false) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%\r";
          std::cout.flush();
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
            auto it = std::find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
            if (it != notCoveredSamples.end()) {
              notCoveredSamples.erase(it);
            }
            break;
          }
        }
        if (currentMinNbCov + 1 < minNbCover) {
          nbProblems += 1;
        }

        rules.push_back(rule);
      }

      std::cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
      if (nbRulesNotFound > 0) {
        std::cout << "Number of rules not found : " << nbRulesNotFound << endl;
      }

      std::cout << "Fidex rules computed" << endl
                << endl;

      std::cout << "Computing global ruleset..." << endl
                << endl;

      nbRules = 0;

      // While there is some not covered samples
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> currentRule;
      std::vector<int>::iterator ite;
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
          ite = std::set_difference(newNotCoveredSamples.begin(), newNotCoveredSamples.end(), get<1>(rules[r]).begin(), get<1>(rules[r]).end(), newNotCoveredSamples.begin()); // vectors have to be sorted
          newNotCoveredSamples.resize(ite - newNotCoveredSamples.begin());
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
      }

      std::cout << "Global ruleset Computed" << endl
                << endl;

      std::cout << "We created " << nbRules << " rules." << endl
                << endl;

      c2 = clock();
      temps1 = (float)(c2 - c1) / CLOCKS_PER_SEC;
      std::cout << "\nTime first heuristic = " << temps1 << " sec\n\n";
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Second heuristic : fast
    float temps2;
    clock_t d1;
    clock_t d2;

    d1 = clock();
    if (heuristic == 2) {
      std::cout << "Fast FidexGlo" << endl
                << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      auto exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl
                << endl;
      for (int idSample = 0; idSample < nbDatas; idSample++) {

        if (int(nbDatas / 100) != 0 && idSample % int(nbDatas / 100) == 0 && consoleFileInit == false) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%\r";
          std::cout.flush();
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
            auto it = std::find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
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

      std::cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
      if (nbRulesNotFound > 0) {
        std::cout << "Number of rules not found : " << nbRulesNotFound << endl;
      }

      std::cout << "Fidex rules computed" << endl
                << endl;

      std::cout << "Computing global ruleset..." << endl
                << endl;

      // Sort the rules(dataIds) depending of their covering size
      vector<int> dataIds = notCoveredSamples;
      std::sort(dataIds.begin(), dataIds.end(), [&rules](int ruleBest, int ruleWorst) {
        return std::get<1>(rules[ruleWorst]).size() < std::get<1>(rules[ruleBest]).size();
      });

      nbRules = 0;

      // While there is some not covered samples
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> currentRule;
      auto ancienNotCoveringSize = static_cast<int>(notCoveredSamples.size());
      int dataId = 0;

      std::vector<int>::iterator ite;
      while (!notCoveredSamples.empty()) {
        currentRule = rules[dataIds[dataId]];

        // Delete covered samples
        ite = std::set_difference(notCoveredSamples.begin(), notCoveredSamples.end(), get<1>(currentRule).begin(), get<1>(currentRule).end(), notCoveredSamples.begin()); // vectors have to be sorted
        notCoveredSamples.resize(ite - notCoveredSamples.begin());

        // If the rule coveres a new sample
        if (ancienNotCoveringSize > notCoveredSamples.size()) {
          nbRules += 1;
          chosenRules.push_back(currentRule); // add best rule with maximum covering
        }
        ancienNotCoveringSize = static_cast<int>(notCoveredSamples.size());

        dataId += 1;
      }

      std::cout << "Global ruleset Computed" << endl
                << endl;

      std::cout << "We created " << nbRules << " rules." << endl
                << endl;

      d2 = clock();
      temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;
      std::cout << "\nTime second heuristic = " << temps2 << " sec\n\n";
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Third heuristic : Very fast
    float temps3;
    clock_t e1;
    clock_t e2;

    e1 = clock();

    if (heuristic == 3) {
      std::cout << "Very fast FidexGlo" << endl
                << endl;

      //  Sort data randomly
      std::shuffle(std::begin(notCoveredSamples), std::end(notCoveredSamples), gen);

      nbRules = 0;
      int idSample;
      bool ruleCreated;
      int currentMinNbCov;
      int nbProblems = 0;
      int nbRulesNotFound = 0;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      auto exp = FidexAlgo();

      std::cout << "Computing rules..." << endl
                << endl;

      // While there is some not covered samples
      while (!notCoveredSamples.empty()) {

        if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && consoleFileInit == false) {
          cout << "Processing : " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%r";
          std::cout.flush();
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
              std::remove_if(
                  notCoveredSamples.begin(), notCoveredSamples.end(), [&rule](int x) {
                    return std::find(get<1>(rule).begin(), get<1>(rule).end(), x) != get<1>(rule).end();
                    // find index of coveredSamples which is x (x is element of notCoveredSamples), std::find returns last if x not found
                    // -> Removes x if it appears on coveredSamples (found before the end of coveredSamples)
                  }),
              notCoveredSamples.end());

          nbRules++;
        } else {
          notCoveredSamples.erase(notCoveredSamples.begin());
        }
      }

      std::cout << endl;

      std::cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;
      if (nbRulesNotFound > 0) {
        std::cout << "Number of rules not found : " << nbRulesNotFound << endl;
      }

      std::cout << "We created " << nbRules << " rules." << endl
                << endl;

      e2 = clock();
      temps3 = (float)(e2 - e1) / CLOCKS_PER_SEC;
      std::cout << "\nTime third heuristic = " << temps3 << " sec\n\n";
    }

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    std::cout << "Rules extraction..." << endl
              << endl;

    double meanCovSize = 0;
    double meanNbAntecedents = 0;

    // Export rules
    string inequality;
    string line;
    vector<string> lines;
    // Sort the rules(RulesIds) depending of their covering size
    vector<int> RulesIds(nbRules);                          // Rules indexes
    std::iota(std::begin(RulesIds), std::end(RulesIds), 0); // Vector from 0 to nbRules-1
    std::sort(RulesIds.begin(), RulesIds.end(), [&chosenRules](int ruleBest, int ruleWorst) {
      return std::get<1>(chosenRules[ruleWorst]).size() < std::get<1>(chosenRules[ruleBest]).size();
    });
    for (int c = 0; c < nbRules; c++) { // each rule
      int r = RulesIds[c];
      meanCovSize += static_cast<double>(get<1>(chosenRules[r]).size());
      meanNbAntecedents += static_cast<double>(get<0>(chosenRules[r]).size());
      line = "Rule " + std::to_string(c + 1) + ": ";
      line += std::to_string(get<0>(chosenRules[r]).size()) + " : "; // If we need to specify the antecedant number
      for (const auto &rule : get<0>(chosenRules[r])) {              // each antecedant
        if (get<1>(rule) == 1) {                                     // check inequality
          inequality = ">=";
        } else {
          inequality = "<";
        }
        if (attributFileInit) {
          line += attributeNames[get<0>(rule)];
        } else {
          line += "X" + std::to_string(get<0>(rule));
        }
        line += inequality + std::to_string(get<2>(rule)) + " ";
      }
      // class of the rule
      if (hasClassNames) {
        line += "-> " + classNames[std::get<2>(chosenRules[r])];
      } else {
        line += "-> class " + std::to_string(std::get<2>(chosenRules[r]));
      }
      line += " Covering size : " + std::to_string(std::get<1>(chosenRules[r]).size()); // Covering size
      line += " Fidelity : 1";                                                          // Rule fidelity
      line += " Accuracy : " + std::to_string(std::get<3>(chosenRules[r]));             // Rule accuracy
      if (hasConfidence) {
        line += " Confidence : " + std::to_string(std::get<4>(chosenRules[r])) + "\n"; // Rule confidence
      } else {
        line += "\n";
      }
      lines.push_back(line);
    }

    meanCovSize /= nbRules;
    meanNbAntecedents /= nbRules;

    ofstream file2(rulesFile);
    if (file2.is_open()) {
      // We add to the file the number of rules, the mean covering per rule and the mean number of antecedents per rule
      string startLine = "Number of rules : " + std::to_string(nbRules) + ", mean sample covering number per rule : " + std::to_string(meanCovSize) + ", mean number of antecedents per rule : " + std::to_string(meanNbAntecedents) + "\n";
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
      throw CannotOpenFileError("Error : Couldn't open rules extraction file " + std::string(rulesFile) + ".");
    }

    std::cout << "Mean covering size per rule : " << meanCovSize << endl;
    std::cout << "Mean number of antecedents per rule : " << meanNbAntecedents << endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec\n";

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    cerr << msg << endl;
  }

  return 0;
}

/* Exemples pour lancer le code :

./fidexGloRules -T datanormTrain -P dimlpDatanormTrain.out -C dataclass2Train -W dimlpDatanorm.wts -Q 50 -I 5 -O globalRulesDatanorm.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesResult -S ../fidexGlo/datafiles

./fidexGloRules -T covidTrainData.txt -P covidTrainPred.out -C covidTrainClass.txt -W covid.wts -Q 50 -I 5 -O globalRulesCovid.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesCovidResult -S ../dimlp/datafiles/covidDataset
./fidexGloRules -T spamTrainData.txt -P spamTrainPred.out -C spamTrainClass.txt -W spam.wts -Q 50 -I 5 -O globalRulesSpam.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesSpamResult -S ../dimlp/datafiles/spamDataset
./fidexGloRules -T isoletTrainData.txt -P isoletTrainPredV2.out -C isoletTrainClass.txt -W isoletV2.wts -Q 50 -I 5 -O globalRulesIsoletV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesIsoletResultV2 -S ../dimlp/datafiles/isoletDataset
./fidexGloRules -T Train/X_train.txt -P Train/pred_trainV2.out -C Train/y_train.txt -W HAPTV2.wts -Q 50 -I 5 -O globalRulesHAPTV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesHAPTResultV2 -S ../dimlp/datafiles/HAPTDataset

*/
