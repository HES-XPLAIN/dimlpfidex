#include "DimlpTrnFct.h"
#include <algorithm>
#include <random>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif
#include "../../../fidex/cpp/src/fidexFct.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "../../../fidexGlo/cpp/src/fidexGloRulesFct.h"
#include "../../../fidexGlo/cpp/src/fidexGloStatsFct.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

bool removeDirectory(const std::string &path) {
  struct stat fileStat;
  if (stat(path.c_str(), &fileStat) != 0) { // If the folder doesn't exist
    std::cout << "the folder " << path << " doesn't exist and trying to delete it..." << std::endl;
    return false;
  }

  if (!S_ISDIR(fileStat.st_mode)) { // Check if it is a file and not a folder
    std::cout << "Error : A file with same folder name (" << path << ") exists." << std::endl;
    return false;
  }

  // Delete files and sub-folders recursively
#ifdef _WIN32
  std::wstring wpath(path.begin(), path.end());
  if (!RemoveDirectoryW(wpath.c_str())) {
    DWORD error = GetLastError();
    if (error == ERROR_DIR_NOT_EMPTY) {
      std::string command = "cmd /c rmdir /s /q \"" + path + "\"";
      int result = std::system(command.c_str());
      if (result != 0) {
        std::cout << "Error when deleting directory and its content: " << path << "." << std::endl;
        return false;
      }
    } else {
      std::cout << "Error when deleting directory: " << path << "." << std::endl;
      return false;
    }
  }
#else
  std::string command = "rm -rf " + path;
  int result = std::system(command.c_str());

  if (result == 0) {
    return true;
  } else {
    return false;
  }
#endif
  return true;
}

bool createOrClearDirectory(const std::string &path) {
  struct stat fileStat;
  if (stat(path.c_str(), &fileStat) == 0) { // If the folder already exists
    if (!S_ISDIR(fileStat.st_mode)) {       // Check if it is a file and not a folder
      std::cout << "Error : A file with same folder name (" << path << ") exists." << std::endl;
      return false;
    }

    // Delete existing folder and his content
    if (removeDirectory(path) == false) {
      std::cout << "Error when deleting directory and his content" << path << ", try deleting it manually." << std::endl;
      return false;
    }
  }

  // Create folder
#ifdef _WIN32
  if (_mkdir(path.c_str()) != 0) {
    std::cout << "Error during creation of the directory " << path << "." << std::endl;
    return false;
  }
#else
#define CREATE_DIRECTORY(path) mkdir(path, 0700)
  if (mkdir(path.c_str(), 0700) != 0) {
    std::cout << "Error during creation of the directory " << path << "." << std::endl;
    return false;
  }
#endif

  return true;
}

static void GiveAllParam()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "crossValid -C <choose the algorithms by giving : fidex fidexGlo or both>";
  cout << "-L <training set file(path with respect to specified root folder)> -1 <file of train classes> ";
  cout << "-h <high side of the interval> "; // Ex: 5
  cout << "-I <number of input neurons> -O <number of output neurons>";

  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-K <K-fold cross-validation>\n";                                                        // Not to be 10
  cout << "-N <number of times we do the cross-validation>\n";                                     // Not to be 10
  cout << "-F <Folder where to save crossValidation data>\n";                                      // Not to be CrossValidation
  cout << "-M <FidexGlo heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo>\n"; // not to be 1
  cout << "-o <output statistic file name, saved in specific crossValidation folder>\n";           // Not to be crossValidationStats.txt
  cout << "-A <file of attributes>\n";
  cout << "-W <file of pretrained weights>\n";
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)\n";
  cout << "-Hk <number of neurons in the kth hidden layer>\n";
  cout << "-Q <number of stairs in staircase activation function> ";        // Not to be 50
  cout << "-l <back-propagation learning parameter (Eta)>\n";               // Not to be 0.1
  cout << "-m <back-propagation momentum parameter (Mu)>\n";                // Not to be 0.6
  cout << "-f <back-propagation flat spot elimination parameter (Flat)>\n"; // Not to be 0.01
  cout << "-e <error threshold>\n";                                         // Not to be -1111111111.0
  cout << "-a <accuracy threshold>\n";                                      // Not to be 11111111111111.0
  cout << "-d <absolute difference error threshold>\n";                     // Not to be 0
  cout << "-i <number of train epochs>\n";                                  // Not to be 1500
  cout << "-s <number of train epochs to show error>\n";                    // Not to be 10
  cout << "-n <max fidex and fidexGlo iteration number>\n";                 // Not to be 100
  cout << "-v <minimum fidex and fidexGlo covering number>\n";              // Not to be 2
  cout << "-x <dimension dropout parameter for fidex and fidexGlo>\n";
  cout << "-y <hyperplan dropout parameter for fidex and fidexGlo>\n";
  cout << "-z <seed (0=random)>"; // Not to be 0

  cout << "\n-------------------------------------------------\n\n";
}

// copy in binary mode
bool copyFile(const char *SRC, const char *DEST) {
  std::ifstream src(SRC, std::ios::binary);
  std::ofstream dest(DEST, std::ios::binary);
  dest << src.rdbuf();
  return src && dest;
}

////////////////////////////////////////////////////////////

int main(int nbParam, char **param)

{

  float temps;
  clock_t t1;
  clock_t t2;

  t1 = clock();

  int N = 10; // Number of times we do cross-validation
  int K = 10; // Number of divisions of the dataset (10 = 8 for train, 1 for test and 1 for validation)
  int seed = 0;

  string learnTarTemp;
  bool learnTarInit = false;
  string learnFileTemp;
  bool learnFileInit = false;
  string folderTemp = "CrossValidation";
  string rootFolderTemp;
  bool rootFolderInit = false;
  string algorithm;
  bool algorithmInit = false;
  string statFile = "crossValidationStats.txt";
  string quant = "50";
  string eta = "0.1";
  string mu = "0.6";
  string flat = "0.01";
  string errThres = "-1111111111.0";
  string accThres = "11111111111111.0";
  string deltaErr = "0";
  string showErr = "10";
  string epochs = "1500";
  string itMax = "100";
  string minNbCover = "2";
  string heuristic = "1"; // FidexGlo heuristic
  bool isFidex = false;
  bool isFidexGlo = false;
  bool dropoutDim = false;
  bool dropoutHyp = false;

  string genericCommand = "dimlpTrn";
  string fidexGenericCommand = "fidex";
  string fidexGloGenericCommand = "fidexGloRules";
  string fidexGloStatsGenericCommand = "fidexGloStats";

  string hiKnot = "5";
  string nbIn;
  string nbOut;
  string arch;
  string arch2;
  string attrFile;
  string weightFile;
  string dropoutDimParam;
  string dropoutHypParam;

  bool nbInInit = false;
  bool nbOutInit = false;
  const char *ptrParam;

#if defined(__unix__) || defined(__APPLE__)
  string separator = "/";
#elif defined(_WIN32)
  string separator = "\\";
#endif

  if (nbParam <= 1) {
    GiveAllParam();
    return 0;
  }

  int p = 1; // We skip "crossValid"
  while (p < nbParam) {
    if (*param[p] == '-') {
      p++;

      if (p >= nbParam && *(param[p - 1] + 1) != 'R') {
        cout << "Missing something at the end of the command.\n";
        return -1;
      }

      char option = *(param[p - 1] + 1);

      switch (option) {
      case 'l':
        eta = param[p];
        genericCommand += " -l " + eta;
        break;

      case 'm':
        mu = param[p];
        genericCommand += " -m " + mu;
        break;

      case 'f':
        flat = param[p];
        genericCommand += " -f " + flat;
        break;

      case 'e':
        errThres = param[p];
        genericCommand += " -e " + errThres;
        break;

      case 'a':
        accThres = param[p];
        genericCommand += " -a " + accThres;
        break;

      case 'd':

        deltaErr = param[p];
        genericCommand += " -d " + deltaErr;
        break;

      case 's':
        showErr = param[p];
        genericCommand += " -s " + showErr;
        break;

      case 'i':
        epochs = param[p];
        genericCommand += " -i " + epochs;
        break;

      case 'Q':
        quant = param[p];
        break;

      case 'h':
        hiKnot = param[p];
        fidexGenericCommand += " -I " + hiKnot;
        fidexGloGenericCommand += " -I " + hiKnot;
        break;

      case 'I':
        nbIn = param[p];
        nbInInit = true;
        genericCommand += " -I " + nbIn;
        break;

      case 'H':
        ptrParam = param[p - 1];

        if (ptrParam[2] != '\0') {
          arch = param[p];
          arch2 = ptrParam[2];
          genericCommand += " -H" + arch2 + " " + arch;
        } else {
          cout << "Which hidden layer (-H) ?\n";
          return -1;
        }

        break;

      case 'O':
        nbOut = param[p];
        nbOutInit = true;
        genericCommand += " -O " + nbOut;
        break;

      case 'S':
        rootFolderTemp = param[p];
        rootFolderInit = true;

        genericCommand += " -S .." + separator + rootFolderTemp;
        fidexGenericCommand += " -R .." + separator + rootFolderTemp;
        fidexGloGenericCommand += " -S .." + separator + rootFolderTemp;
        fidexGloStatsGenericCommand += " -S .." + separator + rootFolderTemp;
        break;

      case 'A':
        attrFile = param[p];
        genericCommand += " -A " + attrFile;
        break;

      case 'W':
        weightFile = param[p];
        genericCommand += " -W " + weightFile;
        break;

      case 'L':
        learnFileTemp = param[p];
        learnFileInit = true;
        break;

      case '1':
        learnTarTemp = param[p];
        learnTarInit = true;
        break;

      case 'C':
        algorithm = param[p];
        algorithmInit = true;
        break;

      case 'M':
        heuristic = param[p];
        break;

      case 'o':
        statFile = param[p];
        break;

      case 'K':
        if (CheckInt(param[p]))
          K = atoi(param[p]);
        else
          return -1;

        break;

      case 'N':
        if (CheckInt(param[p]))
          N = atoi(param[p]);
        else
          return -1;

        break;

      case 'n':
        itMax = param[p];
        fidexGenericCommand += " -i " + itMax;
        fidexGloGenericCommand += " -i " + itMax;
        break;

      case 'v':
        minNbCover = param[p];
        fidexGenericCommand += " -v " + minNbCover;
        fidexGloGenericCommand += " -v " + minNbCover;
        break;

      case 'x':
        dropoutDimParam = param[p];
        dropoutDim = true;
        fidexGenericCommand += " -d " + dropoutDimParam;
        fidexGloGenericCommand += " -d " + dropoutDimParam;
        break;

      case 'y':
        dropoutHypParam = param[p];
        dropoutHyp = true;
        fidexGenericCommand += " -h " + dropoutHypParam;
        fidexGloGenericCommand += " -h " + dropoutHypParam;
        break;

      case 'z':
        if (CheckInt(param[p])) {
          seed = atoi(param[p]);
          genericCommand += " -z " + std::to_string(seed);
          fidexGenericCommand += " -z " + std::to_string(seed);
          fidexGloGenericCommand += " -z " + std::to_string(seed);
        }

        else
          return -1;

        break;

      case 'F':
        folderTemp = param[p];
        break;

      default:
        cout << "Illegal option: " << param[p - 1] << "\n";
        return -1;
      }
    }

    else {
      cout << "Illegal option: " << param[p] << "\n";
      return -1;
    }

    p++;
  }

  // ----------------------------------------------------------------------

  if (algorithmInit == false) {
    cout << "Give a algorithm with -C selection please. You can give fidex, fidexGlo or both."
         << "\n";
    return -1;
  }

  if (algorithm != "fidex" && algorithm != "fidexGlo" && algorithm != "both") {
    cout << "Give a correct algorithm with -C selection please. You can give fidex, fidexGlo or both."
         << "\n";
    return -1;
  }

  if (algorithm == "fidex" || algorithm == "both") {
    isFidex = true;
  }

  if (algorithm == "fidexGlo" || algorithm == "both") {
    isFidexGlo = true;
  }

  if (nbInInit == false) {
    cout << "Give the number of input neurons with option with -I selection please."
         << "\n";
    return -1;
  }

  if (nbOutInit == false) {
    cout << "Give the number of output neurons with option with -O selection please."
         << "\n";
    return -1;
  }

  // create paths with root foler

  const char *learnTar = nullptr;
  const char *learnFile = nullptr;
  const char *folder = nullptr;

  string root;
  if (rootFolderInit) {
    root = ".." + separator + rootFolderTemp + separator;
  } else {
    root = ".." + separator;
    genericCommand += " -S .." + separator;
    fidexGenericCommand += " -R .." + separator;
    fidexGloGenericCommand += " -S .." + separator;
    fidexGloStatsGenericCommand += " -S .." + separator;
  }

  if (learnTarInit) {
    learnTarTemp = root + learnTarTemp;
    learnTar = &learnTarTemp[0];
  }

  if (learnFileInit) {
    learnFileTemp = root + learnFileTemp;
    learnFile = &learnFileTemp[0];
  }

  string folderTempFull = root + folderTemp;
  folder = &folderTempFull[0];

  statFile = std::string(folder) + separator + statFile;

  // ----------------------------------------------------------------------

  if (N <= 0) {
    cout << "The number of times we do cross-validation must be at least 1, change the option N.\n";
    return -1;
  }

  if (K <= 2) {
    cout << "The number of divisions of the dataset must be at least 3, change the option -I.\n";
    return -1;
  }

  if (learnFileInit == false) {
    cout << "Give the training file with -L selection please."
         << "\n";
    return -1;
  }

  if (learnTarInit == false) {
    cout << "Give the training target file with -1 selection please."
         << "\n";
    return -1;
  }

  // Get datas on vector
  fstream fileDta;

  fileDta.open(learnFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error : file " + std::string(learnFile) + " not found");
  }
  vector<string> learnData;
  string line;
  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (line.length() != 0) {
      learnData.push_back(line);
    }
  }
  fileDta.close(); // close data file

  const size_t nbSamples = learnData.size();
  if (K > nbSamples) {
    cout << "The number of divisions K of the dataset must be less or equal to the number of train samples(" << nbSamples << ").\n";
    return -1;
  }

  vector<string> learnTarData;

  // Get datas on vector
  fstream fileDtaTar;

  fileDtaTar.open(learnTar, ios::in); // Read data file
  if (fileDtaTar.fail()) {
    throw FileNotFoundError("Error : file " + std::string(learnTar) + " not found");
  }

  while (!fileDtaTar.eof()) {
    getline(fileDtaTar, line);
    if (!checkStringEmpty(line)) {
      learnTarData.push_back(line);
    }
  }
  fileDtaTar.close(); // close data file

  if (learnTarData.size() != nbSamples) {
    cout << "The number of samples in each learn files need to be the same.\n";
    return -1;
  }

  if (createOrClearDirectory(folder)) { // Create folder if doesn't exist
    std::cout << "Folder created." << std::endl;
  } else {
    throw CannotCreateDirectoryError("Error during the creation of the directory " + std::string(folder) + ".");
  }

  // Initialize random number generator

  std::random_device rd;
  std::vector<int> seeds;
  if (seed != 0) { // Not random
    for (int i = 0; i < 10 * N; ++i)
      seeds.push_back(i);
    std::mt19937 gen(seed);
    std::shuffle(seeds.begin(), seeds.end(), gen);
  }

  // Create temp file strings for train, test and validation
  const char *trainFileStr = nullptr;
  string trainFileStrTemp = root + "tempTrain.txt";
  trainFileStr = &trainFileStrTemp[0];

  const char *testFileStr = nullptr;
  string testFileStrTemp = root + "tempTest.txt";
  testFileStr = &testFileStrTemp[0];

  const char *validFileStr = nullptr;
  string validFileStrTemp = root + "tempValid.txt";
  validFileStr = &validFileStrTemp[0];

  const char *trainTarFileStr = nullptr;
  string trainTarFileStrTemp = root + "tempTarTrain.txt";
  trainTarFileStr = &trainTarFileStrTemp[0];

  const char *testTarFileStr = nullptr;
  string testTarFileStrTemp = root + "tempTarTest.txt";
  testTarFileStr = &testTarFileStrTemp[0];

  const char *validTarFileStr = nullptr;
  string validTarFileStrTemp = root + "tempTarValid.txt";
  validTarFileStr = &validTarFileStrTemp[0];

  // Statistics for Fidex
  // One execution
  double meanCovSizeFid = 0.0;
  double meanNbAntFid = 0.0;
  double meanFidelFid = 0.0;
  double meanAccFid = 0.0;
  double meanConfidFid = 0.0;

  // All executions
  double meanCovSizeFidAll = 0.0;
  double meanNbAntFidAll = 0.0;
  double meanFidelFidAll = 0.0;
  double meanAccFidAll = 0.0;
  double meanConfidFidAll = 0.0;

  double stdCovSizeFidAll = 0.0;
  double stdNbAntFidAll = 0.0;
  double stdFidelFidAll = 0.0;
  double stdAccFidAll = 0.0;
  double stdConfidFidAll = 0.0;

  // Statistics for FidexGlo
  // One execution
  double meanNbRules = 0.0;
  double meanNbCover = 0.0;
  double meanNbAntecedants = 0.0;
  double meanFidelGlo = 0.0;
  double meanAccGlo = 0.0;
  double meanExplGlo = 0.0;
  double meanDefaultRate = 0.0;
  double meanNbFidelActivations = 0.0;
  double meanWrongActivations = 0.0;
  double meanTestAccGlo = 0.0;
  double meanTestAccWhenRulesAndModelAgree = 0.0;
  double meanTestAccWhenActivatedRulesAndModelAgree = 0.0;

  // All executions
  double meanNbRulesAll = 0.0;
  double meanNbCoverAll = 0.0;
  double meanNbAntecedantsAll = 0.0;
  double meanFidelGloAll = 0.0;
  double meanAccGloAll = 0.0;
  double meanExplGloAll = 0.0;
  double meanDefaultRateAll = 0.0;
  double meanNbFidelActivationsAll = 0.0;
  double meanWrongActivationsAll = 0.0;
  double meanTestAccGloAll = 0.0;
  double meanTestAccWhenRulesAndModelAgreeAll = 0.0;
  double meanTestAccWhenActivatedRulesAndModelAgreeAll = 0.0;

  double stdNbRulesAll = 0.0;
  double stdNbCoverAll = 0.0;
  double stdNbAntecedantsAll = 0.0;
  double stdFidelGloAll = 0.0;
  double stdAccGloAll = 0.0;
  double stdExplGloAll = 0.0;
  double stdDefaultRateAll = 0.0;
  double stdNbFidelActivationsAll = 0.0;
  double stdWrongActivationsAll = 0.0;
  double stdTestAccGloAll = 0.0;
  double stdTestAccWhenRulesAndModelAgreeAll = 0.0;
  double stdTestAccWhenActivatedRulesAndModelAgreeAll = 0.0;

  vector<vector<double>> meanFoldValuesFidex;    // each mean value in an entire fold for each fold for fidex
  vector<vector<double>> meanFoldValuesFidexGlo; // each mean value in an entire fold for each fold for fidexGlo

  std::ofstream outputStatsFile; // Output stats file

  // Parameters
  outputStatsFile.open(statFile, std::ios::app);
  if (outputStatsFile.is_open()) {

    outputStatsFile << "Parameters for " << N << " times " << K << "-Cross validation :\n";
    outputStatsFile << "---------------------------------------------------------\n";
    outputStatsFile << "Training of dimlp with " << nbIn << " input neurons and " << nbOut << " output neurons\n";
    outputStatsFile << "The number of stairs in staircase activation function is " << quant << " and the interval in which hyperplans are contained is [-" << hiKnot << "," << hiKnot << "]\n";
    outputStatsFile << "The back-propagation learning parameter (Eta) is " << eta << "\n";
    outputStatsFile << "The back-propagation momentum parameter (Mu) is " << mu << "\n";
    outputStatsFile << "The back-propagation flat spot elimination parameter (Flat) is " << flat << "\n";
    outputStatsFile << "The error threshold is " << errThres << "\n";
    outputStatsFile << "The accuracy threshold is " << accThres << "\n";
    outputStatsFile << "The absolute difference error threshold is " << deltaErr << "\n";
    outputStatsFile << "The number of train epochs is " << epochs << "\n";
    outputStatsFile << "The number of train epochs to show error is " << showErr << "\n";
    outputStatsFile << "The max fidex and fidexGlo iteration number is " << itMax << "\n";
    outputStatsFile << "The minimum fidex and fidexGlo covering number is " << minNbCover << "\n";
    if (dropoutHyp) {
      outputStatsFile << "The hyperplan dropout parameter for fidex and fidexGlo is " << dropoutHypParam << "\n";
    } else {
      outputStatsFile << "There is no hyperplan dropout\n";
    }
    if (dropoutDim) {
      outputStatsFile << "The dimension dropout parameter for fidex and fidexGlo is " << dropoutDimParam << "\n";
    } else {
      outputStatsFile << "There is no dimension dropout\n";
    }

    outputStatsFile << "---------------------------------------------------------\n\n";
    outputStatsFile.close();
  } else {
    throw CannotOpenFileError("Error : Couldn't open stats extraction file " + std::string(statFile) + ".");
  }

  // Loop on N executions of cross-validation
  for (int n = 0; n < N; n++) {
    cout << "n=" << n + 1 << endl;

// Create folder for this execution
#if defined(__unix__) || defined(__APPLE__)
    string folderName = std::string(folder) + "/Execution" + std::to_string(n + 1) + "/";
    mkdir(folderName.c_str(), 0700);
#elif defined(_WIN32)
    string folderName = std::string(folder) + "\\Execution" + std::to_string(n + 1);
    mkdir(folderName.c_str());
#endif

    // Randomly split data in K sub-parts
    vector<vector<string>> learnDataSplit;
    vector<vector<string>> learnTarDataSplit;

    std::vector<int> indexes;
    for (int i = 0; i < nbSamples; ++i)
      indexes.push_back(i);
    if (seed != 0) { // Not random
      std::mt19937 g(seeds[n]);
      std::shuffle(indexes.begin(), indexes.end(), g);
    } else { // random
      std::mt19937 g(rd());
      std::shuffle(indexes.begin(), indexes.end(), g);
    }

    // Vector of evenly spaced numbers between 1 and nbSammples+1
    vector<double> range;
    double delta = static_cast<double>(nbSamples) / static_cast<double>(K);
    for (int i = 0; i < K + 1; i++) {
      range.push_back(i * delta);
    }
    // Split data evenly in K groups
    for (int q = 0; q < range.size() - 1; q++) {
      vector<string> tempVect;
      vector<string> tempVectTar;
      for (auto ind = static_cast<int>(range[q]); ind < static_cast<int>(range[q + 1]); ind++) {
        tempVect.push_back(learnData[indexes[ind]]);
        tempVectTar.push_back(learnTarData[indexes[ind]]);
      }
      learnDataSplit.push_back(tempVect);
      learnTarDataSplit.push_back(tempVectTar);
    }

    int validationIdx;
    int testIdx;
    for (int k = 0; k < K; k++) { // K-fold, we shift each time groups by 1.
      cout << "----" << endl;
      cout << "k=" << k + 1 << endl;
      // path to folder
      string folderPath = std::string(folder) + separator + "Execution" + std::to_string(n + 1) + separator + "Fold" + std::to_string(k + 1);
      // Create folder
#if defined(__unix__) || defined(__APPLE__)
      string folderFoldName = folderPath + separator;
      mkdir(folderFoldName.c_str(), 0700);
#elif defined(_WIN32)
      string folderFoldName = folderPath;
      mkdir(folderFoldName.c_str());
#endif
      // Get group index for test, validation and train
      vector<int> trainIdx;
      validationIdx = k;
      testIdx = (k + 1) % K;
      for (int m = 2; m < K; m++) {
        trainIdx.push_back((k + m) % K);
      }
      // Creation of train, test and validation files (temp files)
      ofstream trainFile(trainFileStr);
      if (trainFile.fail()) {
        throw CannotOpenFileError("Error : temp train file cound'nt open");
      }
      for (int id : trainIdx) {
        for (string lineTrn : learnDataSplit[id]) {
          trainFile << lineTrn << endl;
        }
      }
      trainFile.close();

      ofstream testFile(testFileStr);
      if (testFile.fail()) {
        throw CannotOpenFileError("Error : temp test file cound'nt open");
      }
      for (string lineTst : learnDataSplit[testIdx]) {
        testFile << lineTst << endl;
      }
      testFile.close();

      ofstream validFile(validFileStr);
      if (validFile.fail()) {
        throw CannotOpenFileError("Error : temp valid file cound'nt open");
      }
      for (string lineVal : learnDataSplit[validationIdx]) {
        validFile << lineVal << endl;
      }
      validFile.close();

      ofstream trainTarFile(trainTarFileStr);
      if (trainTarFile.fail()) {
        throw CannotOpenFileError("Error : temp trainTar file cound'nt open");
      }
      for (int id : trainIdx) {
        for (string lineTrntar : learnTarDataSplit[id]) {
          trainTarFile << lineTrntar << endl;
        }
      }
      trainTarFile.close();

      ofstream testTarFile(testTarFileStr);
      if (testTarFile.fail()) {
        throw CannotOpenFileError("Error : temp testTar file cound'nt open");
      }
      for (string lineTstTar : learnTarDataSplit[testIdx]) {
        testTarFile << lineTstTar << endl;
      }
      testTarFile.close();

      ofstream validTarFile(validTarFileStr);
      if (validTarFile.fail()) {
        throw CannotOpenFileError("Error : temp validTar file cound'nt open");
      }
      for (string lineValTar : learnTarDataSplit[validationIdx]) {
        validTarFile << lineValTar << endl;
      }
      validTarFile.close();

      // Get train, test and validation files in folder

      if (!copyFile(trainFileStr, (folderPath + separator + "train.txt").c_str())) {
        cout << "File tempTrain.txt coundn't be copied.\n";
        return -1;
      }
      if (!copyFile(testFileStr, (folderPath + separator + "test.txt").c_str())) {
        cout << "File tempTest.txt coundn't be copied.\n";
        return -1;
      }
      if (!copyFile(validFileStr, (folderPath + separator + "valid.txt").c_str())) {
        cout << "File tempValid.txt coundn't be copied.\n";
        return -1;
      }
      if (!copyFile(trainTarFileStr, (folderPath + separator + "trainTarget.txt").c_str())) {
        cout << "File tempTarTrain.txt coundn't be copied.\n";
        return -1;
      }
      if (!copyFile(testTarFileStr, (folderPath + separator + "testTarget.txt").c_str())) {
        cout << "File tempTarTest.txt coundn't be copied.\n";
        return -1;
      }
      if (!copyFile(validTarFileStr, (folderPath + separator + "validTarget.txt").c_str())) {
        cout << "File tempTarValid.txt coundn't be copied.\n";
        return -1;
      }

      // Train datas with DimlpTrn
      genericCommand += " -q " + quant;
      string command = genericCommand;
      string folderPathFromRoot = std::string(folderTemp) + separator + "Execution" + std::to_string(n + 1) + separator + "Fold" + std::to_string(k + 1);

      command += " -L " + folderPathFromRoot + separator + "train.txt ";
      command += "-T " + folderPathFromRoot + separator + "test.txt ";
      command += "-V " + folderPathFromRoot + separator + "valid.txt ";
      command += "-1 " + folderPathFromRoot + separator + "trainTarget.txt ";
      command += "-2 " + folderPathFromRoot + separator + "testTarget.txt ";
      command += "-3 " + folderPathFromRoot + separator + "validTarget.txt ";

      command += "-p " + folderPathFromRoot + separator + "train.out ";   // Output train pred file
      command += "-t " + folderPathFromRoot + separator + "test.out ";    // Output test pred file
      command += "-v " + folderPathFromRoot + separator + "valid.out ";   // Output validation pred file
      command += "-w " + folderPathFromRoot + separator + "weights.wts "; // Output weight file

      command += "-r consoleTemp.txt"; // To not show console result
      cout << "Enter in DimlpTrn function" << endl;
      int res = dimlpTrn(command);
      if (res == -1) {
        return -1; // If there is an error in the Trn
      }

      fidexGenericCommand += " -Q " + quant;
      fidexGloGenericCommand += " -Q " + quant;
      fidexGenericCommand += " -W " + folderPathFromRoot + separator + "weights.wts ";
      fidexGloGenericCommand += " -W " + folderPathFromRoot + separator + "weights.wts ";

      if (isFidex) {
        // Compute fidex stats in folder
        string fidexCommand = fidexGenericCommand;

        fidexCommand += " -T " + folderPathFromRoot + separator + "train.txt ";
        fidexCommand += "-P " + folderPathFromRoot + separator + "train.out ";
        fidexCommand += "-C " + folderPathFromRoot + separator + "trainTarget.txt ";
        fidexCommand += "-S " + folderPathFromRoot + separator + "test.txt ";
        fidexCommand += "-p " + folderPathFromRoot + separator + "test.out ";
        fidexCommand += "-c " + folderPathFromRoot + separator + "testTarget.txt ";
        fidexCommand += "-O " + folderPathFromRoot + separator + "fidexRule.txt ";
        fidexCommand += "-s " + folderPathFromRoot + separator + "fidexStats.txt ";
        fidexCommand += "-r " + folderPathFromRoot + separator + "fidexResult.txt ";

        cout << "Enter in fidex function" << endl;
        int resFid = fidex(fidexCommand);
        if (resFid == -1) {
          return -1; // If there is an error in fidex
        }

        // Get statistics from fidex
        string statsFile = "";
        statsFile = folderPath + separator + "fidexStats.txt";

        fstream statsData;
        statsData.open(statsFile, ios::in); // Read data file
        if (statsData.fail()) {
          throw FileNotFoundError("Error : fidex stat file " + std::string(statsFile) + " not found");
        }
        string lineStats;
        getline(statsData, lineStats);
        getline(statsData, lineStats);
        vector<double> statVals; // stat values
        while (!statsData.eof()) {
          getline(statsData, lineStats);
          if (!checkStringEmpty(lineStats)) {
            std::string lastElement(lineStats.substr(lineStats.rfind(" ") + 1));
            statVals.push_back(std::stod(lastElement));
          }
        }
        statsData.close();

        meanCovSizeFid += statVals[0];
        meanNbAntFid += statVals[1];
        meanFidelFid += statVals[2];
        meanAccFid += statVals[3];
        meanConfidFid += statVals[4];
      }

      if (isFidexGlo) {
        // Compute fidexGlo rules in folder
        string fidexGloCommand = fidexGloGenericCommand;

        fidexGloCommand += " -T " + folderPathFromRoot + separator + "train.txt ";
        fidexGloCommand += "-P " + folderPathFromRoot + separator + "train.out ";
        fidexGloCommand += "-C " + folderPathFromRoot + separator + "trainTarget.txt ";
        fidexGloCommand += "-O " + folderPathFromRoot + separator + "fidexGloRules.txt ";
        fidexGloCommand += "-r " + folderPathFromRoot + separator + "fidexGloResult.txt ";
        fidexGloCommand += "-M " + heuristic;

        cout << "Enter in fidexGloRules function" << endl;
        int resFidGloRu = fidexGloRules(fidexGloCommand);
        if (resFidGloRu == -1) {
          return -1; // If there is an error in fidexGlo
        }

        // Compute fidexGlo statistics in folder
        string fidexGloStatsCommand = fidexGloStatsGenericCommand;

        fidexGloStatsCommand += " -T " + folderPathFromRoot + separator + "test.txt ";
        fidexGloStatsCommand += "-P " + folderPathFromRoot + separator + "test.out ";
        fidexGloStatsCommand += "-C " + folderPathFromRoot + separator + "testTarget.txt ";
        fidexGloStatsCommand += "-R " + folderPathFromRoot + separator + "fidexGloRules.txt ";
        fidexGloStatsCommand += "-O " + folderPathFromRoot + separator + "fidexGloStats.txt ";
        fidexGloStatsCommand += "-r " + folderPathFromRoot + separator + "fidexGloStatsResult.txt ";

        cout << "Enter in fidexGloStats function" << endl;
        int resFidGloSt = fidexGloStats(fidexGloStatsCommand);
        if (resFidGloSt == -1) {
          return -1; // If there is an error in fidexGloStats
        }

        // Get statistics from fidexGlo
        string statsGloFile = "";
        statsGloFile = folderPath + separator + "fidexGloStats.txt";

        fstream statsGloData;
        statsGloData.open(statsGloFile, ios::in); // Read data file
        if (statsGloData.fail()) {
          throw FileNotFoundError("Error : fidex stat file " + std::string(statsGloFile) + " not found");
        }
        string lineStatsGlo;
        string value;
        string meanNbRulesStr;
        string meanNbCoverStr;
        string meanNbAntecedantsStr;

        getline(statsGloData, lineStatsGlo);
        getline(statsGloData, lineStatsGlo);

        if (!checkStringEmpty(lineStatsGlo)) {
          std::stringstream myLine(lineStatsGlo);
          while (myLine >> value) {
            if (value == "rules") {
              myLine >> value;
              myLine >> meanNbRulesStr;
              meanNbRulesStr.pop_back();
              meanNbRules += stod(meanNbRulesStr);
            }
            if (value == "sample") {
              myLine >> value;
              myLine >> value;
              myLine >> value;
              myLine >> value;
              myLine >> value;
              myLine >> meanNbCoverStr;
              meanNbCoverStr.pop_back();
              meanNbCover += stod(meanNbCoverStr);
            }
            if (value == "antecedents") {
              myLine >> value;
              myLine >> value;
              myLine >> value;
              myLine >> meanNbAntecedantsStr;
              meanNbAntecedants += stod(meanNbAntecedantsStr);
            }
          }
        } else {
          throw FileContentError("Error in second line of fidexGlo stat file.");
        }

        getline(statsGloData, lineStatsGlo);
        getline(statsGloData, lineStatsGlo);
        getline(statsGloData, lineStatsGlo);
        vector<double> statGloVals; // stat values
        while (!statsGloData.eof()) {
          getline(statsGloData, lineStatsGlo);
          if (!checkStringEmpty(lineStatsGlo)) {
            std::string lastElement(lineStatsGlo.substr(lineStatsGlo.rfind(" ") + 1));
            statGloVals.push_back(std::stod(lastElement));
          }
        }
        statsGloData.close();

        meanFidelGlo += statGloVals[0];
        meanAccGlo += statGloVals[1];
        meanExplGlo += statGloVals[2];
        meanDefaultRate += statGloVals[3];
        meanNbFidelActivations += statGloVals[4];
        meanWrongActivations += statGloVals[5];
        meanTestAccGlo += statGloVals[6];
        meanTestAccWhenRulesAndModelAgree += statGloVals[7];
        meanTestAccWhenActivatedRulesAndModelAgree += statGloVals[8];
      }
    }

    // Compute fold Stats
    vector<double> meanCurrentExecValuesFidex;
    if (isFidex) { // For Fidex
      meanCurrentExecValuesFidex.push_back(meanCovSizeFid / K);
      meanCovSizeFid = 0;
      meanCurrentExecValuesFidex.push_back(meanNbAntFid / K);
      meanNbAntFid = 0;
      meanCurrentExecValuesFidex.push_back(meanFidelFid / K);
      meanFidelFid = 0;
      meanCurrentExecValuesFidex.push_back(meanAccFid / K);
      meanAccFid = 0;
      meanCurrentExecValuesFidex.push_back(meanConfidFid / K);
      meanConfidFid = 0;
      meanFoldValuesFidex.push_back(meanCurrentExecValuesFidex);
    }
    vector<double> meanCurrentExecValuesFidexGlo;
    if (isFidexGlo) { // For FidexGlo
      meanCurrentExecValuesFidexGlo.push_back(meanNbRules / K);
      meanNbRules = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanNbCover / K);
      meanNbCover = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanNbAntecedants / K);
      meanNbAntecedants = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanFidelGlo / K);
      meanFidelGlo = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanAccGlo / K);
      meanAccGlo = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanExplGlo / K);
      meanExplGlo = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanDefaultRate / K);
      meanDefaultRate = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanNbFidelActivations / K);
      meanNbFidelActivations = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanWrongActivations / K);
      meanWrongActivations = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanTestAccGlo / K);
      meanTestAccGlo = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanTestAccWhenRulesAndModelAgree / K);
      meanTestAccWhenRulesAndModelAgree = 0;
      meanCurrentExecValuesFidexGlo.push_back(meanTestAccWhenActivatedRulesAndModelAgree / K);
      meanTestAccWhenActivatedRulesAndModelAgree = 0;
      meanFoldValuesFidexGlo.push_back(meanCurrentExecValuesFidexGlo);
    }

    // Output and show stats
    outputStatsFile.open(statFile, std::ios::app);
    if (outputStatsFile.is_open()) {

      outputStatsFile << "Results for execution " << n + 1 << " of " << K << "-Cross validation :\n\n";
      cout << "\n---------------------------------------------------------" << endl;
      cout << "---------------------------------------------------------" << endl
           << endl;
      cout << "Results for execution " << n + 1 << " of " << K << "-Cross validation :" << endl
           << endl;
      if (isFidex) {
        outputStatsFile << "Fidex :\n";
        outputStatsFile << "The mean covering size per rule is : " << meanCurrentExecValuesFidex[0] << "\n";
        outputStatsFile << "The mean number of antecedents per rule is : " << meanCurrentExecValuesFidex[1] << "\n";
        outputStatsFile << "The mean rule fidelity rate is : " << meanCurrentExecValuesFidex[2] << "\n";
        outputStatsFile << "The mean rule accuracy is : " << meanCurrentExecValuesFidex[3] << "\n";
        outputStatsFile << "The mean rule confidence is : " << meanCurrentExecValuesFidex[4] << "\n";
        cout << "Fidex :" << endl;
        cout << "The mean covering size per rule is : " << meanCurrentExecValuesFidex[0] << endl;
        cout << "The mean number of antecedents per rule is : " << meanCurrentExecValuesFidex[1] << endl;
        cout << "The mean rule fidelity rate is : " << meanCurrentExecValuesFidex[2] << endl;
        cout << "The mean rule accuracy is : " << meanCurrentExecValuesFidex[3] << endl;
        cout << "The mean rule confidence is : " << meanCurrentExecValuesFidex[4] << endl;
        cout << "\n---------------------------------------------------------" << endl;
        cout << "---------------------------------------------------------" << endl;
      }
      if (isFidex && isFidexGlo) {
        outputStatsFile << "\n---------------------------------------------------------\n\n";
        cout << endl;
      }
      if (isFidexGlo) {
        outputStatsFile << "FidexGlo :\n";
        outputStatsFile << "The mean number of rules is : " << meanCurrentExecValuesFidexGlo[0] << "\n";
        outputStatsFile << "The mean sample covering number per rule is : " << meanCurrentExecValuesFidexGlo[1] << "\n";
        outputStatsFile << "The mean number of antecedents per rule is : " << meanCurrentExecValuesFidexGlo[2] << "\n";
        outputStatsFile << "The mean global rule fidelity rate is : " << meanCurrentExecValuesFidexGlo[3] << "\n";
        outputStatsFile << "The mean global rule accuracy is : " << meanCurrentExecValuesFidexGlo[4] << "\n";
        outputStatsFile << "The mean explainability rate (when we can find a rule) is : " << meanCurrentExecValuesFidexGlo[5] << "\n";
        outputStatsFile << "The mean default rule rate (when we can't find a rule) is : " << meanCurrentExecValuesFidexGlo[6] << "\n";
        outputStatsFile << "The mean number of correct(fidel) activated rules per sample is : " << meanCurrentExecValuesFidexGlo[7] << "\n";
        outputStatsFile << "The mean number of wrong(not fidel) activated rules per sample is : " << meanCurrentExecValuesFidexGlo[8] << "\n";
        outputStatsFile << "The mean model test accuracy is : " << meanCurrentExecValuesFidexGlo[9] << "\n";
        outputStatsFile << "The mean model test accuracy when rules and model agree is : " << meanCurrentExecValuesFidexGlo[10] << "\n";
        outputStatsFile << "The mean model test accuracy when activated rules and model agree is : " << meanCurrentExecValuesFidexGlo[11] << "\n";
        cout << "FidexGlo :" << endl;
        cout << "The mean number of rules is : " << meanCurrentExecValuesFidexGlo[0] << endl;
        cout << "The mean sample covering number per rule is : " << meanCurrentExecValuesFidexGlo[1] << endl;
        cout << "The mean number of antecedents per rule is : " << meanCurrentExecValuesFidexGlo[2] << endl;
        cout << "The mean global rule fidelity rate is : " << meanCurrentExecValuesFidexGlo[3] << endl;
        cout << "The mean global rule accuracy is : " << meanCurrentExecValuesFidexGlo[4] << endl;
        cout << "The mean explainability rate (when we can find a rule) is : " << meanCurrentExecValuesFidexGlo[5] << endl;
        cout << "The mean default rule rate (when we can't find a rule) is : " << meanCurrentExecValuesFidexGlo[6] << endl;
        cout << "The mean number of correct(fidel) activated rules per sample is : " << meanCurrentExecValuesFidexGlo[7] << endl;
        cout << "The mean number of wrong(not fidel) activated rules per sample is : " << meanCurrentExecValuesFidexGlo[8] << endl;
        cout << "The mean model test accuracy is : " << meanCurrentExecValuesFidexGlo[9] << endl;
        cout << "The mean model test accuracy when rules and model agree is : " << meanCurrentExecValuesFidexGlo[10] << endl;
        cout << "The mean model test accuracy when activated rules and model agree is : " << meanCurrentExecValuesFidexGlo[11] << endl;
        cout << "\n---------------------------------------------------------" << endl;
        cout << "---------------------------------------------------------" << endl;
      }
      outputStatsFile << "\n---------------------------------------------------------\n";
      outputStatsFile << "---------------------------------------------------------\n\n";
      outputStatsFile.close();
    } else {
      throw CannotOpenFileError("Error : Couldn't open stats extraction file " + std::string(statFile) + ".");
    }
  }

  // Compute stats on all executions

  if (isFidex) { // For Fidex
    for (int exec = 0; exec < N; exec++) {
      meanCovSizeFidAll += meanFoldValuesFidex[exec][0];
      meanNbAntFidAll += meanFoldValuesFidex[exec][1];
      meanFidelFidAll += meanFoldValuesFidex[exec][2];
      meanAccFidAll += meanFoldValuesFidex[exec][3];
      meanConfidFidAll += meanFoldValuesFidex[exec][4];
    }
    meanCovSizeFidAll /= N;
    meanNbAntFidAll /= N;
    meanFidelFidAll /= N;
    meanAccFidAll /= N;
    meanConfidFidAll /= N;

    for (int exec = 0; exec < N; exec++) {
      stdCovSizeFidAll += pow(meanFoldValuesFidex[exec][0] - meanCovSizeFidAll, 2);
      stdNbAntFidAll += pow(meanFoldValuesFidex[exec][1] - meanNbAntFidAll, 2);
      stdFidelFidAll += pow(meanFoldValuesFidex[exec][2] - meanFidelFidAll, 2);
      stdAccFidAll += pow(meanFoldValuesFidex[exec][3] - meanAccFidAll, 2);
      stdConfidFidAll += pow(meanFoldValuesFidex[exec][4] - meanConfidFidAll, 2);
    }

    stdCovSizeFidAll = sqrt(stdCovSizeFidAll / N);
    stdNbAntFidAll = sqrt(stdNbAntFidAll / N);
    stdFidelFidAll = sqrt(stdFidelFidAll / N);
    stdAccFidAll = sqrt(stdAccFidAll / N);
    stdConfidFidAll = sqrt(stdConfidFidAll / N);
  }
  if (isFidexGlo) { // For FidexGlo
    for (int exec = 0; exec < N; exec++) {
      meanNbRulesAll += meanFoldValuesFidexGlo[exec][0];
      meanNbCoverAll += meanFoldValuesFidexGlo[exec][1];
      meanNbAntecedantsAll += meanFoldValuesFidexGlo[exec][2];
      meanFidelGloAll += meanFoldValuesFidexGlo[exec][3];
      meanAccGloAll += meanFoldValuesFidexGlo[exec][4];
      meanExplGloAll += meanFoldValuesFidexGlo[exec][5];
      meanDefaultRateAll += meanFoldValuesFidexGlo[exec][6];
      meanNbFidelActivationsAll += meanFoldValuesFidexGlo[exec][7];
      meanWrongActivationsAll += meanFoldValuesFidexGlo[exec][8];
      meanTestAccGloAll += meanFoldValuesFidexGlo[exec][9];
      meanTestAccWhenRulesAndModelAgreeAll += meanFoldValuesFidexGlo[exec][10];
      meanTestAccWhenActivatedRulesAndModelAgreeAll += meanFoldValuesFidexGlo[exec][11];
    }
    meanNbRulesAll /= N;
    meanNbCoverAll /= N;
    meanNbAntecedantsAll /= N;
    meanFidelGloAll /= N;
    meanAccGloAll /= N;
    meanExplGloAll /= N;
    meanDefaultRateAll /= N;
    meanNbFidelActivationsAll /= N;
    meanWrongActivationsAll /= N;
    meanTestAccGloAll /= N;
    meanTestAccWhenRulesAndModelAgreeAll /= N;
    meanTestAccWhenActivatedRulesAndModelAgreeAll /= N;

    for (int exec = 0; exec < N; exec++) {
      stdNbRulesAll += pow(meanFoldValuesFidexGlo[exec][0] - meanNbRulesAll, 2);
      stdNbCoverAll += pow(meanFoldValuesFidexGlo[exec][1] - meanNbCoverAll, 2);
      stdNbAntecedantsAll += pow(meanFoldValuesFidexGlo[exec][2] - meanNbAntecedantsAll, 2);
      stdFidelGloAll += pow(meanFoldValuesFidexGlo[exec][3] - meanFidelGloAll, 2);
      stdAccGloAll += pow(meanFoldValuesFidexGlo[exec][4] - meanAccGloAll, 2);
      stdExplGloAll += pow(meanFoldValuesFidexGlo[exec][5] - meanExplGloAll, 2);
      stdDefaultRateAll += pow(meanFoldValuesFidexGlo[exec][6] - meanDefaultRateAll, 2);
      stdNbFidelActivationsAll += pow(meanFoldValuesFidexGlo[exec][7] - meanNbFidelActivationsAll, 2);
      stdWrongActivationsAll += pow(meanFoldValuesFidexGlo[exec][8] - meanWrongActivationsAll, 2);
      stdTestAccGloAll += pow(meanFoldValuesFidexGlo[exec][9] - meanTestAccGloAll, 2);
      stdTestAccWhenRulesAndModelAgreeAll += pow(meanFoldValuesFidexGlo[exec][10] - meanTestAccWhenRulesAndModelAgreeAll, 2);
      stdTestAccWhenActivatedRulesAndModelAgreeAll += pow(meanFoldValuesFidexGlo[exec][11] - meanTestAccWhenActivatedRulesAndModelAgreeAll, 2);
    }

    stdNbRulesAll = sqrt(stdNbRulesAll / N);
    stdNbCoverAll = sqrt(stdNbCoverAll / N);
    stdNbAntecedantsAll = sqrt(stdNbAntecedantsAll / N);
    stdFidelGloAll = sqrt(stdFidelGloAll / N);
    stdAccGloAll = sqrt(stdAccGloAll / N);
    stdExplGloAll = sqrt(stdExplGloAll / N);
    stdDefaultRateAll = sqrt(stdDefaultRateAll / N);
    stdNbFidelActivationsAll = sqrt(stdNbFidelActivationsAll / N);
    stdWrongActivationsAll = sqrt(stdWrongActivationsAll / N);
    stdTestAccGloAll = sqrt(stdTestAccGloAll / N);
    stdTestAccWhenRulesAndModelAgreeAll = sqrt(stdTestAccWhenRulesAndModelAgreeAll / N);
    stdTestAccWhenActivatedRulesAndModelAgreeAll = sqrt(stdTestAccWhenActivatedRulesAndModelAgreeAll / N);
  }

  // Show and save results

  outputStatsFile.open(statFile, std::ios::app);
  if (outputStatsFile.is_open()) {

    outputStatsFile << "Results for " << N << " times " << K << "-Cross validation :\n\n";
    cout << "\n---------------------------------------------------------" << endl;
    cout << "---------------------------------------------------------" << endl
         << endl;
    cout << "Results for " << N << " times " << K << "-Cross validation :" << endl
         << endl;
    if (isFidex) {
      outputStatsFile << "Fidex :\n";
      outputStatsFile << "The mean covering size per rule is : " << meanCovSizeFidAll << "\n";
      outputStatsFile << "The standard deviation of the covering size per rule is : " << stdCovSizeFidAll << "\n";
      outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntFidAll << "\n";
      outputStatsFile << "The standard deviation of the number of antecedents per rule is : " << stdNbAntFidAll << "\n";
      outputStatsFile << "The mean rule fidelity rate is : " << meanFidelFidAll << "\n";
      outputStatsFile << "The standard deviation of the rule fidelity rate is : " << stdFidelFidAll << "\n";
      outputStatsFile << "The mean rule accuracy is : " << meanAccFidAll << "\n";
      outputStatsFile << "The standard deviation of the rule accuracy is : " << stdAccFidAll << "\n";
      outputStatsFile << "The mean rule confidence is : " << meanConfidFidAll << "\n";
      outputStatsFile << "The standard deviation of the rule confidence is : " << stdConfidFidAll << "\n";
      cout << "Fidex :" << endl;
      cout << "The mean covering size per rule is : " << meanCovSizeFidAll << endl;
      cout << "The standard deviation of the covering size per rule is : " << stdCovSizeFidAll << endl;
      cout << "The mean number of antecedents per rule is : " << meanNbAntFidAll << endl;
      cout << "The standard deviation of the number of antecedents per rule is : " << stdNbAntFidAll << endl;
      cout << "The mean rule fidelity rate is : " << meanFidelFidAll << endl;
      cout << "The standard deviation of the rule fidelity rate is : " << stdFidelFidAll << endl;
      cout << "The mean rule accuracy is : " << meanAccFidAll << endl;
      cout << "The standard deviation of the rule accuracy is : " << stdAccFidAll << endl;
      cout << "The mean rule confidence is : " << meanConfidFidAll << endl;
      cout << "The standard deviation of the rule confidence is : " << stdConfidFidAll << endl;
      cout << "\n---------------------------------------------------------" << endl;
      cout << "---------------------------------------------------------" << endl;
    }
    if (isFidex && isFidexGlo) {
      cout << endl;
      outputStatsFile << "\n---------------------------------------------------------\n\n";
    }
    if (isFidexGlo) {
      outputStatsFile << "FidexGlo :\n";
      outputStatsFile << "The mean number of rules is : " << meanNbRulesAll << "\n";
      outputStatsFile << "The standard deviation of the number of rules is : " << stdNbRulesAll << "\n";
      outputStatsFile << "The mean sample covering number per rule is : " << meanNbCoverAll << "\n";
      outputStatsFile << "The standard deviation of the sample covering number per rule is : " << stdNbCoverAll << "\n";
      outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntecedantsAll << "\n";
      outputStatsFile << "The standard deviation of the number of antecedents per rule is : " << stdNbAntecedantsAll << "\n";
      outputStatsFile << "The mean global rule fidelity rate is : " << meanFidelGloAll << "\n";
      outputStatsFile << "The standard deviation of the global rule fidelity rate is : " << stdFidelGloAll << "\n";
      outputStatsFile << "The mean global rule accuracy is : " << meanAccGloAll << "\n";
      outputStatsFile << "The standard deviation of the global rule accuracy is : " << stdAccGloAll << "\n";
      outputStatsFile << "The mean explainability rate (when we can find a rule) is : " << meanExplGloAll << "\n";
      outputStatsFile << "The standard deviation of the explainability rate (when we can find a rule) is : " << stdExplGloAll << "\n";
      outputStatsFile << "The mean default rule rate (when we can't find a rule) is : " << meanDefaultRateAll << "\n";
      outputStatsFile << "The standard deviation of the default rule rate (when we can't find a rule) is : " << stdDefaultRateAll << "\n";
      outputStatsFile << "The mean number of correct(fidel) activated rules per sample is : " << meanNbFidelActivationsAll << "\n";
      outputStatsFile << "The standard deviation of the number of correct(fidel) activated rules per sample is : " << stdNbFidelActivationsAll << "\n";
      outputStatsFile << "The mean number of wrong(not fidel) activated rules per sample is : " << meanWrongActivationsAll << "\n";
      outputStatsFile << "The standard deviation of the number of wrong(not fidel) activated rules per sample is : " << stdWrongActivationsAll << "\n";
      outputStatsFile << "The mean model test accuracy is : " << meanTestAccGloAll << "\n";
      outputStatsFile << "The standard deviation of the model test accuracy is : " << stdTestAccGloAll << "\n";
      outputStatsFile << "The mean model test accuracy when rules and model agree is : " << meanTestAccWhenRulesAndModelAgreeAll << "\n";
      outputStatsFile << "The standard deviation of the model test accuracy when rules and model agree is : " << stdTestAccWhenRulesAndModelAgreeAll << "\n";
      outputStatsFile << "The mean model test accuracy when activated rules and model agree is : " << meanTestAccWhenActivatedRulesAndModelAgreeAll << "\n";
      outputStatsFile << "The standard deviation of the model test accuracy when activated rules and model agree is : " << stdTestAccWhenActivatedRulesAndModelAgreeAll << "\n";
      cout << "FidexGlo :" << endl;
      cout << "The mean number of rules is : " << meanNbRulesAll << endl;
      cout << "The standard deviation of the number of rules is : " << stdNbRulesAll << endl;
      cout << "The mean sample covering number per rule is : " << meanNbCoverAll << endl;
      cout << "The standard deviation of the sample covering number per rule is : " << stdNbCoverAll << endl;
      cout << "The mean number of antecedents per rule is : " << meanNbAntecedantsAll << endl;
      cout << "The standard deviation of the number of antecedents per rule is : " << stdNbAntecedantsAll << endl;
      cout << "The mean global rule fidelity rate is : " << meanFidelGloAll << endl;
      cout << "The standard deviation of the global rule fidelity rate is : " << stdFidelGloAll << endl;
      cout << "The mean global rule accuracy is : " << meanAccGloAll << endl;
      cout << "The standard deviation of the global rule accuracy is : " << stdAccGloAll << endl;
      cout << "The mean explainability rate (when we can find a rule) is : " << meanExplGloAll << endl;
      cout << "The standard deviation of the explainability rate (when we can find a rule) is : " << stdExplGloAll << endl;
      cout << "The mean default rule rate (when we can't find a rule) is : " << meanDefaultRateAll << endl;
      cout << "The standard deviation of the default rule rate (when we can't find a rule) is : " << stdDefaultRateAll << endl;
      cout << "The mean number of correct(fidel) activated rules per sample is : " << meanNbFidelActivationsAll << endl;
      cout << "The standard deviation of the number of correct(fidel) activated rules per sample is : " << stdNbFidelActivationsAll << endl;
      cout << "The mean number of wrong(not fidel) activated rules per sample is : " << meanWrongActivationsAll << endl;
      cout << "The standard deviation of the number of wrong(not fidel) activated rules per sample is : " << stdWrongActivationsAll << endl;
      cout << "The mean model test accuracy is : " << meanTestAccGloAll << endl;
      cout << "The standard deviation of the model test accuracy is : " << stdTestAccGloAll << endl;
      cout << "The mean model test accuracy when rules and model agree is : " << meanTestAccWhenRulesAndModelAgreeAll << endl;
      cout << "The standard deviation of the model test accuracy when rules and model agree is : " << stdTestAccWhenRulesAndModelAgreeAll << endl;
      cout << "The mean model test accuracy when activated rules and model agree is : " << meanTestAccWhenActivatedRulesAndModelAgreeAll << endl;
      cout << "The standard deviation of the model test accuracy when activated rules and model agree is : " << stdTestAccWhenActivatedRulesAndModelAgreeAll << endl;
      cout << "\n---------------------------------------------------------" << endl;
      cout << "---------------------------------------------------------" << endl;
    }
    outputStatsFile.close();
  } else {
    throw CannotOpenFileError("Error : Couldn't open stats extraction file " + std::string(statFile) + ".");
  }

  // Delete temporary files

  const char *deleteConsole = nullptr;
  string deleteConsoleTemp = root + "consoleTemp.txt";
  deleteConsole = &deleteConsoleTemp[0];
  remove(deleteConsole);
  const char *deleteTrain = nullptr;
  string deleteTrainTemp = root + "tempTrain.txt";
  deleteTrain = &deleteTrainTemp[0];
  remove(deleteTrain);
  const char *deleteTest = nullptr;
  string deleteTestTemp = root + "tempTest.txt";
  deleteTest = &deleteTestTemp[0];
  remove(deleteTest);
  const char *deleteValid = nullptr;
  string deleteValidTemp = root + "tempValid.txt";
  deleteValid = &deleteValidTemp[0];
  remove(deleteValid);
  const char *deleteTarTrain = nullptr;
  string deleteTarTrainTemp = root + "tempTarTrain.txt";
  deleteTarTrain = &deleteTarTrainTemp[0];
  remove(deleteTarTrain);
  const char *deleteTarTest = nullptr;
  string deleteTarTestTemp = root + "tempTarTest.txt";
  deleteTarTest = &deleteTarTestTemp[0];
  remove(deleteTarTest);
  const char *deleteTarValid = nullptr;
  string deleteTarValidTemp = root + "tempTarValid.txt";
  deleteTarValid = &deleteTarValidTemp[0];

  remove(deleteConsole);
  remove(deleteTrain);
  remove(deleteTest);
  remove(deleteValid);
  remove(deleteTarTrain);
  remove(deleteTarTest);
  remove(deleteTarValid);

  t2 = clock();
  temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
  std::cout << "\nFull execution time = " << temps << " sec\n";
}

// ./CrossValid -L datanorm -1 dataclass2 -K 3 -N 2 -I 16 -H2 5 -O 2 -F CrossValidation -S dimlp/datafiles -h 5 -v 25 -x 0.5 -y 0.5 -C both
