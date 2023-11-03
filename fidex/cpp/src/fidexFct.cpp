#include "fidexFct.h"

using namespace std;

void showFidexParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;
  std::cout << "fidex -T <train dataset file> -P <train prediction file> -C <train true class file> ";
  std::cout << "-S <test sample(s) data file with data, prediction(if no -p) and true class(if no -c, with only the index of the true class)> ";
  std::cout << "-W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f] ";
  std::cout << "-f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  std::cout << "-O <Rule output file>";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "-R <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "-N <number of networks for bagging, 1 means no bagging, necessary to use bagging>";
  std::cout << "-p <test prediction file> ";
  std::cout << "-c <test true class file> If at least -p is specified, -S needs to have only test datas" << std::endl;
  std::cout << "-A <file of attributes>" << std::endl;
  std::cout << "-s <output statistic file>" << std::endl;
  std::cout << "-r <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "-i <max iteration number>" << std::endl;
  std::cout << "-v <minimum covering number>" << std::endl;
  std::cout << "-d <dimension dropout parameter>" << std::endl;
  std::cout << "-h <hyperplan dropout parameter>" << std::endl;
  std::cout << "-Q <number of stairs in staircase activation function (50 by default)>" << std::endl;
  std::cout << "-I <high side of the interval (5 by default)>" << std::endl;
  std::cout << "-t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "-x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << std::endl;
  std::cout << "-z <seed (0=ranodom)>";

  std::cout << "\n-------------------------------------------------\n"
            << std::endl;
}

int fidex(const string &command) {

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
    size_t nbParam = commandList.size();

    // Parameters declaration

    int seed = 0;

    string trainDataFileTemp; // Train data
    bool trainDataFileInit = false;
    string trainDataFilePredTemp; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    string trainDataFileTrueClassTemp; // Train true classes
    bool trainDataFileTrueClassInit = false;

    string mainSamplesDataFileTemp; // Test data value, prediction(if no -p) and true class(optional, if no -c)
    bool mainSamplesDataFileInit = false;
    string mainSamplesPredFileTemp; // Test predictions
    bool mainSamplesPredFileInit = false;
    string mainSamplesClassFileTemp; // Test true classes
    bool mainSamplesClassFileInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;

    vector<bool> hasTrueClass; // Check if we have the true classes

    std::string weightsFileTemp;
    std::vector<std::string> weightsFilesTemp;
    bool weightsFileInit = false;
    string inputRulesFileTemp; // Rule file to be converted to hyperlocus, replace weight file
    bool inputRulesFileInit = false;

    string ruleFileTemp;
    bool ruleFileInit = false;
    string statsFileTemp;
    bool statsFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;

    int nbDimlpNets = 1; // Number of networks. 1 means no bagging

    int nbQuantLevels = 50; // Number of steps of the step function
    double hiKnot = 5;      // High end of the interval for each dimension, a hyperplan can't be after

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

    // Import parameters

    if (nbParam <= 1) {
      showFidexParams();
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

        case 'S':
          mainSamplesDataFileTemp = arg;
          mainSamplesDataFileInit = true;
          break;

        case 'p':
          mainSamplesPredFileTemp = arg;
          mainSamplesPredFileInit = true;
          break;

        case 'c':
          mainSamplesClassFileTemp = arg;
          mainSamplesClassFileInit = true;
          break;

        case 'A':
          attributFileTemp = arg;
          attributFileInit = true;
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
          ruleFileTemp = arg;
          ruleFileInit = true;
          break;

        case 's':
          statsFileTemp = arg;
          statsFileInit = true;
          break;

        case 'r':
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case 'R':
          rootFolderTemp = arg;
          rootFolderInit = true;
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

    // ----------------------------------------------------------------------

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

    // create paths with root foler

    const char *trainDataFile = nullptr;
    const char *trainDataFilePred = nullptr;
    const char *trainDataFileTrueClass = nullptr;
    const char *mainSamplesDataFile = nullptr;
    const char *mainSamplesPredFile = nullptr;
    const char *mainSamplesClassFile = nullptr;
    const char *attributFile = nullptr;
    std::vector<const char *> weightsFiles;
    const char *inputRulesFile = nullptr;
    const char *ruleFile = nullptr;
    const char *statsFile = nullptr;
    const char *consoleFile = nullptr;

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

    if (mainSamplesDataFileInit) {
      mainSamplesDataFileTemp = root + mainSamplesDataFileTemp;
      mainSamplesDataFile = &mainSamplesDataFileTemp[0];
    }

    if (mainSamplesPredFileInit) {
      mainSamplesPredFileTemp = root + mainSamplesPredFileTemp;
      mainSamplesPredFile = &mainSamplesPredFileTemp[0];
    }

    if (mainSamplesClassFileInit) {
      mainSamplesClassFileTemp = root + mainSamplesClassFileTemp;
      mainSamplesClassFile = &mainSamplesClassFileTemp[0];
    }

    if (attributFileInit) {
      attributFileTemp = root + attributFileTemp;
      attributFile = &attributFileTemp[0];
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

    if (ruleFileInit) {
      ruleFileTemp = root + ruleFileTemp;
      ruleFile = &ruleFileTemp[0];
    }

    if (statsFileInit) {
      statsFileTemp = root + statsFileTemp;
      statsFile = &statsFileTemp[0];
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
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
    if (!mainSamplesDataFileInit) {
      throw CommandArgumentException("The test samples data file <value, prediction(if no -p), true class(if no -c)> has to be given with option -S");
    }
    if (mainSamplesClassFileInit && !mainSamplesPredFileInit) {
      throw CommandArgumentException("The test prediction data file(-p) needs to be specified if the test class data file(-c) is given");
    }
    if (!weightsFileInit && !inputRulesFileInit) {
      throw CommandArgumentException("A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
    } else if (weightsFileInit && inputRulesFileInit) {
      throw CommandArgumentException("Do not specify both a weight file(-W) and a rules file(-f). Choose one of them.");
    }
    if (!ruleFileInit) {
      throw CommandArgumentException("The output rule file has to be given with option -O");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    std::cout << "\nParameters :\n"
              << std::endl;
    std::cout << "- Max iteration number : " << itMax << endl;
    std::cout << "- Min size of covering : " << minNbCover << endl;
    if (dropoutDim) {
      std::cout << "- We use a dimension dropout of " << dropoutDimParam << endl;
    } else {
      std::cout << "- We don't use dimension dropout" << std::endl;
    }
    if (dropoutHyp) {
      std::cout << "- We use a hyperplan dropout of " << dropoutHypParam << endl;
    } else {
      std::cout << "- We don't use hyperplan dropout" << std::endl;
    }
    std::cout << endl;

    // Import files

    std::cout << "Import files..." << std::endl;

    float importTime;
    clock_t impt1;
    clock_t impt2;

    impt1 = clock();

    std::unique_ptr<DataSetFid> trainDatas(new DataSetFid(trainDataFile, trainDataFilePred, hasDecisionThreshold, decisionThreshold, indexPositiveClass, trainDataFileTrueClass));

    vector<vector<double>> *trainData = trainDatas->getDatas();
    vector<int> *trainPreds = trainDatas->getPredictions();
    bool hasConfidence = trainDatas->hasConfidence();
    vector<vector<double>> *trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
    vector<int> *trainTrueClass = trainDatas->getTrueClasses();

    if (minNbCover > ((*trainData).size())) {
      throw CommandArgumentException("Error : invalide type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
    }

    // Get test data
    vector<int> mainSamplesPreds;
    vector<int> mainSamplesTrueClass;
    vector<vector<double>> mainSamplesValues;
    vector<vector<double>> mainSamplesOutputValuesPredictions;

    if (!mainSamplesPredFileInit) { // If we have only one test data file with data, pred and class

      vector<double> mainSampleValues;
      vector<double> mainSampleOutputValuesPredictions;
      int mainSamplePred;
      int mainSampleTrueClass;
      fstream testData;
      testData.open(mainSamplesDataFile, ios::in); // Read data file
      if (testData.fail()) {
        throw FileNotFoundError("Error : file " + std::string(mainSamplesDataFile) + " not found");
      }
      string line;
      bool firstLine = true;
      while (!testData.eof()) {
        getline(testData, line);
        if (!checkStringEmpty(line)) {
          std::stringstream myLine(line);
          double value;
          mainSampleValues.clear();
          while (myLine >> value) {
            mainSampleValues.push_back(value);
          }
          mainSamplesValues.push_back(mainSampleValues);
        } else if (firstLine) {
          throw FileFormatError("Error : in file " + std::string(mainSamplesDataFile) + ", first line is empty");
        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw FileFormatError("Error : file " + std::string(mainSamplesDataFile) + " is not on good format, there is more than one empty line between 2 samples");
            }
          }
          break; // There is just empty lines at the end of the file
        }
        if (testData.eof()) {
          throw FileContentError("Error : file " + std::string(mainSamplesDataFile) + " has not enough prediction data");
        }
        getline(testData, line);
        if (!checkStringEmpty(line)) {
          std::stringstream myLine(line);
          double value;
          mainSampleOutputValuesPredictions.clear();
          while (myLine >> value) {
            mainSampleOutputValuesPredictions.push_back(value);
          }
          mainSamplesOutputValuesPredictions.push_back(mainSampleOutputValuesPredictions);

          if (hasDecisionThreshold && mainSampleOutputValuesPredictions[indexPositiveClass] >= decisionThreshold) {
            mainSamplePred = indexPositiveClass;
          } else {
            mainSamplePred = static_cast<int>(std::max_element(mainSampleOutputValuesPredictions.begin(), mainSampleOutputValuesPredictions.end()) - mainSampleOutputValuesPredictions.begin());
          }

          mainSamplePred = static_cast<int>(std::max_element(mainSampleOutputValuesPredictions.begin(), mainSampleOutputValuesPredictions.end()) - mainSampleOutputValuesPredictions.begin());
          mainSamplesPreds.push_back(mainSamplePred);
        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw FileFormatError("Error : file " + std::string(mainSamplesDataFile) + " is not on good format, there is empty lines inbetween data");
            }
          }
          throw FileContentError("Error : file " + std::string(mainSamplesDataFile) + " has not enough prediction data");
        }
        bool endOfLine = false;
        if (testData.eof()) {
          endOfLine = true;
        }
        getline(testData, line);
        if (endOfLine || checkStringEmpty(line)) {
          hasTrueClass.push_back(false);
          mainSamplesTrueClass.push_back(-1);
        } else {
          // Check if true class is integer
          while (std::isspace(line.back())) {
            line.pop_back();
          }
          while (std::isspace(line.front())) {
            line.erase(line.begin());
          }
          const char *trueClassTest = strdup(line.c_str());
          if (!CheckPositiveInt(trueClassTest)) {
            std::cout << trueClassTest << std::endl;
            throw FileContentError("Error : in file " + std::string(mainSamplesDataFile) + ", true classes need to be positive integers");
          }
          hasTrueClass.push_back(true);
          std::stringstream myLine(line);
          myLine >> mainSampleTrueClass;
          mainSamplesTrueClass.push_back(mainSampleTrueClass);
          if (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw FileFormatError("Error : in file " + std::string(mainSamplesDataFile) + ", you need to have empty lines between samples");
            }
          }
        }
        firstLine = false;
      }

      testData.close(); // close data file
    } else {            // We have different files for test predictions and test classes
      std::unique_ptr<DataSetFid> testDatas(new DataSetFid(mainSamplesDataFile, mainSamplesPredFile, hasDecisionThreshold, decisionThreshold, indexPositiveClass));
      mainSamplesValues = (*testDatas->getDatas());
      mainSamplesPreds = (*testDatas->getPredictions());
      mainSamplesOutputValuesPredictions = (*testDatas->getOutputValuesPredictions());

      // Check if there is good number of lines
      if (mainSamplesPreds.size() != mainSamplesValues.size()) {
        throw FileContentError("Error : in file " + std::string(mainSamplesPredFile) + ", you need to specify as many predictions as there is datas");
      }

      // Classes :
      if (mainSamplesClassFileInit) {
        fstream classData;
        classData.open(mainSamplesClassFile, ios::in); // Read data file
        if (classData.fail()) {
          throw FileNotFoundError("Error : file " + std::string(mainSamplesClassFile) + " not found");
        }
        string line;
        int mainSampleTrueClass;
        while (!classData.eof()) {
          int ind = 0;
          getline(classData, line);
          if (!checkStringEmpty(line)) {

            std::stringstream myLine(line);
            string tempTest;
            bool found = false;
            while (myLine >> tempTest && !found) {
              if (tempTest != "-1" && tempTest != "0" && tempTest != "1") {
                throw FileContentError("Error : in file " + std::string(mainSamplesClassFile) + ", true classes need to be 0, 1 or -1(no class)");
              }
              mainSampleTrueClass = stoi(tempTest);
              if (mainSampleTrueClass == 1) {
                mainSamplesTrueClass.push_back(ind);
                hasTrueClass.push_back(true);
                found = true;
              } else if (mainSampleTrueClass == -1) {
                mainSamplesTrueClass.push_back(-1);
                hasTrueClass.push_back(false);
                found = true;
              }
              ind++;
            }
            if (!found) {
              throw FileContentError("Error : in file " + std::string(mainSamplesClassFile) + ", true classes need to be have at least a 1 or -1(no class) in each line");
            }
          }
        }

        // Check if there is good number of lines
        if (mainSamplesTrueClass.size() != mainSamplesValues.size()) {
          throw FileContentError("Error : in file " + std::string(mainSamplesClassFile) + ", you need to specify as many true classes as there is datas (-1 if no true class)");
        }
      } else {
        hasTrueClass.resize(mainSamplesValues.size(), false);
      }
    }

    const size_t nbSamples = mainSamplesValues.size();
    const size_t nbAttributs = mainSamplesValues[0].size();
    const size_t nbClass = mainSamplesOutputValuesPredictions[0].size();

    if (indexPositiveClass >= static_cast<int>(nbClass)) {
      throw CommandArgumentException("Error : parameter positive_index(-x) has to be a positive integer smaller than " + to_string(nbClass));
    }

    for (int spl = 0; spl < nbSamples; spl++) {
      if (mainSamplesValues[spl].size() != nbAttributs) {
        throw FileContentError("Error : in file " + std::string(mainSamplesDataFile) + ", all test datas need to have the same number of variables");
      }
    }

    for (int spl = 0; spl < nbSamples; spl++) {
      if (mainSamplesOutputValuesPredictions[spl].size() != nbClass) {
        throw FileContentError("Error : in file " + std::string(mainSamplesDataFile) + ", all test datas need to have the same number of prediction values");
      }
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

    impt2 = clock();
    importTime = (float)(impt2 - impt1) / CLOCKS_PER_SEC;

    std::cout << "\nImport time = " << importTime << " sec" << std::endl;

    std::cout << "Files imported" << endl
              << endl;
    std::cout << "----------------------------------------------" << endl
              << endl;

    float temps2;
    clock_t d1;
    clock_t d2;

    d1 = clock();

    vector<string> lines;

    // Initialize random number generator

    if (seed == 0) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      auto seedValue = currentTime.time_since_epoch().count();
      seed = static_cast<unsigned int>(seedValue);
    }
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    // compute hyperspace
    std::cout << "Creation of hyperspace..." << std::endl;

    std::vector<std::vector<double>> matHypLocus;

    if (weightsFileInit) {
      for (const auto &weightsFile : weightsFiles) {
        std::vector<std::vector<double>> hypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end());                      // Concatenate hypLocus to matHypLocus
      }
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, nbAttributs);
    }

    FidexNameSpace::Hyperspace hyperspace(matHypLocus); // Initialize hyperbox and get hyperplans

    const size_t nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributs));
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    // Stats
    double meanFidelity = 0;
    double meanConfidence = 0;
    double meanCovSize = 0;
    double meanNbAntecedentsPerRule = 0;
    double meanAccuracy = 0;

    /*float sampleTime;
    clock_t samplet1;
    clock_t samplet2;*/

    // We compute rule for each test sample
    for (int currentSample = 0; currentSample < nbSamples; currentSample++) {

      // samplet1 = clock();

      lines.push_back("Rule for sample " + std::to_string(currentSample) + " :\n");
      if (nbSamples > 1) {
        std::cout << "Computation of rule for sample " << currentSample << " : " << endl
                  << endl;
      }

      if (nbSamples == 1) {
        std::cout << "Searching for discriminating hyperplans..." << std::endl;
      }

      // Compute initial covering
      vector<int> coveredSamples((*trainData).size());                    // Samples covered by the hyperbox
      std::iota(std::begin(coveredSamples), std::end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

      // Store covering and compute initial fidelty
      hyperspace.getHyperbox()->setCoveredSamples(coveredSamples);
      hyperspace.getHyperbox()->computeFidelity(mainSamplesPreds[currentSample], trainPreds); // Compute fidelity of initial hyperbox

      if (nbSamples == 1) {
        std::cout << "Initial fidelity : " << hyperspace.getHyperbox()->getFidelity() << endl
                  << endl;
      }

      int nbIt = 0;

      while (hyperspace.getHyperbox()->getFidelity() != 1 && nbIt < itMax) { // While fidelity of our hyperbox is not 100%
        // std::cout << "New iteration : " << nbIt << std::endl;
        std::unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());    // best hyperbox to choose for next step
        std::unique_ptr<Hyperbox> currentHyperbox(new Hyperbox()); // best hyperbox to choose for next step
        double mainSampleValue;
        int attribut;
        int dimension;
        int indexBestHyp = -1;
        int bestDimension = -1;
        int minHyp = -1; // Index of first hyperplan without any change of the best hyperplan
        int maxHyp = -1;
        // Randomize dimensions
        vector<int> dimensions(nbIn);
        std::iota(std::begin(dimensions), std::end(dimensions), 0); // Vector from 0 to nbIn-1
        std::shuffle(std::begin(dimensions), std::end(dimensions), gen);
        vector<int> currentCovSamp;

        /*float itTime;
        clock_t itt1;
        clock_t itt2;
        itt1 = clock();*/
        for (int d = 0; d < nbIn; d++) { // Loop on all dimensions
          if (bestHyperbox->getFidelity() == 1) {
            break;
          }
          dimension = dimensions[d];
          attribut = dimension % nbAttributs;
          mainSampleValue = mainSamplesValues[currentSample][attribut];
          // Test if we dropout this dimension

          if (dropoutDim && dis(gen) < dropoutDimParam) {
            continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
          }

          bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

          size_t nbHyp = hyperspace.getHyperLocus()[dimension].size();
          if (nbHyp == 0) {
            continue; // No data on this dimension
          }
          // std::cout << std::endl;
          // std::cout << "NEW DIM" << d << std::endl;
          for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)
            // std::cout << "\rNEW hyp: " << k+1 << "/" << nbHyp << std::flush;
            //  Test if we dropout this hyperplan
            if (dropoutHyp && dis(gen) < dropoutHypParam) {
              continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
            }

            double hypValue = hyperspace.getHyperLocus()[dimension][k];
            bool mainSampleGreater = hypValue <= mainSampleValue;

            // Check if main sample value is on the right of the hyperplan
            currentHyperbox->computeCoveredSamples(hyperspace.getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
            currentHyperbox->computeFidelity(mainSamplesPreds[currentSample], trainPreds);
            // Compute fidelity
            // If the fidelity is better or is same with better covering but not if covering size is lower than minNbCover
            if (currentHyperbox->getCoveredSamples().size() >= minNbCover && (currentHyperbox->getFidelity() > bestHyperbox->getFidelity() || (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() > bestHyperbox->getCoveredSamples().size()))) {
              bestHyperbox->setFidelity(currentHyperbox->getFidelity()); // Update best hyperbox
              bestHyperbox->setCoveredSamples(currentHyperbox->getCoveredSamples());
              indexBestHyp = k;
              minHyp = k; // New best
              maxHyp = -1;
              maxHypBlocked = false; // We can increase maxHyp if next is the same
              bestDimension = dimension;
            } else if (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() == bestHyperbox->getCoveredSamples().size()) {
              if (!maxHypBlocked) {
                maxHyp = k; // Index of last (for now) hyperplan which is equal to the best.
              }

            } else {
              maxHypBlocked = true; // we can't increase maxHyp anymore for this best hyperplan
            }

            if (bestHyperbox->getFidelity() == 1) {
              break;
            }
          }
        }

        // Modication of our hyperbox with the best at this iteration and modify discriminative hyperplans
        if (indexBestHyp != -1 && bestDimension != -1) { // If we found any good dimension with good hyperplan (with enough covering)
          if (maxHyp != -1) {
            indexBestHyp = (maxHyp + minHyp) / 2;
          }
          // Rule is not added if fidelity and covering size did not increase
          if (bestHyperbox->getFidelity() > hyperspace.getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == hyperspace.getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > hyperspace.getHyperbox()->getCoveredSamples().size())) {
            // std::cout << "Found new best" << std::endl;
            // std::cout << "Fidelity : " << bestHyperbox->getFidelity() << std::endl;
            // std::cout << "Cov size :" << bestHyperbox->getCoveredSamples().size() << endl;
            hyperspace.getHyperbox()->setFidelity(bestHyperbox->getFidelity());
            hyperspace.getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
            hyperspace.getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
          } else if (minNbCover == 1 && dropoutDim == false && dropoutHyp == false) {
            // std::cout << "Choosing randomly" << std::endl;
            //  If we have a minimum covering of 1 and no dropout, we need to choose randomly a hyperplan, because we are stocked
            std::unique_ptr<Hyperbox> randomHyperbox(new Hyperbox()); // best hyperbox to choose for next step

            std::uniform_int_distribution<int> distribution(0, static_cast<int>(nbIn) - 1);
            int randomDimension;
            size_t hypSize;
            do {
              randomDimension = dimensions[distribution(gen)];
              hypSize = hyperspace.getHyperLocus()[randomDimension].size();
            } while (hypSize == 0);
            attribut = randomDimension % nbAttributs;
            mainSampleValue = mainSamplesValues[currentSample][attribut];
            std::uniform_int_distribution<int> distributionHyp(0, static_cast<int>(hypSize) - 1);
            int indexRandomHyp = dimensions[distributionHyp(gen)];
            double hypValue = hyperspace.getHyperLocus()[randomDimension][indexRandomHyp];
            bool mainSampleGreater = hypValue <= mainSampleValue;

            // Check if main sample value is on the right of the hyperplan
            randomHyperbox->computeCoveredSamples(hyperspace.getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
            randomHyperbox->computeFidelity(mainSamplesPreds[currentSample], trainPreds);

            hyperspace.getHyperbox()->setFidelity(randomHyperbox->getFidelity());
            hyperspace.getHyperbox()->setCoveredSamples(randomHyperbox->getCoveredSamples());
            hyperspace.getHyperbox()->discriminateHyperplan(randomDimension, indexRandomHyp);
          }
        }
        /*itt2 = clock();
        itTime = (float)(itt2 - itt1) / CLOCKS_PER_SEC;

        std::cout << "\n Iteration time = " << itTime << " sec" << std::endl;*/
        nbIt += 1;
      }

      meanFidelity += hyperspace.getHyperbox()->getFidelity();
      std::cout << "Final fidelity : " << hyperspace.getHyperbox()->getFidelity() << endl;
      if (hyperspace.getHyperbox()->getFidelity() != 1) {
        std::cout << "WARNING Fidelity is not maximum! You may want to try again." << std::endl;
        std::cout << "If you can't find a maximal fidelity, try a lowest minimal covering" << std::endl;
        std::cout << "If this is not enough, put the min covering to 1 and do not use dropout." << std::endl;
      } else {
        std::cout << endl;
      }
      if (nbSamples == 1) {
        std::cout << "Discriminating hyperplans generated." << endl
                  << endl;
      }
      // Rule extraction
      if (nbSamples == 1) {
        std::cout << "Rule extraction" << endl
                  << endl;
      }

      // Compute rule accuracy and confidence
      double ruleAccuracy;
      if (hasTrueClass[currentSample]) {
        bool mainSampleCorrect = mainSamplesPreds[currentSample] == mainSamplesTrueClass[currentSample];
        ruleAccuracy = hyperspace.computeRuleAccuracy(trainPreds, trainTrueClass, mainSampleCorrect); // Percentage of correct model prediction on samples covered by the rule
      } else {
        ruleAccuracy = hyperspace.computeRuleAccuracy(trainPreds, trainTrueClass); // Percentage of correct model prediction on samples covered by the rule
      }

      meanAccuracy += ruleAccuracy;
      std::cout << "Rule accuracy : " << ruleAccuracy << endl
                << endl;

      int currentSamplePred = mainSamplesPreds[currentSample];
      double currentSamplePredValue = mainSamplesOutputValuesPredictions[currentSample][currentSamplePred];
      double ruleConfidence;
      if (hasConfidence) {
        ruleConfidence = hyperspace.computeRuleConfidence(trainOutputValuesPredictions, currentSamplePred, currentSamplePredValue); // Mean output value of prediction of class chosen by the rule for the covered samples
        meanConfidence += ruleConfidence;
        std::cout << "Rule confidence : " << ruleConfidence << endl
                  << endl;
      } else {
        ruleConfidence = -1;
      }

      meanCovSize += static_cast<double>(hyperspace.getHyperbox()->getCoveredSamples().size());
      meanNbAntecedentsPerRule += static_cast<double>(hyperspace.getHyperbox()->getDiscriminativeHyperplans().size());
      // Extract rules
      hyperspace.ruleExtraction(&mainSamplesValues[currentSample], currentSamplePred, ruleAccuracy, ruleConfidence, lines, attributFileInit, &attributeNames, hasClassNames, &classNames);

      std::cout << "Result found after " << nbIt << " iterations." << std::endl;

      /*samplet2 = clock();
      sampleTime = (float)(samplet2 - samplet1) / CLOCKS_PER_SEC;

      std::cout << "\n Computation time = " << sampleTime << " sec" << std::endl;*/

      std::cout << "-------------------------------------------------" << std::endl;

      if (nbSamples > 1) {
        lines.emplace_back("-------------------------------------------------\n");
      }

      // Reinitialize discriminativeHyperplans for next sample
      if (nbSamples > 1) {
        hyperspace.getHyperbox()->resetDiscriminativeHyperplans();
      }
    }

    // Stats
    meanFidelity /= static_cast<double>(nbSamples);
    if (hasConfidence) {
      meanConfidence /= static_cast<double>(nbSamples);
    }
    meanCovSize /= static_cast<double>(nbSamples);
    meanNbAntecedentsPerRule /= static_cast<double>(nbSamples);
    meanAccuracy /= static_cast<double>(nbSamples);

    if (statsFileInit) {
      ofstream outputStatsFile(statsFile);
      if (outputStatsFile.is_open()) {
        outputStatsFile << "Statistics with a test set of " << nbSamples << " samples :\n"
                        << std::endl;
        outputStatsFile << "The mean covering size per rule is : " << meanCovSize << "" << std::endl;
        outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntecedentsPerRule << "" << std::endl;
        outputStatsFile << "The mean rule fidelity rate is : " << meanFidelity << "" << std::endl;
        outputStatsFile << "The mean rule accuracy is : " << meanAccuracy << "" << std::endl;
        if (hasConfidence) {
          outputStatsFile << "The mean rule confidence is : " << meanConfidence << "" << std::endl;
        }
        outputStatsFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open stats extraction file " + std::string(statsFile) + ".");
      }
    }

    ofstream outputFile(ruleFile);
    if (outputFile.is_open()) {
      for (const auto &line : lines) {
        outputFile << line << "" << std::endl;
      }
      outputFile.close();
    } else {
      throw CannotOpenFileError("Error : Couldn't open rule extraction file " + std::string(ruleFile) + ".");
    }

    d2 = clock();
    temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;

    std::cout << "\nTime without data import = " << temps2 << " sec" << std::endl;

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

/*

  Exemple pour lancer le code :

  ./fidex -T datanorm -P dimlp.out -C dataclass2 -S testSampleDataCombine -W dimlp.wts -Q 50 -I 5 -O rule.txt -s stats -i 100 -v 25 -d 0.5 -h 0.5 -R ../fidex/datafiles

  ./fidex -T datanorm -P dimlp.out -C dataclass2 -S testData.txt -p testPred.txt -c testClass.txt -W dimlp.wts -Q 50 -I 5 -O rule.txt -s stats -i 100 -v 25 -d 0.5 -h 0.5 -R ../fidex/datafiles
  ./fidex -T isoletTrainData.txt -P isoletTrainPredV2.out -C isoletTrainClass.txt -S isoletTestData.txt -p isoletTestPredV2.out -c isoletTestClass.txt -W isoletV2.wts -Q 50 -I 5 -O ruleFidex.txt -s stats -i 100 -v 25 -d 0.5 -h 0.5 -R ../dimlp/datafiles/isoletDataset
  ./fidex -T Train/X_train.txt -P Train/pred_trainV2.out -C Train/y_train.txt -S Test/X_test.txt -p Test/pred_testV2.out -c Test/y_test.txt -W HAPTV2.wts -Q 50 -I 5 -O ruleFidexV2.txt -s stats -i 100 -v 2 -d 0.5 -h 0.5 -R ../dimlp/datafiles/HAPTDataset


  #include <profileapi.h>

  LARGE_INTEGER frequency;        - ticks per second
  LARGE_INTEGER t1, t2;           - ticks
  double elapsedTime;

  get ticks per second
  QueryPerformanceFrequency(&frequency);

  start timer
  QueryPerformanceCounter(&t1);

  do something
  ...

  stop timer
  QueryPerformanceCounter(&j2WB2);

  compute and print the elapsed time in millisec
  elapsedTimeWB2 = (j2WB2.QuadPart - j1WB2.QuadPart) * 1000.0 / frequencyWB2.QuadPart;
  std::printf("\npart 2 : Full execution time = %f sec\n", elapsedTimeWB2);

*/
