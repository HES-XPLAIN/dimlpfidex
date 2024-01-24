#include "DimlpRulFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpRul()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpRul --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--weights_file <file of weights> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--valid_data_file <validation set file>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--valid_class_file <file of validation classes>" << std::endl;
  cout << "--global_rules_outfile <rulesFile (dimlp.rls by default)>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--stats_file <output file with train, test and validation accuracy>" << std::endl;
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

enum ParameterDimlpRulEnum {
  TRAIN_DATA_FILE,
  WEIGHTS_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  ROOT_FOLDER,
  ATTRIBUTES_FILE,
  VALID_DATA_FILE,
  TEST_DATA_FILE,
  TRAIN_CLASS_FILE,
  TEST_CLASS_FILE,
  VALID_CLASS_FILE,
  GLOBAL_RULES_OUTFILE,
  CONSOLE_FILE,
  STATS_FILE,
  H,
  NB_QUANT_LEVELS,
  INVALID
};

const std::unordered_map<std::string, ParameterDimlpRulEnum> parameterMap = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"root_folder", ROOT_FOLDER},
    {"attributes_file", ATTRIBUTES_FILE},
    {"valid_data_file", VALID_DATA_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"valid_class_file", VALID_CLASS_FILE},
    {"global_rules_outfile", GLOBAL_RULES_OUTFILE},
    {"console_file", CONSOLE_FILE},
    {"stats_file", STATS_FILE},
    {"H", H},
    {"nb_quant_levels", NB_QUANT_LEVELS}};

int dimlpRul(const string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

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

    int quant = 50;
    int nbIn = 0;
    int nbOut = 0;

    string learnFileTemp;
    bool learnFileInit = false;
    string testFileTemp;
    bool testFileInit = false;
    string validFileTemp;
    bool validFileInit = false;
    string weightFileTemp;
    bool weightFileInit = false;
    string learnTarTemp;
    bool learnTarInit = false;
    string testTarTemp;
    bool testTarInit = false;
    string validTarTemp;
    bool validTarInit = false;
    string rulesFileTemp = "dimlp.rls";
    string consoleFileTemp;
    bool consoleFileInit = false;
    string accuracyFileTemp;
    bool accuracyFileInit = false;
    string attrFileTemp;
    bool attrFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;

    StringInt arch;
    StringInt archInd;

    if (nbParam <= 1) {
      GiveAllParamDimlpRul();
      return 0;
    }

    int p = 1; // We skip "DimlpRul"
    while (p < nbParam) {
      string param = commandList[p];

      if (param.substr(0, 2) == "--") {
        param = param.substr(2);
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }
        const char *arg = commandList[p].c_str();
        string stringArg = arg;

        ParameterDimlpRulEnum option;
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

        switch (option) { // After --
        case NB_QUANT_LEVELS:
          if (CheckInt(arg))
            quant = atoi(arg);
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

        case ROOT_FOLDER:
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case ATTRIBUTES_FILE:
          attrFileTemp = arg;
          attrFileInit = true;
          break;

        case WEIGHTS_FILE:
          weightFileTemp = arg;
          weightFileInit = true;
          break;

        case TRAIN_DATA_FILE:
          learnFileTemp = arg;
          learnFileInit = true;
          break;

        case TEST_DATA_FILE:
          testFileTemp = arg;
          testFileInit = true;
          break;

        case VALID_DATA_FILE:
          validFileTemp = arg;
          validFileInit = true;
          break;

        case TRAIN_CLASS_FILE:
          learnTarTemp = arg;
          learnTarInit = true;
          break;

        case TEST_CLASS_FILE:
          testTarTemp = arg;
          testTarInit = true;
          break;

        case VALID_CLASS_FILE:
          validTarTemp = arg;
          validTarInit = true;
          break;

        case GLOBAL_RULES_OUTFILE:
          rulesFileTemp = arg;
          break;

        case CONSOLE_FILE:
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case STATS_FILE:
          accuracyFileTemp = arg;
          accuracyFileInit = true;
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
    const char *learnTar = nullptr;
    const char *testTar = nullptr;
    const char *validTar = nullptr;
    const char *rulesFile = nullptr;
    const char *consoleFile = nullptr;
    const char *accuracyFile = nullptr;
    const char *attrFile = nullptr;

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

    if (weightFileInit) {
      weightFileTemp = root + weightFileTemp;
      weightFile = &weightFileTemp[0];
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

    rulesFileTemp = root + rulesFileTemp;
    rulesFile = &rulesFileTemp[0];

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }

    if (accuracyFileInit) {
      accuracyFileTemp = root + accuracyFileTemp;
      accuracyFile = &accuracyFileTemp[0];
    }

    if (attrFileInit) {
      attrFileTemp = root + attrFileTemp;
      attrFile = &attrFileTemp[0];
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    if (weightFileInit == false) {
      throw CommandArgumentException("Give a file of weights with --weights_file selection please.");
    }

    if (quant <= 2) {
      throw CommandArgumentException("The number of quantized levels must be greater than 2.");
    }

    if (nbIn == 0) {
      throw CommandArgumentException("The number of input neurons must be given with option --nb_attributes.");
    }

    if (nbOut <= 1) {
      throw CommandArgumentException("At least two output neurons must be given with option --nb_classes.");
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

    // ----------------------------------------------------------------------

    auto net = std::make_shared<Dimlp>(weightFile, nbLayers, vecNbNeurons, quant);

    float accTrain;
    float errTrain;
    float accValid;
    float errValid;
    float accTest;
    float errTest;

    errTrain = net->Error(Train, TrainClass, &accTrain);

    cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << errTrain;
    cout << "\n\n*** ACCURACY ON TRAINING SET = " << accTrain << "" << std::endl;

    if (Valid.GetNbEx() > 0) {
      errValid = net->Error(Valid, ValidClass, &accValid);

      cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << errValid;
      cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "" << std::endl;
    }

    if (Test.GetNbEx() > 0) {
      errTest = net->Error(Test, TestClass, &accTest);

      cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << errTest;
      cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
        accFile << "Sum squared error on training set = " << errTrain << "" << std::endl;
        accFile << "Accuracy on training set = " << accTrain << "\n"
                << std::endl;
        if (Valid.GetNbEx() > 0) {
          accFile << "Sum squared error on validation set = " << errValid << "" << std::endl;
          accFile << "Accuracy on validation set = " << accValid << "\n"
                  << std::endl;
        }
        if (Test.GetNbEx() > 0) {
          accFile << "Sum squared error on testing set = " << errTest << "" << std::endl;
          accFile << "Accuracy on testing set = " << accTest << "\n"
                  << std::endl;
        }
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : could not open accuracy file " + std::string(accuracyFile));
      }
    }

    cout << "\n-------------------------------------------------\n"
         << std::endl;

    // ----------------------------------------------------------------------

    All = Train;

    cout << "Extraction Part :: " << std::endl;

    if (Valid.GetNbEx() > 0) {
      DataSet all2(All, Valid);
      All = all2;
    }

    cout << "\n\n****************************************************\n"
         << std::endl;
    cout << "*** RULE EXTRACTION" << std::endl;

    if (attrFileInit != false) {
      AttrName attr(attrFile, nbIn, nbOut);

      if (attr.ReadAttr())
        cout << "\n\n"
             << attrFile << ": Read file of attributes.\n"
             << std::endl;

      Attr = attr;
    }

    RealHyp ryp(All, net, quant, nbIn,
                vecNbNeurons[1] / nbIn, nbWeightLayers);

    filebuf buf;

    if (buf.open(rulesFile, ios_base::out) == nullptr) {
      throw CannotOpenFileError("Error : Cannot open rules file " + std::string(rulesFile));
    }

    ostream rulesFileost(&buf);

    ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                       Test, TestClass, Attr, rulesFileost);

    if (ryp.TreeAborted()) {
      RealHyp2 ryp2(All, net, quant, nbIn,
                    vecNbNeurons[1] / nbIn, nbWeightLayers);

      ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                          Test, TestClass, Attr, rulesFileost);
    }

    cout << "\n\n"
         << rulesFile << ": "
         << "Written.\n"
         << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();
  } catch (const char *msg) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    cerr << msg << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : dimlp.dimlpRul("DimlpRul --train_data_file datanormTrain --train_class_file dataclass2Train --weights_file dimlpDatanorm.wts --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --H2 5 --nb_classes 2 --global_rules_outfile dimlpDatanormRul.rls --stats_file dimlpDatanormRulStats --console_file dimlpDatanormRulResult.txt --root_folder dimlp/datafiles")
