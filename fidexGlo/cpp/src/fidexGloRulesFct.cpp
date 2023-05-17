#include "fidexGloRulesFct.h"

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  cout << "fidexGloRules -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
  std::cout << "-T <train dataset file> -P <train prediction file> -C <train true class file> ";
  std::cout << "-H <hyperLocus file> ";
  std::cout << "-O <Rules output file> ";
  std::cout << "-M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-A <file of attributes>\n";
  std::cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  std::cout << "-i <max iteration number>\n";
  std::cout << "-v <minimum covering number>\n";
  std::cout << "-d <dimension dropout parameter>\n";
  std::cout << "-h <hyperplan dropout parameter>\n";
  std::cout << "-z <seed (0=ranodom)>";

  std::cout << "\n-------------------------------------------------\n\n";
}

int fidexGloRules(string command) {
  try {

    float temps;
    clock_t t1, t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    int nbParam = commandList.size();

    // Import parameters
    if (nbParam == 1) {
      showParams();
      return -1;
    }

    // Parameters declaration

    int seed = 0;

    string trainDataFileTemp; // Train data
    bool trainDataFileInit = false;
    string trainDataFilePredTemp; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    string trainDataFileTrueClassTemp; // Train true classes
    bool trainDataFileTrueClassInit = false;

    string hyperLocusFileTemp;
    bool hyperLocusFileInit = false;
    string rulesFileTemp;
    bool rulesFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;

    int heuristic = -1;
    bool heuristicInit = false;

    int itMax = 100;         // We stop if we have more than itMax iterations
    int minNbCover = 2;      // Minimum size of covering that we ask
    bool dropoutHyp = false; // We dropout a bunch of hyperplans each iteration (could accelerate the processus)
    double dropoutHypParam = 0.5;
    bool dropoutDim = false; // We dropout a bunch of dimensions each iteration (could accelerate the processus)
    double dropoutDimParam = 0.5;
    DataSetFid *trainDatas;
    Attribute *attributesData;

    int nbRules; // Number of rules created

    // Import parameters

    for (int p = 1; p < nbParam; p++) { // We skip "fidexGloRules"
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw std::runtime_error("Missing something at the end of the command.");
        }

        switch (commandList[p - 1][1]) { // Get letter after the -

        case 'T':
          trainDataFileTemp = &(commandList[p])[0]; // Parameter after -T
          trainDataFileInit = true;
          break;

        case 'P':
          trainDataFilePredTemp = &(commandList[p])[0];
          trainDataFilePredInit = true;
          break;

        case 'C':
          trainDataFileTrueClassTemp = &(commandList[p])[0];
          trainDataFileTrueClassInit = true;
          break;

        case 'H':
          hyperLocusFileTemp = &(commandList[p])[0];
          hyperLocusFileInit = true;
          break;

        case 'O':
          rulesFileTemp = &(commandList[p])[0];
          rulesFileInit = true;
          break;

        case 'r':
          consoleFileTemp = &(commandList[p])[0];
          consoleFileInit = true;
          break;

        case 'A':
          attributFileTemp = &(commandList[p])[0];
          attributFileInit = true;
          break;

        case 'S':
          rootFolderTemp = &(commandList[p])[0];
          rootFolderInit = true;
          break;

        case 'M':
          if (CheckPositiveInt(&(commandList[p])[0]) && atoi(&(commandList[p])[0]) >= 1 && atoi(&(commandList[p])[0]) <= 3) {
            heuristic = atoi(&(commandList[p])[0]);
            heuristicInit = true;
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", heuristic must be an integer between 1 and 3(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo)");
          }
          break;

        case 'i':
          if (CheckPositiveInt(&(commandList[p])[0])) {
            itMax = atoi(&(commandList[p])[0]);
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", positive integer requested");
          }
          break;

        case 'v':
          if (CheckPositiveInt(&(commandList[p])[0]) && atoi(&(commandList[p])[0]) >= 1) {
            minNbCover = atoi(&(commandList[p])[0]);
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", strictly positive integer requested");
          }
          break;

        case 'd':
          if (CheckFloatFid(&(commandList[p])[0]) && atof(&(commandList[p])[0]) >= 0 && atof(&(commandList[p])[0]) <= 1) {
            dropoutDimParam = atof(&(commandList[p])[0]);
            dropoutDim = true; // We dropout a bunch of dimensions each iteration (accelerate the processus)
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", float included in [0,1] requested");
          }
          break;

        case 'h':
          if (CheckFloatFid(&(commandList[p])[0]) && atof(&(commandList[p])[0]) >= 0 && atof(&(commandList[p])[0]) <= 1) {
            dropoutHypParam = atof(&(commandList[p])[0]);
            dropoutHyp = true; // We dropout a bunch of hyperplans each iteration (accelerate the processus)
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", float included in [0,1] requested");
          }
          break;

        case 'z':
          if (CheckPositiveInt(&(commandList[p])[0]))
            seed = atoi(&(commandList[p])[0]);
          else
            return -1;

          break;

        default: // If we put another -X option
          throw std::runtime_error("Illegal option : " + string(&(commandList[p - 1])[0]));
        }
      }
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    char trainDataFileTmp[160], trainDataFilePredTmp[160], trainDataFileTrueClassTmp[160], rulesFileTmp[160], hyperLocusFileTmp[160], consoleFileTmp[160], attributFileTmp[160];

    char *trainDataFile = 0;
    char *trainDataFilePred = 0;
    char *trainDataFileTrueClass = 0;
    char *rulesFile = 0;
    char *hyperLocusFile = 0;
    char *consoleFile = 0;
    char *attributFile = 0;

#if defined(__unix__) || defined(__APPLE__)
    string root = rootFolderTemp + "/";
#elif defined(_WIN32)
    string root = rootFolderTemp + "\\";
#endif

    if (trainDataFileInit) {
      trainDataFileTemp = root + trainDataFileTemp;
      if (trainDataFileTemp.length() >= 160) {
        cout << "Path " << trainDataFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(trainDataFileTmp, trainDataFileTemp.c_str());
      trainDataFile = trainDataFileTmp;
    }

    if (trainDataFilePredInit) {
      trainDataFilePredTemp = root + trainDataFilePredTemp;
      if (trainDataFilePredTemp.length() >= 160) {
        cout << "Path " << trainDataFilePredTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(trainDataFilePredTmp, trainDataFilePredTemp.c_str());
      trainDataFilePred = trainDataFilePredTmp;
    }

    if (trainDataFileTrueClassInit) {
      trainDataFileTrueClassTemp = root + trainDataFileTrueClassTemp;
      if (trainDataFileTrueClassTemp.length() >= 160) {
        cout << "Path " << trainDataFileTrueClassTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(trainDataFileTrueClassTmp, trainDataFileTrueClassTemp.c_str());
      trainDataFileTrueClass = trainDataFileTrueClassTmp;
    }

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      if (rulesFileTemp.length() >= 160) {
        cout << "Path " << rulesFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(rulesFileTmp, rulesFileTemp.c_str());
      rulesFile = rulesFileTmp;
    }

    if (hyperLocusFileInit) {
      hyperLocusFileTemp = root + hyperLocusFileTemp;
      if (hyperLocusFileTemp.length() >= 160) {
        cout << "Path " << hyperLocusFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(hyperLocusFileTmp, hyperLocusFileTemp.c_str());
      hyperLocusFile = hyperLocusFileTmp;
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      if (consoleFileTemp.length() >= 160) {
        cout << "Path " << consoleFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(consoleFileTmp, consoleFileTemp.c_str());
      consoleFile = consoleFileTmp;
    }

    if (attributFileInit) {
      attributFileTemp = root + attributFileTemp;
      if (attributFileTemp.length() >= 160) {
        cout << "Path " << attributFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(attributFileTmp, attributFileTemp.c_str());
      attributFile = attributFileTmp;
    }

    // ----------------------------------------------------------------------

    if (!trainDataFileInit) {
      throw std::runtime_error("The train data file has to be given with option -T");
    }
    if (!trainDataFilePredInit) {
      throw std::runtime_error("The train prediction file has to be given with option -P");
    }
    if (!trainDataFileTrueClassInit) {
      throw std::runtime_error("The train true classes file has to be given with option -C");
    }
    if (!hyperLocusFileInit) {
      throw std::runtime_error("The hyperLocus file has to be given with option -H");
    }
    if (!rulesFileInit) {
      throw std::runtime_error("The output rules file has to be given with option -O");
    }
    if (!heuristicInit) {
      throw std::runtime_error("The heuristic(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo) has to be given with option -M");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    std::ofstream ofs;
    std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }
    std::ostream &output = consoleFileInit != false ? ofs : std::cout;

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

    trainDatas = new DataSetFid(trainDataFile, trainDataFilePred, trainDataFileTrueClass);

    vector<vector<double>> *trainData = trainDatas->getDatas();
    vector<int> *trainPreds = trainDatas->getPredictions();
    vector<vector<double>> *trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
    vector<int> *trainTrueClass = trainDatas->getTrueClasses();

    const int nbDatas = (*trainData).size();
    const int nbAttributs = (*trainData)[0].size();
    const int nbClass = (*trainOutputValuesPredictions)[0].size();
    if ((*trainPreds).size() != nbDatas || (*trainTrueClass).size() != nbDatas) {
      throw std::runtime_error("All the train files need to have the same amount of datas");
    }

    if (minNbCover > nbDatas) {
      throw std::runtime_error("Error : invalide type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
    }

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames;
    if (attributFileInit) {
      attributesData = new Attribute(attributFile);
      attributeNames = (*attributesData->getAttributes());
      if (attributeNames.size() < nbAttributs) {
        throw std::runtime_error("Error : in file " + std::string(attributFile) + ", there is not enough attribute names");
      } else if (attributeNames.size() == nbAttributs) {
        hasClassNames = false;
      } else if (attributeNames.size() != nbAttributs + nbClass) {
        throw std::runtime_error("Error : in file " + std::string(attributFile) + ", there is not the good amount of attribute and class names");
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

    FidexGloNameSpace::Hyperspace hyperspace(hyperLocusFile); // Initialize hyperbox and get hyperplans

    const int nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    const int nbHyp = hyperspace.getHyperLocus()[0].size();

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0) {
      throw std::runtime_error("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributs));
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    // Samples not yet covered by any rules
    vector<int> notCoveredSamples(nbDatas);
    std::iota(std::begin(notCoveredSamples), std::end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

    vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> chosenRules; // antecedents, cover vector, class, rule accuracy, rule confidence

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    std::random_device rd;
    std::mt19937 g;
    if (seed != 0) { // Not random
      std::mt19937 g(seed);
    } else { // random
      std::mt19937 g(rd());
    }

    if (seed == 0) {
      std::srand(time(0)); // Initialize random number generator
    } else {
      std::srand(seed);
    }

    // First heuristic : optimal (slower)
    float temps1;
    clock_t c1, c2;

    c1 = clock();
    if (heuristic == 1) {
      std::cout << "Optimal FidexGlo" << endl
                << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl
                << endl;

      for (int idSample = 0; idSample < nbDatas; idSample++) {

        if (idSample % int(nbDatas / 100) == 0) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%" << endl;
        }
        currentMinNbCov = minNbCover;
        ruleCreated = false;

        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam, seed, g);
          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
        }
        if (currentMinNbCov + 1 < minNbCover) {
          nbProblems += 1;
        }

        // cout << get<1>(rule).size() << endl;
        rules.push_back(rule);
      }

      std::cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;

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
      while (notCoveredSamples.size() != 0) {

        // Get rule that covers the most new samples
        int bestRule = -1;
        int bestCovering = INT_MAX;
        int currentCovering; // Size of new covering if we choose this rule
        for (int r = 0; r < rules.size(); r++) {
          newNotCoveredSamples = notCoveredSamples;
          // Remove samples that are in current covering
          ite = std::set_difference(newNotCoveredSamples.begin(), newNotCoveredSamples.end(), get<1>(rules[r]).begin(), get<1>(rules[r]).end(), newNotCoveredSamples.begin()); // vectors have to be sorted
          newNotCoveredSamples.resize(ite - newNotCoveredSamples.begin());
          currentCovering = newNotCoveredSamples.size();

          if (currentCovering < bestCovering) {
            bestRule = r;
            bestCovering = currentCovering;
            bestNewNotCoveredSamples = newNotCoveredSamples;
          }
        }
        currentRule = rules[bestRule];

        notCoveredSamples = bestNewNotCoveredSamples; // Delete new covered samples

        nbRules += 1;
        // std::cout << "Rule number " << nbRules << " ";
        // std::cout << "Size of covering : " << std::get<1>(currentRule).size() << " ";
        chosenRules.push_back(currentRule); // add best rule with maximum covering
        // cout << "nbRules : " << chosenRules.size() << " ";
        // std::cout << "New size : " << notCoveredSamples.size() << " ";

        rules.erase(rules.begin() + bestRule); // Remove this rule

        // cout << "Rules left : " << rules.size() << endl;
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
    clock_t d1, d2;

    d1 = clock();
    if (heuristic == 2) {
      std::cout << "Fast FidexGlo" << endl
                << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl
                << endl;
      for (int idSample = 0; idSample < nbDatas; idSample++) {

        if (idSample % int(nbDatas / 100) == 0) {
          cout << "Processing : " << int((double(idSample) / nbDatas) * 100) << "%" << endl;
        }
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam, seed, g);
          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
        }
        if (currentMinNbCov + 1 < minNbCover) {
          nbProblems += 1;
        }

        rules.push_back(rule);
      }

      std::cout << "\nNumber of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;

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
      int ancienNotCoveringSize = notCoveredSamples.size();
      int dataId = 0;

      std::vector<int>::iterator ite;
      while (notCoveredSamples.size() != 0) {
        currentRule = rules[dataIds[dataId]];

        // Delete covered samples
        ite = std::set_difference(notCoveredSamples.begin(), notCoveredSamples.end(), get<1>(currentRule).begin(), get<1>(currentRule).end(), notCoveredSamples.begin()); // vectors have to be sorted
        notCoveredSamples.resize(ite - notCoveredSamples.begin());

        // If the rule coveres a new sample
        if (ancienNotCoveringSize > notCoveredSamples.size()) {
          nbRules += 1;
          // std::cout << "Rule number " << nbRules << " ";
          // std::cout << "Size of covering : " << std::get<1>(currentRule).size() << " ";
          chosenRules.push_back(currentRule); // add best rule with maximum covering
          // cout << "nbRules : " << chosenRules.size() << " ";
          // std::cout << "New size : " << notCoveredSamples.size() << " ";
        }
        ancienNotCoveringSize = notCoveredSamples.size();

        // cout << "Rules left : " << rules.size()-dataId << endl;

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
    clock_t e1, e2;

    e1 = clock();

    if (heuristic == 3) {
      std::cout << "Very fast FidexGlo" << endl
                << endl;

      //  Sort data randomly
      if (seed == 0) { // random
        unsigned seedShuffle = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(std::begin(notCoveredSamples), std::end(notCoveredSamples), std::default_random_engine(seedShuffle));
      } else { // not random
        std::shuffle(std::begin(notCoveredSamples), std::end(notCoveredSamples), g);
      }

      nbRules = 0;
      int idSample;
      bool ruleCreated;
      int currentMinNbCov;
      int nbProblems = 0;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      std::cout << "Computing rules..." << endl
                << endl;

      // While there is some not covered samples
      while (notCoveredSamples.size() != 0) {

        if ((nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0) {
          cout << "Processing : " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%" << endl;
        }
        // std::cout << "iteration " << nbRules << " ";

        idSample = notCoveredSamples[0];
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        while (!ruleCreated) {
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam, seed, g);
          if (currentMinNbCov >= 2) {
            currentMinNbCov -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
          if (currentMinNbCov + 1 < minNbCover) {
            nbProblems += 1;
          }
        }

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

        // std::cout << "New size : " << notCoveredSamples.size() << endl;

        nbRules++;
      }

      std::cout << endl;

      std::cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;

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
      meanCovSize += get<1>(chosenRules[r]).size();
      meanNbAntecedents += get<0>(chosenRules[r]).size();
      line = "Rule " + std::to_string(c + 1) + ": ";
      line += std::to_string(get<0>(chosenRules[r]).size()) + " : "; // If we need to specify the antecedant number
      for (int a = 0; a < get<0>(chosenRules[r]).size(); a++) {      // each antecedant
        if (get<1>(get<0>(chosenRules[r])[a]) == 1) {                // check inequality
          inequality = ">=";
        } else {
          inequality = "<";
        }
        if (attributFileInit) {
          line += attributeNames[get<0>(get<0>(chosenRules[r])[a])];
        } else {
          line += "X" + std::to_string(get<0>(get<0>(chosenRules[r])[a]));
        }
        line += inequality + std::to_string(get<2>(get<0>(chosenRules[r])[a])) + " ";
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
      line += " Confidence : " + std::to_string(std::get<4>(chosenRules[r])) + "\n";    // Rule confidence
      lines.push_back(line);
    }

    meanCovSize /= nbRules;
    meanNbAntecedents /= nbRules;

    ofstream file2(rulesFile);
    if (file2.is_open()) {
      // We add to the file the number of rules, the mean covering per rule and the mean number of antecedents per rule
      string startLine = "Number of rules : " + std::to_string(nbRules) + ", mean sample covering number per rule : " + std::to_string(meanCovSize) + ", mean number of antecedents per rule : " + std::to_string(meanNbAntecedents) + "\n";
      file2 << startLine;
      for (int l = 0; l < lines.size(); l++) {
        file2 << lines[l];
      }

      file2.close();

    } else {
      throw std::runtime_error("Error : Couldn't open rules extraction file " + std::string(rulesFile) + ".");
    }

    std::cout << "Mean covering size per rule : " << meanCovSize << endl;
    std::cout << "Mean number of antecedents per rule : " << meanNbAntecedents << endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec\n";

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    std::printf(msg);
    cerr << msg << endl;
  }

  return 0;
}

// Exemple :
//  .\fidexGloRules.exe -T datanormTrain -P dimlpDatanormTrain.out -C dataclass2Train -H hyperLocusDatanorm -O globalRulesDatanorm.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesResult -S ../fidexGlo/datafiles/

// .\fidexGloRules.exe -T covidTrainData.txt -P covidTrainPred.out -C covidTrainClass.txt -H hyperLocusCovid -O globalRulesCovid.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesCovidResult -S ../dimlp/datafiles/covidDataset
// .\fidexGloRules.exe -T spamTrainData.txt -P spamTrainPred.out -C spamTrainClass.txt -H hyperLocusSpam -O globalRulesSpam.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesSpamResult -S ../dimlp/datafiles/spamDataset
// .\fidexGloRules.exe -T isoletTrainData.txt -P isoletTrainPredV2.out -C isoletTrainClass.txt -H hyperLocusIsoletV2 -O globalRulesIsoletV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesIsoletResultV2 -S ../dimlp/datafiles/isoletDataset
// .\fidexGloRules.exe -T Train/X_train.txt -P Train/pred_trainV2.out -C Train/y_train.txt -H hyperLocusHAPTV2 -O globalRulesHAPTV2.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5 -r rulesHAPTResultV2 -S ../dimlp/datafiles/HAPTDataset
