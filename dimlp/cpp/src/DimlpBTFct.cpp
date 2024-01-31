#include "DimlpBTFct.h"
const int BPNN = 1;

using namespace std;
////////////////////////////////////////////////////////////

void GiveAllParamDimlpBT()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpBT --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";

  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--nb_dimlp_nets <number of networks (25 by default)>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--weights_generic_filename <output weights generic name file(without .wts, dimlpBT by default)>" << std::endl;
  cout << "--train_pred_file <output train prediction file (dimlpBT.out by default)>" << std::endl;
  cout << "--test_pred_file <output test prediction file (dimlpBTTest.out by default)>" << std::endl;
  cout << "--stats_file <output file with train, test and validation accuracy and with the global accuracy for train and test>" << std::endl;
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--with_rule_extraction (RULE EXTRACTION)" << std::endl;
  cout << "--global_rules_outfile <extraction ruleFile>" << std::endl; // If we want to extract rules in a rulesFile instead of console
  cout << "--learning_rate <back-propagation learning parameter (Eta, 0.1 by default)>" << std::endl;
  cout << "--momentum <back-propagation momentum parameter (Mu, 0.6 by default)>" << std::endl;
  cout << "--flat <back-propagation flat spot elimination parameter (Flat, 0.01 by default)>" << std::endl;
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  cout << "--error_thresh <error threshold (-1111111111 by default)>" << std::endl;
  cout << "--acc_thresh <accuracy threshold (11111111111111 by default)>" << std::endl;
  cout << "--abs_error_thresh <absolute difference error threshold>" << std::endl;
  cout << "--nb_epochs <number of epochs (0 by default)>" << std::endl;
  cout << "--nb_epochs_error <number of epochs to show error (1500 by default)>" << std::endl;
  cout << "--nb_ex_per_net <number of examples for one single network (10 by default)>" << std::endl;
  cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_stats is given)>" << std::endl;
  cout << "--seed <seed (0=random, default)>";

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

enum ParameterDimlpBTEnum {
  TRAIN_DATA_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  ROOT_FOLDER,
  NB_DIMLP_NETS,
  ATTRIBUTES_FILE,
  TEST_DATA_FILE,
  TRAIN_CLASS_FILE,
  TEST_CLASS_FILE,
  CONSOLE_FILE,
  WEIGHTS_GENERIC_FILENAME,
  TRAIN_PRED_FILE,
  TEST_PRED_FILE,
  STATS_FILE,
  H,
  WITH_RULE_EXTRACTION,
  GLOBAL_RULES_OUTFILE,
  LEARNING_RATE,
  MOMENTUM,
  FLAT,
  NB_QUANT_LEVELS,
  ERROR_THRESH,
  ACC_THRESH,
  ABS_ERROR_THRESH,
  NB_EPOCHS,
  NB_EPOCHS_ERROR,
  NB_EX_PER_NET,
  NORMALIZATION_FILE,
  MUS,
  SIGMAS,
  NORMALIZATION_INDICES,
  SEED,
  INVALID
};

const std::unordered_map<std::string, ParameterDimlpBTEnum> parameterMap = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"root_folder", ROOT_FOLDER},
    {"nb_dimlp_nets", NB_DIMLP_NETS},
    {"attributes_file", ATTRIBUTES_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"console_file", CONSOLE_FILE},
    {"weights_generic_filename", WEIGHTS_GENERIC_FILENAME},
    {"train_pred_file", TRAIN_PRED_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"stats_file", STATS_FILE},
    {"H", H},
    {"with_rule_extraction", WITH_RULE_EXTRACTION},
    {"global_rules_outfile", GLOBAL_RULES_OUTFILE},
    {"learning_rate", LEARNING_RATE},
    {"momentum", MOMENTUM},
    {"flat", FLAT},
    {"nb_quant_levels", NB_QUANT_LEVELS},
    {"error_thresh", ERROR_THRESH},
    {"acc_thresh", ACC_THRESH},
    {"abs_error_thresh", ABS_ERROR_THRESH},
    {"nb_epochs", NB_EPOCHS},
    {"nb_epochs_error", NB_EPOCHS_ERROR},
    {"nb_ex_per_net", NB_EX_PER_NET},
    {"normalization_file", NORMALIZATION_FILE},
    {"mus", MUS},
    {"sigmas", SIGMAS},
    {"normalization_indices", NORMALIZATION_INDICES},
    {"seed", SEED}};

int dimlpBT(const string &command) {
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

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

    float eta = 0.1f;
    float mu = 0.6f;
    float flat = 0.01f;
    float errThres = -1111111111.0f;
    float accThres = 11111111111111.0f;
    float deltaErr = 0;
    int showErr = 10;
    int epochs = 1500;
    int quant = 50;
    int nbDimlpNets = 25;

    int ruleExtr = 0;

    int nbIn = 0;
    int nbOut = 0;
    int nbExInOne = 0;
    int seed = 0;

    string learnFileTemp;
    bool learnFileInit = false;
    string testFileTemp;
    bool testFileInit = false;
    string validFileTemp;
    bool validFileInit = false;
    string weightFileTemp = "dimlp.wts";
    string genericWeightsFileTemp = "dimlpBT";
    string predTrainFileTemp = "dimlpBT.out";
    string predTestFileTemp = "dimlpBTTest.out";
    string rulesFileTemp;
    bool rulesFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string accuracyFileTemp;
    bool accuracyFileInit = false;
    string learnTarTemp;
    bool learnTarInit = false;
    string testTarTemp;
    bool testTarInit = false;
    string validTarTemp;
    bool validTarInit = false;
    string attrFileTemp;
    bool attrFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;

    string normalizationFileTemp;
    bool normalizationFileInit = false;
    std::vector<double> mus;
    bool hasMus = false;
    std::vector<double> sigmas;
    bool hasSigmas = false;
    std::vector<int> normalizationIndices;
    bool hasNormalizationIndices = false;

    int flagEp = 0;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;

    StringInt arch;
    StringInt archInd;

    if (nbParam <= 1) {
      GiveAllParamDimlpBT();
      return 0;
    }

    std::vector<ParameterDimlpBTEnum> paramsWithoutValues;
    paramsWithoutValues.push_back(WITH_RULE_EXTRACTION);
    bool isParamWithoutValue = false;

    int p = 1; // We skip "DimlpBT"
    while (p < nbParam) {
      isParamWithoutValue = false;
      string param = commandList[p];

      if (param.substr(0, 2) == "--") {
        param = param.substr(2);
        p++;

        ParameterDimlpBTEnum option;
        auto it = parameterMap.find(param);
        if (it != parameterMap.end()) {
          option = it->second;
        } else {
          if (param[0] == 'H') {
            std::string numberPart = param.substr(1);
            if (CheckInt(numberPart.c_str())) {
              option = H;
            } else {
              option = INVALID;
            }
          } else {
            option = INVALID;
          }
        }

        if (std::find(paramsWithoutValues.begin(), paramsWithoutValues.end(), option) != paramsWithoutValues.end()) {
          isParamWithoutValue = true;
        }

        if (p >= nbParam && !isParamWithoutValue) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        if (p + 1 < nbParam && commandList[p + 1].substr(0, 2) != "--" && !isParamWithoutValue) {
          throw CommandArgumentException("There is a parameter without -- (" + commandList[p + 1] + ").");
        }

        const char *arg;
        string stringArg;
        if (!isParamWithoutValue) {
          arg = commandList[p].c_str();
          stringArg = arg;
        }

        switch (option) { // After --
        case LEARNING_RATE:
          if (CheckFloat(arg))
            eta = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          break;

        case MOMENTUM:
          if (CheckFloat(arg))
            mu = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          break;

        case FLAT:
          if (CheckFloat(arg))
            flat = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          break;

        case ERROR_THRESH:
          if (CheckFloat(arg))
            errThres = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case ACC_THRESH:
          if (CheckFloat(arg))
            accThres = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case ABS_ERROR_THRESH:
          if (CheckFloat(arg))
            deltaErr = static_cast<float>(atof(arg));
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case NB_EPOCHS_ERROR:
          if (CheckInt(arg))
            showErr = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_EPOCHS:
          if (CheckInt(arg)) {
            epochs = atoi(arg);
            flagEp = 1;
          } else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_QUANT_LEVELS:
          if (CheckInt(arg))
            quant = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_EX_PER_NET:
          if (CheckInt(arg))
            nbExInOne = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_DIMLP_NETS:
          if (CheckInt(arg))
            nbDimlpNets = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_ATTRIBUTES:
          if (CheckInt(arg))
            nbIn = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case H:
          if (CheckInt(arg)) {
            arch.Insert(atoi(arg));

            const char *ptrParam = param.c_str();

            if (ptrParam[1] != '\0') {
              std::string str(ptrParam + 1);
              archInd.Insert(std::atoi(str.c_str()));
            } else {
              throw CommandArgumentException("Error : Which hidden layer (-H) ?");
            }
          } else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_CLASSES:
          if (CheckInt(arg))
            nbOut = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case SEED:
          if (CheckInt(arg))
            seed = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case ROOT_FOLDER:
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case ATTRIBUTES_FILE:
          attrFileTemp = arg;
          attrFileInit = true;
          break;

        case TRAIN_DATA_FILE:
          learnFileTemp = arg;
          learnFileInit = true;
          break;

        case TEST_DATA_FILE:
          testFileTemp = arg;
          testFileInit = true;
          break;

        case CONSOLE_FILE:
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case WEIGHTS_GENERIC_FILENAME:
          genericWeightsFileTemp = arg;
          break;

        case TRAIN_PRED_FILE:
          predTrainFileTemp = arg;
          break;

        case TEST_PRED_FILE:
          predTestFileTemp = arg;
          break;

        case STATS_FILE:
          accuracyFileTemp = arg;
          accuracyFileInit = true;
          break;

        case TRAIN_CLASS_FILE:
          learnTarTemp = arg;
          learnTarInit = true;
          break;

        case TEST_CLASS_FILE:
          testTarTemp = arg;
          testTarInit = true;
          break;

        case WITH_RULE_EXTRACTION:
          ruleExtr = 1;
          p--;
          break;

        case GLOBAL_RULES_OUTFILE:
          rulesFileTemp = arg;
          rulesFileInit = true;
          break;

        case NORMALIZATION_FILE:
          normalizationFileTemp = arg;
          normalizationFileInit = true;
          break;

        case MUS:
          if (!checkList(arg)) {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", list in the form [a,b,...,c] without spaces requested, a,b,c are numbers. Received " + string(arg) + ".");
          }
          mus = getDoubleVectorFromString(arg);
          hasMus = true;
          break;

        case SIGMAS:
          if (!checkList(arg)) {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", list in the form [a,b,...,c] without spaces requested, a,b,c are numbers. Received " + string(arg) + ".");
          }
          sigmas = getDoubleVectorFromString(arg);
          hasSigmas = true;
          break;

        case NORMALIZATION_INDICES:
          if (!checkList(arg)) {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", list in the form [a,b,...,c] without spaces requested, a,b,c are integers. Received " + string(arg) + ".");
          }
          normalizationIndices = getIntVectorFromString(arg);
          hasNormalizationIndices = true;
          break;

        default:
          throw CommandArgumentException("Illegal option : " + param + ".");
        }
      }

      else {
        throw CommandArgumentException("Illegal option : " + string(&(commandList[p])[0]));
      }
      p++;
    }

    // ----------------------------------------------------------------------

    // create paths with root foler

    const char *learnFile = nullptr;
    const char *testFile = nullptr;
    const char *validFile = nullptr;
    const char *weightFile = nullptr;
    const char *genericWeightsFile = nullptr;
    const char *predTrainFile = nullptr;
    const char *predTestFile = nullptr;
    const char *rulesFile = nullptr;
    const char *consoleFile = nullptr;
    const char *accuracyFile = nullptr;
    const char *learnTar = nullptr;
    const char *testTar = nullptr;
    const char *validTar = nullptr;
    const char *attrFile = nullptr;
    const char *normalizationFile = nullptr;

    string root = "";
    if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
      root = rootFolderTemp + "/";
#elif defined(_WIN32)
      root = rootFolderTemp + "\\";
#endif
    }

    if (learnFileInit) {
      learnFileTemp = root + learnFileTemp;
      learnFile = &learnFileTemp[0];
    }

    if (testFileInit) {
      testFileTemp = root + testFileTemp;
      testFile = &testFileTemp[0];
    }

    if (validFileInit) {
      validFileTemp = root + validFileTemp;
      validFile = &validFileTemp[0];
    }

    weightFileTemp = root + weightFileTemp;
    weightFile = &weightFileTemp[0];

    genericWeightsFileTemp = root + genericWeightsFileTemp;
    genericWeightsFile = &genericWeightsFileTemp[0];

    predTrainFileTemp = root + predTrainFileTemp;
    predTrainFile = &predTrainFileTemp[0];

    predTestFileTemp = root + predTestFileTemp;
    predTestFile = &predTestFileTemp[0];

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      rulesFile = &rulesFileTemp[0];
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }

    if (accuracyFileInit) {
      accuracyFileTemp = root + accuracyFileTemp;
      accuracyFile = &accuracyFileTemp[0];
    }

    if (learnTarInit) {
      learnTarTemp = root + learnTarTemp;
      learnTar = &learnTarTemp[0];
    }

    if (testTarInit) {
      testTarTemp = root + testTarTemp;
      testTar = &testTarTemp[0];
    }

    if (validTarInit) {
      validTarTemp = root + validTarTemp;
      validTar = &validTarTemp[0];
    }

    if (attrFileInit) {
      attrFileTemp = root + attrFileTemp;
      attrFile = &attrFileTemp[0];
    }

    if (normalizationFileInit) {
      normalizationFileTemp = root + normalizationFileTemp;
      normalizationFile = &normalizationFileTemp[0];
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    if (eta <= 0) {
      throw CommandArgumentException("The learning parameter must be greater than 0.");
    }

    if (mu < 0) {
      throw CommandArgumentException("The momentum parameter must be greater or equal to 0.");
    }

    if (showErr == 0) {
      throw CommandArgumentException("The number of epochs must be greater than 0.");
    }

    if (quant == 0) {
      throw CommandArgumentException("The number of quantized levels must be greater than 0.");
    }

    if (nbIn == 0) {
      throw CommandArgumentException("The number of input neurons must be given with option --nb_attributes.");
    }

    if (nbOut == 0) {
      throw CommandArgumentException("The number of output neurons must be given with option --nb_classes.");
    }

    // ----------------------------------------------------------------------

    // Check denormalization parameters

    // If normalizationIndices were not specified, it's all attributes
    if (!normalizationFileInit && !hasNormalizationIndices) {
      for (int i = 0; i < nbIn; ++i) {
        normalizationIndices.push_back(i);
      }
      hasNormalizationIndices = true;
    }

    // Check if mus and sigmas are both given or both not
    if ((hasMus || hasSigmas) &&
        !(hasMus && hasSigmas)) {
      throw CommandArgumentException("Error : One of Mus(--mus) and sigmas(--sigmas) is given but not the other.");
    }

    if (normalizationFileInit && hasMus || normalizationFileInit && hasNormalizationIndices) {
      throw CommandArgumentException("Error : normlization file (--normalization_file) and mus or normalizationIndices (--normalization_indices) are both given.");
    }

    // Mus, sigmas and normalizationIndices must have the same size and not be empty
    if (hasMus && (mus.size() != sigmas.size() || mus.size() != normalizationIndices.size() || mus.empty())) {
      throw CommandArgumentException("Error : mus (--mus), sigmas (--sigmas) and normalization indices (--normalization_indices) don't have the same size or are empty.");
    }

    // Check normalizationIndices
    std::set<int> uniqueIndices(normalizationIndices.begin(), normalizationIndices.end());
    if (uniqueIndices.size() != normalizationIndices.size() ||
        *std::max_element(uniqueIndices.begin(), uniqueIndices.end()) >= nbIn ||
        *std::min_element(uniqueIndices.begin(), uniqueIndices.end()) < 0) {
      throw CommandArgumentException("Error : parameter normalization indices (--normalization_indices) has negative, greater than the number of attributes or repeted elements.");
    }

    // ----------------------------------------------------------------------

    if (arch.GetNbEl() == 0) {
      nbLayers = 3;
      nbWeightLayers = nbLayers - 1;

      vecNbNeurons.assign(nbLayers, 0);
      vecNbNeurons[0] = nbIn;
      vecNbNeurons[1] = nbIn;
      vecNbNeurons[2] = nbOut;
    }

    else {
      archInd.GoToBeg();

      if (archInd.GetVal() == 1) {
        arch.GoToBeg();

        if (arch.GetVal() % nbIn != 0) {
          throw InternalError("The number of neurons in the first hidden layer must be a multiple of the number of input neurons.");
        }

        nbLayers = arch.GetNbEl() + 2;
        nbWeightLayers = nbLayers - 1;

        vecNbNeurons.assign(nbLayers, 0);
        vecNbNeurons[0] = nbIn;
        vecNbNeurons[nbLayers - 1] = nbOut;

        for (p = 1, arch.GoToBeg(); p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p] = arch.GetVal();

          if (vecNbNeurons[p] == 0) {
            throw InternalError("The number of neurons must be greater than 0.");
          }
        }
      }

      else {
        nbLayers = arch.GetNbEl() + 3;
        nbWeightLayers = nbLayers - 1;

        vecNbNeurons.assign(nbLayers, 0);
        vecNbNeurons[0] = nbIn;
        vecNbNeurons[1] = nbIn;
        vecNbNeurons[nbLayers - 1] = nbOut;

        for (p = 1, arch.GoToBeg(); p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p + 1] = arch.GetVal();

          if (vecNbNeurons[p + 1] == 0) {
            throw InternalError("The number of neurons must be greater than 0.");
          }
        }
      }
    }

    // ----------------------------------------------------------------------
    if (learnFileInit == false) {
      throw CommandArgumentException("Give the training file with --train_data_file selection please.");
    }
    if (learnTarInit != false) {
      DataSet train(learnFile, nbIn, nbOut);
      DataSet trainClass(learnTar, nbIn, nbOut);

      Train = train;
      TrainClass = trainClass;
    } else {
      DataSet data(learnFile, nbIn, nbOut);

      DataSet train(data.GetNbEx());
      DataSet trainClass(data.GetNbEx());

      data.ExtractDataAndTarget(train, nbIn, trainClass, nbOut);

      Train = train;
      TrainClass = trainClass;

      data.Del();
    }

    if (validFileInit != false) {
      if (validTarInit != false) {
        DataSet valid(validFile, nbIn, nbOut);
        DataSet validClass(validTar, nbIn, nbOut);

        Valid = valid;
        ValidClass = validClass;
      }

      else {
        DataSet data(validFile, nbIn, nbOut);

        DataSet valid(data.GetNbEx());
        DataSet validClass(data.GetNbEx());

        data.ExtractDataAndTarget(valid, nbIn, validClass, nbOut);

        Valid = valid;
        ValidClass = validClass;

        data.Del();
      }
    }

    if (testFileInit != false) {
      if (testTarInit != false) {
        DataSet test(testFile, nbIn, nbOut);
        DataSet testClass(testTar, nbIn, nbOut);

        Test = test;
        TestClass = testClass;
      }

      else {
        DataSet data(testFile, nbIn, nbOut);

        DataSet test(data.GetNbEx());
        DataSet testClass(data.GetNbEx());

        data.ExtractDataAndTarget(test, nbIn, testClass, nbOut);

        Test = test;
        TestClass = testClass;

        data.Del();
      }
    }
    auto net = std::make_shared<BagDimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                          quant, showErr, epochs, nbLayers, vecNbNeurons,
                                          nbDimlpNets, weightFile, seed);

    if (nbExInOne == 0)
      nbExInOne = Train.GetNbEx();
    else if (nbExInOne > Train.GetNbEx())
      nbExInOne = Train.GetNbEx();

    net->MakeDataSets(Train, TrainClass, nbExInOne);
    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
        accFile << "Accuracy for Bag training : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + std::string(accuracyFile));
      }
    }
    net->TrainAll(Test, TestClass, genericWeightsFile, accuracyFile, seed);
    float acc;
    float accTest;

    net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile);
    cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n"
         << std::endl;

    if (Test.GetNbEx() != 0) {
      net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile);
      cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile, ios::app);
      if (accFile.is_open()) {
        accFile << "-------------------------------------------------------" << std::endl;
        accFile << "-------------------------------------------------------\n"
                << std::endl;
        accFile << "Global accuracy on training set = " << acc << "" << std::endl;
        if (Test.GetNbEx() != 0) {
          accFile << "Global accuracy on testing set = " << accTest;
        }
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : could not open accuracy file " + std::string(accuracyFile));
      }
    }

    if (ruleExtr) {
      vector<string> attributeNames;
      if (attrFileInit != false) {
        AttrName attr(attrFile, nbIn, nbOut);

        if (attr.ReadAttr())
          cout << "\n\n"
               << attrFile << ": Read file of attributes.\n"
               << std::endl;

        Attr = attr;
        attributeNames = Attr.GetListAttr();
      }

      All = Train;
      if (rulesFileInit != false) {
        cout << "Extraction Part :: " << std::endl;
      }

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
      }

      // Get mus, sigmas and normalizationIndices from normalizationFile for denormalization :
      if (normalizationFileInit) {
        auto results = parseNormalizationStats(normalizationFile, nbIn, attributeNames);
        normalizationIndices = std::get<0>(results);
        mus = std::get<2>(results);
        sigmas = std::get<3>(results);
        hasMus = true;
      }

      cout << "\n\n****************************************************\n"
           << std::endl;
      cout << "*** RULE EXTRACTION" << std::endl;

      std::shared_ptr<VirtualHyp> globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                                 vecNbNeurons[1] / nbIn);

      RealHyp ryp(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                  All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                  nbWeightLayers);

      if (rulesFileInit != false) {
        filebuf buf;

        if (buf.open(rulesFile, ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + std::string(rulesFile));
        }

        ostream rulesFileost(&buf);
        ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                           Test, TestClass, Attr, rulesFileost);

        if (ryp.TreeAborted()) {

          std::shared_ptr<VirtualHyp> globVirt2 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt2, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost);
        }

        cout << "\n\n"
             << rulesFile << ": "
             << "Written.\n"
             << std::endl;
      } else {
        ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                           Test, TestClass, Attr, cout);

        if (ryp.TreeAborted()) {

          std::shared_ptr<VirtualHyp> globVirt3 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt3, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, cout);
        }
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again
    BpNN::resetInitRandomGen();

  } catch (const errorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : dimlpBT("DimlpBT --train_data_file datanormTrain --train_class_file dataclass2Train --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --H2 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_generic_filename dimlpDatanormBT --train_pred_file dimlpDatanormBTTrain.out --test_pred_file dimlpDatanormBTTest.out --stats_file dimlpDatanormBTStats --console_file dimlpDatanormBTResult.txt --root_folder dimlp/datafiles");
