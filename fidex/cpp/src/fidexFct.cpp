#include "checkFun.h"
#include "dataSet.h"
#include "hyperspace.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidex -R <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
  std::cout << "-T <train dataset file> -P <train prediction file> -C <train true class file> ";
  std::cout << "-S <test sample(s) data file with data, prediction(if no -p) and true class(if no -c)> ";
  std::cout << "-H <hyperLocus file> ";
  std::cout << "-O <Rule output file>";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-p <test prediction file> ";
  std::cout << "-c <test true class file> If at least -p is specified, -S needs to have only test datas\n";
  std::cout << "-s <output statistic file>\n";
  std::cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  std::cout << "-i <max iteration number>\n";
  std::cout << "-v <minimum covering number>\n";
  std::cout << "-d <dimension dropout parameter>\n";
  std::cout << "-h <hyperplan dropout parameter>\n";
  std::cout << "-z <seed (0=ranodom)>";

  std::cout << "\n-------------------------------------------------\n\n";
}

bool checkStringEmpty(string line) {
  if (line.length() == 0) {
    return true;
  } else {
    for (int c : line) {
      if (isgraph(c)) {
        return false;
      }
    }
    return true;
  }
}

int fidex(string command) {

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
      return 0;
    }

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

    vector<bool> hasTrueClass; // Check if we have the true classes

    string hyperLocusFileTemp;
    bool hyperLocusFileInit = false;
    string ruleFileTemp;
    bool ruleFileInit = false;
    string statsFileTemp;
    bool statsFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;

    int itMax = 100;         // We stop if we have more than itMax iterations
    int minNbCover = 2;      // Minimum size of covering that we ask
    bool dropoutHyp = false; // We dropout a bunch of hyperplans each iteration (could accelerate the processus)
    double dropoutHypParam = 0.5;
    bool dropoutDim = false; // We dropout a bunch of dimensions each iteration (could accelerate the processus)
    double dropoutDimParam = 0.5;
    DataSetFid *trainDatas;
    DataSetFid *testDatas;

    // Import parameters

    for (int p = 1; p < nbParam; p++) { // We skip "fidex"
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

        case 'S':
          mainSamplesDataFileTemp = &(commandList[p])[0];
          mainSamplesDataFileInit = true;
          break;

        case 'p':
          mainSamplesPredFileTemp = &(commandList[p])[0];
          mainSamplesPredFileInit = true;
          break;

        case 'c':
          mainSamplesClassFileTemp = &(commandList[p])[0];
          mainSamplesClassFileInit = true;
          break;

        case 'H':
          hyperLocusFileTemp = &(commandList[p])[0];
          hyperLocusFileInit = true;
          break;

        case 'O':
          ruleFileTemp = &(commandList[p])[0];
          ruleFileInit = true;
          break;

        case 's':
          statsFileTemp = &(commandList[p])[0];
          statsFileInit = true;
          break;

        case 'r':
          consoleFileTemp = &(commandList[p])[0];
          consoleFileInit = true;
          break;

        case 'R':
          rootFolderTemp = &(commandList[p])[0];
          rootFolderInit = true;
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

    char trainDataFileTmp[160], trainDataFilePredTmp[160], trainDataFileTrueClassTmp[160], mainSamplesDataFileTmp[160], mainSamplesPredFileTmp[160], mainSamplesClassFileTmp[160], hyperLocusFileTmp[160], ruleFileTmp[160], statsFileTmp[160], consoleFileTmp[160];

    char *trainDataFile = 0;
    char *trainDataFilePred = 0;
    char *trainDataFileTrueClass = 0;
    char *mainSamplesDataFile = 0;
    char *mainSamplesPredFile = 0;
    char *mainSamplesClassFile = 0;
    char *hyperLocusFile = 0;
    char *ruleFile = 0;
    char *statsFile = 0;
    char *consoleFile = 0;

#ifdef __unix__
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

    if (mainSamplesDataFileInit) {
      mainSamplesDataFileTemp = root + mainSamplesDataFileTemp;
      if (mainSamplesDataFileTemp.length() >= 160) {
        cout << "Path " << mainSamplesDataFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(mainSamplesDataFileTmp, mainSamplesDataFileTemp.c_str());
      mainSamplesDataFile = mainSamplesDataFileTmp;
    }

    if (mainSamplesPredFileInit) {
      mainSamplesPredFileTemp = root + mainSamplesPredFileTemp;
      if (mainSamplesPredFileTemp.length() >= 160) {
        cout << "Path " << mainSamplesPredFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(mainSamplesPredFileTmp, mainSamplesPredFileTemp.c_str());
      mainSamplesPredFile = mainSamplesPredFileTmp;
    }

    if (mainSamplesClassFileInit) {
      mainSamplesClassFileTemp = root + mainSamplesClassFileTemp;
      if (mainSamplesClassFileTemp.length() >= 160) {
        cout << "Path " << mainSamplesClassFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(mainSamplesClassFileTmp, mainSamplesClassFileTemp.c_str());
      mainSamplesClassFile = mainSamplesClassFileTmp;
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

    if (ruleFileInit) {
      ruleFileTemp = root + ruleFileTemp;
      if (ruleFileTemp.length() >= 160) {
        cout << "Path " << ruleFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(ruleFileTmp, ruleFileTemp.c_str());
      ruleFile = ruleFileTmp;
    }

    if (statsFileInit) {
      statsFileTemp = root + statsFileTemp;
      if (statsFileTemp.length() >= 160) {
        cout << "Path " << statsFileTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(statsFileTmp, statsFileTemp.c_str());
      statsFile = statsFileTmp;
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
    if (!mainSamplesDataFileInit) {
      throw std::runtime_error("The test samples data file <value, prediction(if no -p), true class(if no -c)> has to be given with option -S");
    }
    if (mainSamplesClassFileInit && !mainSamplesPredFileInit) {
      throw std::runtime_error("The test prediction data file(-p) needs to be specified if the test class data file(-c) is given");
    }
    if (!hyperLocusFileInit) {
      throw std::runtime_error("The hyperLocus file has to be given with option -H");
    }
    if (!ruleFileInit) {
      throw std::runtime_error("The output rule file has to be given with option -O");
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
      std::cout << "- We use a dimension dropout of " << dropoutDimParam << endl;
    } else {
      std::cout << "- We don't use dimension dropout\n";
    }
    if (dropoutHyp) {
      std::cout << "- We use a hyperplan dropout of " << dropoutHypParam << endl;
    } else {
      std::cout << "- We don't use hyperplan dropout\n";
    }
    std::cout << endl;

    // Import files

    std::cout << "Import files..." << endl;

    trainDatas = new DataSetFid(trainDataFile, trainDataFilePred, trainDataFileTrueClass);

    vector<vector<double>> *trainData = trainDatas->getDatas();
    vector<int> *trainPreds = trainDatas->getPredictions();
    vector<vector<double>> *trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
    vector<int> *trainTrueClass = trainDatas->getTrueClasses();

    if (minNbCover > ((*trainData).size())) {
      throw std::runtime_error("Error : invalide type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
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
        throw std::runtime_error("Error : file " + std::string(mainSamplesDataFile) + " not found");
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
          throw std::runtime_error("Error : in file " + std::string(mainSamplesDataFile) + ", first line is empty");
        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw std::runtime_error("Error : file " + std::string(mainSamplesDataFile) + " is not on good format, there is more than one empty line between 2 samples");
            }
          }
          break; // There is just empty lines at the end of the file
        }
        if (testData.eof()) {
          throw std::runtime_error("Error : file " + std::string(mainSamplesDataFile) + " has not enough prediction data");
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
          mainSamplePred = std::max_element(mainSampleOutputValuesPredictions.begin(), mainSampleOutputValuesPredictions.end()) - mainSampleOutputValuesPredictions.begin();
          mainSamplesPreds.push_back(mainSamplePred);
        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw std::runtime_error("Error : file " + std::string(mainSamplesDataFile) + " is not on good format, there is empty lines inbetween data");
            }
          }
          throw std::runtime_error("Error : file " + std::string(mainSamplesDataFile) + " has not enough prediction data");
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
          char *trueClassTest = new char[line.length() + 1];
          strcpy(trueClassTest, line.c_str());
          if (!CheckPositiveInt(trueClassTest)) {
            throw std::runtime_error("Error : in file " + std::string(mainSamplesDataFile) + ", true classes need to be positive integers");
          }
          hasTrueClass.push_back(true);
          std::stringstream myLine(line);
          myLine >> mainSampleTrueClass;
          mainSamplesTrueClass.push_back(mainSampleTrueClass);
          if (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw std::runtime_error("Error : in file " + std::string(mainSamplesDataFile) + ", you need to have empty lines between samples");
            }
          }
        }
        firstLine = false;
      }

      testData.close(); // close data file
    } else {            // We have different files for test predictions and test classes
      testDatas = new DataSetFid(mainSamplesDataFile, mainSamplesPredFile);
      mainSamplesValues = (*testDatas->getDatas());
      mainSamplesPreds = (*testDatas->getPredictions());
      mainSamplesOutputValuesPredictions = (*testDatas->getOutputValuesPredictions());

      // Classes :
      if (mainSamplesClassFileInit) {
        fstream classData;
        classData.open(mainSamplesClassFile, ios::in); // Read data file
        if (classData.fail()) {
          throw std::runtime_error("Error : file " + std::string(mainSamplesClassFile) + " not found");
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
            while (myLine >> tempTest) {
              if (tempTest != "-1" && tempTest != "0" && tempTest != "1") {
                throw std::runtime_error("Error : in file " + std::string(mainSamplesClassFile) + ", true classes need to be 0, 1 or -1(no class)");
              }
              mainSampleTrueClass = stoi(tempTest);
              if (mainSampleTrueClass == 1) {
                mainSamplesTrueClass.push_back(ind);
                hasTrueClass.push_back(true);
                found = true;
                break;
              } else if (mainSampleTrueClass == -1) {
                mainSamplesTrueClass.push_back(-1);
                hasTrueClass.push_back(false);
                found = true;
                break;
              }
              ind++;
            }
            if (!found) {
              throw std::runtime_error("Error : in file " + std::string(mainSamplesClassFile) + ", true classes need to be have at least a 1 or -1(no class) in each line");
            }
          }
        }

        // Check if there is good number of lines
        if (mainSamplesTrueClass.size() != mainSamplesValues.size()) {
          throw std::runtime_error("Error : in file " + std::string(mainSamplesClassFile) + ", you need to specify as many true classes as there is datas (-1 if no true class)");
        }
      } else {
        for (int i = 0; i < mainSamplesValues.size(); i++) {
          hasTrueClass.push_back(false);
        }
      }
    }

    int nbSamples = mainSamplesValues.size();
    const int nbAttributs = mainSamplesValues[0].size();
    const int nbClass = mainSamplesOutputValuesPredictions[0].size();

    for (int s = 0; s < nbSamples; s++) {
      if (mainSamplesValues[s].size() != nbAttributs) {
        throw std::runtime_error("Error : in file " + std::string(mainSamplesDataFile) + ", all test datas need to have the same number of variables");
      }
    }

    for (int s = 0; s < nbSamples; s++) {
      if (mainSamplesOutputValuesPredictions[s].size() != nbClass) {
        throw std::runtime_error("Error : in file " + std::string(mainSamplesDataFile) + ", all test datas need to have the same number of prediction values");
      }
    }

    std::cout << "Files imported" << endl
              << endl;
    std::cout << "----------------------------------------------" << endl
              << endl;

    float temps2;
    clock_t d1, d2;

    d1 = clock();

    vector<string> lines;

    if (seed == 0) {
      std::srand(time(0)); // Initialize random number generator
    } else {
      std::srand(seed);
    }

    // compute hyperspace
    std::cout << "Creation of hyperspace..." << endl;

    Hyperspace hyperspace(hyperLocusFile); // Initialize hyperbox and get hyperplans

    const int nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    const int nbHyp = hyperspace.getHyperLocus()[0].size();

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0) {
      throw std::runtime_error("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributs));
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    // Stats
    double meanFidelity = 0;
    double meanConfidence = 0;
    double meanCovSize = 0;
    double meanNbAntecedentsPerRule = 0;
    double meanAccuracy = 0;

    // We compute rule for each test sample
    for (int currentSample = 0; currentSample < nbSamples; currentSample++) {

      lines.push_back("Rule for sample " + std::to_string(currentSample) + " :\n");
      if (nbSamples > 1) {
        std::cout << "Computation of rule for sample " << currentSample << " : " << endl
                  << endl;
      }

      if (nbSamples == 1) {
        std::cout << "Searching for discriminating hyperplans..." << endl;
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

        unsigned seedShuffle;
        if (seed == 0) {
          seedShuffle = std::chrono::system_clock::now().time_since_epoch().count();
        } else {
          unsigned seedShuffle = seed;
        }
        // cout << endl << "It." << nbIt << " F : " << hyperspace.getHyperbox()->getFidelity() << ", att : " << attribut << endl;
        Hyperbox *bestHyperbox = new Hyperbox(); // best hyperbox to choose for next step
        Hyperbox *currentHyperbox = new Hyperbox();
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
        std::shuffle(std::begin(dimensions), std::end(dimensions), std::default_random_engine(seedShuffle));
        vector<int> currentCovSamp;

        for (int d = 0; d < nbIn; d++) { // Loop on all dimensions

          if (bestHyperbox->getFidelity() == 1) {
            break;
          }
          dimension = dimensions[d];
          attribut = dimension % nbAttributs;
          mainSampleValue = mainSamplesValues[currentSample][attribut];
          // Test if we dropout this dimension
          if (dropoutDim) {
            if ((double)rand() / (RAND_MAX) < dropoutDimParam) {
              continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
            }
          }
          bool maxHypBlocked;

          for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)

            maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox
            // Test if we dropout this hyperplan
            if (dropoutHyp) {
              if ((double)rand() / (RAND_MAX) < dropoutHypParam) {
                continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
              }
            }

            double hypValue = hyperspace.getHyperLocus()[dimension][k];
            bool mainSampleGreater = hypValue <= mainSampleValue;

            // Check if main sample value is on the right of the hyperplan
            currentHyperbox->computeCoveredSamples(hyperspace.getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue, &mainSamplesValues[currentSample]); // Compute new cover samples
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
              maxHypBlocked = true; // we can't increase maxHyp anymmore for this best hyperplan
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
            hyperspace.getHyperbox()->setFidelity(bestHyperbox->getFidelity());
            hyperspace.getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
            hyperspace.getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
          }
        }

        nbIt += 1;
      };

      meanFidelity += hyperspace.getHyperbox()->getFidelity();
      std::cout << "Final fidelity : " << hyperspace.getHyperbox()->getFidelity() << endl;
      if (hyperspace.getHyperbox()->getFidelity() != 1) {
        std::cout << "WARNING Fidelity is not maximum! You may want to try again." << endl
                  << endl;
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

      // Compute rule accuracy
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
      double ruleConfidence = hyperspace.computeRuleConfidence(trainOutputValuesPredictions, currentSamplePred, currentSamplePredValue); // Mean output value of prediction of class chosen by the rule for the covered samples

      meanConfidence += ruleConfidence;
      std::cout << "Rule confidence : " << ruleConfidence << endl
                << endl;

      meanCovSize += hyperspace.getHyperbox()->getCoveredSamples().size();
      meanNbAntecedentsPerRule += hyperspace.getHyperbox()->getDiscriminativeHyperplans().size();
      hyperspace.ruleExtraction(&mainSamplesValues[currentSample], currentSamplePred, ruleAccuracy, ruleConfidence, lines);

      std::cout << "Result found after " << nbIt << " iterations." << endl;

      std::cout << "-------------------------------------------------" << endl;

      if (nbSamples > 1) {
        lines.push_back("-------------------------------------------------\n");
      }

      // Reinitialize discriminativeHyperplans for next sample
      if (nbSamples > 1) {
        hyperspace.getHyperbox()->resetDiscriminativeHyperplans();
      }
    }

    // Stats
    meanFidelity /= nbSamples;
    meanConfidence /= nbSamples;
    meanCovSize /= nbSamples;
    meanNbAntecedentsPerRule /= nbSamples;
    meanAccuracy /= nbSamples;

    if (statsFileInit) {
      ofstream outputStatsFile(statsFile);
      if (outputStatsFile.is_open()) {
        outputStatsFile << "Statistics with a test set of " << nbSamples << " samples :\n\n";
        outputStatsFile << "The mean covering size per rule is : " << meanCovSize << "\n";
        outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntecedentsPerRule << "\n";
        outputStatsFile << "The mean rule fidelity rate is : " << meanFidelity << "\n";
        outputStatsFile << "The mean rule accuracy is : " << meanAccuracy << "\n";
        outputStatsFile << "The mean rule confidence is : " << meanConfidence << "\n";
        outputStatsFile.close();
      } else {
        throw std::runtime_error("Error : Couldn't open stats extraction file " + std::string(statsFile) + ".");
      }
    }

    ofstream outputFile(ruleFile);
    if (outputFile.is_open()) {
      for (int l = 0; l < lines.size(); l++) {
        outputFile << lines[l] + "\n";
      }
      outputFile.close();
    } else {
      throw std::runtime_error("Error : Couldn't open rule extraction file " + std::string(ruleFile) + ".");
    }

    d2 = clock();
    temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;

    std::cout << "\nTime without data import = " << temps2 << " sec\n";

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

// Exemple : .\fidex.exe -T datanorm -P dimlp.out -C dataclass2 -S testSampleDataCombine -H hyperLocus -O rule.txt -s stats -i 100 -v 25 -d 0.5 -h 0.5 -R ../fidex/datafiles

// .\fidex.exe -T datanorm -P dimlp.out -C dataclass2 -S testData.txt -p testPred.txt -c testClass.txt -H hyperLocus -O rule.txt -s stats -i 100 -v 25 -d 0.5 -h 0.5 -R ../fidex/datafiles

/*
#include <profileapi.h>

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double elapsedTime;

// get ticks per second
QueryPerformanceFrequency(&frequency);

// start timer
QueryPerformanceCounter(&t1);

// do something
...

// stop timer
QueryPerformanceCounter(&j2WB2);

// compute and print the elapsed time in millisec
elapsedTimeWB2 = (j2WB2.QuadPart - j1WB2.QuadPart) * 1000.0 / frequencyWB2.QuadPart;
std::printf("\npart 2 : Full execution time = %f sec\n", elapsedTimeWB2);
*/
