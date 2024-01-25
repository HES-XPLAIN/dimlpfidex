#include "DimlpTrnFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpTrn()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpTrn --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--valid_data_file <validation set file>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--pretrained_weights_file <file of pretrained weights>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--valid_class_file <file of validation classes>" << std::endl;
  cout << "--weights_file <output weight file (weights.wts by default)>" << std::endl;
  cout << "--train_pred_file <output train prediction file (dimlp.out by default)>" << std::endl;
  cout << "--test_pred_file <output test prediction file (dimlpTest.out by default)>" << std::endl;
  cout << "--valid_pred_file <output validation prediction file (dimlpValidation.out by default)>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--stats_file <output file with train, test and validation accuracy>" << std::endl;
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
  cout << "--abs_error_thresh <absolute difference error threshold (0 by default)>" << std::endl;
  cout << "--nb_epochs <number of epochs (1500 by default)>" << std::endl;
  cout << "--nb_epochs_error <number of epochs to show error (10 by default)>" << std::endl;
  cout << "--seed <seed (0=random, default)>";

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

static void SaveOutputs(
    DataSet &data,
    std::shared_ptr<Dimlp> net,
    int nbOut,
    int nbWeightLayers,
    const char *outfile)

{
  filebuf buf;

  if (buf.open(outfile, ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + std::string(outfile));
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  cout << "\n\n"
       << outfile << ": "
       << "Writing ..." << std::endl;

  ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "" << std::endl;
  }

  cout << outfile << ": "
       << "Written.\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

enum ParameterDimlpTrnEnum {
  TRAIN_DATA_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  ROOT_FOLDER,
  ATTRIBUTES_FILE,
  VALID_DATA_FILE,
  TEST_DATA_FILE,
  PRETRAINED_WEIGHTS_FILE,
  TRAIN_CLASS_FILE,
  TEST_CLASS_FILE,
  VALID_CLASS_FILE,
  WEIGHTS_FILE,
  TRAIN_PRED_FILE,
  TEST_PRED_FILE,
  VALID_PRED_FILE,
  CONSOLE_FILE,
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
  SEED,
  INVALID
};

const std::unordered_map<std::string, ParameterDimlpTrnEnum> parameterMap = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"root_folder", ROOT_FOLDER},
    {"attributes_file", ATTRIBUTES_FILE},
    {"valid_data_file", VALID_DATA_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"pretrained_weights_file", PRETRAINED_WEIGHTS_FILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"valid_class_file", VALID_CLASS_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"train_pred_file", TRAIN_PRED_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"valid_pred_file", VALID_PRED_FILE},
    {"console_file", CONSOLE_FILE},
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
    {"seed", SEED},
};

int dimlpTrn(const string &command) {
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

    std::shared_ptr<Dimlp> net;

    float eta = 0.1f;
    float mu = 0.6f;
    float flat = 0.01f;
    float errThres = -1111111111.0f;
    float accThres = 11111111111111.0f;
    float deltaErr = 0;
    int showErr = 10;
    int epochs = 1500;
    int quant = 50;

    int ruleExtr = 0;

    int nbIn = 0;
    int nbOut = 0;
    int seed = 0;

    string learnFileTemp;
    bool learnFileInit = false;
    string testFileTemp;
    bool testFileInit = false;
    string validFileTemp;
    bool validFileInit = false;
    string weightFileTemp;
    bool weightFileInit = false;
    string outputWeightFileTemp = "dimlp.wts";
    string predTrainFileTemp = "dimlp.out";
    string predTestFileTemp = "dimlpTest.out";
    string predValidationFileTemp = "dimlpValidation.out";
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

    int flagEp = 0;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;

    StringInt arch;
    StringInt archInd;

    if (nbParam <= 1) {
      GiveAllParamDimlpTrn();
      return 0;
    }

    int p = 1; // We skip "DimlpTrn"
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

        ParameterDimlpTrnEnum option;
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
        case LEARNING_RATE:
          if (CheckFloat(arg)) {
            eta = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          break;

        case MOMENTUM:
          if (CheckFloat(arg)) {
            mu = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          break;

        case FLAT:
          if (CheckFloat(arg)) {
            flat = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          break;

        case ERROR_THRESH:
          if (CheckFloat(arg)) {
            errThres = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case ACC_THRESH:
          if (CheckFloat(arg)) {
            accThres = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case ABS_ERROR_THRESH:
          if (CheckFloat(arg)) {
            deltaErr = static_cast<float>(atof(arg));
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", float requested");
          }

          if (flagEp == 0)
            epochs = 2000000000;
          break;

        case NB_EPOCHS_ERROR:
          if (CheckInt(arg)) {
            showErr = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case NB_EPOCHS:
          if (CheckInt(arg)) {
            epochs = atoi(arg);
            flagEp = 1;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case NB_QUANT_LEVELS:
          if (CheckInt(arg)) {
            quant = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case NB_ATTRIBUTES:
          if (CheckInt(arg)) {
            nbIn = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

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
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case NB_CLASSES:
          if (CheckInt(arg)) {
            nbOut = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case SEED:
          if (CheckInt(arg)) {
            seed = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");
          }

          break;

        case ROOT_FOLDER:
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case ATTRIBUTES_FILE:
          attrFileTemp = arg;
          attrFileInit = true;
          break;

        case PRETRAINED_WEIGHTS_FILE:
          weightFileTemp = arg;
          weightFileInit = true;
          break;

        case WEIGHTS_FILE:
          outputWeightFileTemp = arg;
          break;

        case TRAIN_PRED_FILE:
          predTrainFileTemp = arg;
          break;

        case TEST_PRED_FILE:
          predTestFileTemp = arg;
          break;

        case VALID_PRED_FILE:
          predValidationFileTemp = arg;
          break;

        case CONSOLE_FILE:
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case STATS_FILE:
          accuracyFileTemp = arg;
          accuracyFileInit = true;
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

        case WITH_RULE_EXTRACTION:
          ruleExtr = 1;
          p--;
          break;

        case GLOBAL_RULES_OUTFILE:
          rulesFileTemp = arg;
          rulesFileInit = true;
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
    const char *outputWeightFile = nullptr;
    const char *predTrainFile = nullptr;
    const char *predTestFile = nullptr;
    const char *predValidationFile = nullptr;
    const char *rulesFile = nullptr;
    const char *consoleFile = nullptr;
    const char *accuracyFile = nullptr;
    const char *learnTar = nullptr;
    const char *testTar = nullptr;
    const char *validTar = nullptr;
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

    outputWeightFileTemp = root + outputWeightFileTemp;
    outputWeightFile = &outputWeightFileTemp[0];

    predTrainFileTemp = root + predTrainFileTemp;
    predTrainFile = &predTrainFileTemp[0];

    predTestFileTemp = root + predTestFileTemp;
    predTestFile = &predTestFileTemp[0];

    predValidationFileTemp = root + predValidationFileTemp;
    predValidationFile = &predValidationFileTemp[0];

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

    if (weightFileInit == false)
      net = std::make_shared<Dimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                    quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile, seed);

    else
      net = std::make_shared<Dimlp>(weightFile, eta, mu, flat, errThres, accThres,
                                    deltaErr, quant, showErr, epochs,
                                    nbLayers, vecNbNeurons, outputWeightFile, seed);

    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
        accFile << "Accuracy : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + std::string(accuracyFile));
      }
    }

    net->Train(Train, TrainClass, Test, TestClass, Valid, ValidClass, accuracyFile);

    SaveOutputs(Train, net, nbOut, nbWeightLayers, predTrainFile); // Get train predictions
    if (testFileInit != false) {
      SaveOutputs(Test, net, nbOut, nbWeightLayers, predTestFile); // Get test predictions
    }
    if (validFileInit != false) {
      SaveOutputs(Valid, net, nbOut, nbWeightLayers, predValidationFile); // Get validation predictions
    }

    if (ruleExtr) {

      if (attrFileInit != false) {
        AttrName attr(attrFile, nbIn, nbOut);

        if (attr.ReadAttr())
          cout << "\n\n"
               << attrFile << ": Read file of attributes.\n"
               << std::endl;

        Attr = attr;
      }

      All = Train;
      if (rulesFileInit != false) {
        cout << "Extraction Part :: " << std::endl;
      }

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
      }

      cout << "\n\n****************************************************\n"
           << endl;
      cout << "*** RULE EXTRACTION" << std::endl;
      RealHyp ryp1(All, net, quant, nbIn,
                   vecNbNeurons[1] / nbIn, nbWeightLayers);
      if (rulesFileInit != false) {
        filebuf buf;

        if (buf.open(rulesFile, ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + std::string(rulesFile));
        }
        ostream rulesFileost(&buf);
        ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost);

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

          ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost);

        } else

          cout << "\n\n"
               << rulesFile << ": "
               << "Written.\n"
               << std::endl;
      }

      else {
        ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, cout);

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

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

/* Exemples to launch the code :

./DimlpTrn --train_data_file irisTrainData.txt --train_class_file irisTrainClass.txt --test_data_file irisTestData.txt --test_class_file irisTestClass.txt --weights_file weights.wts --nb_attributes 4 --H2 5 --nb_classes 3 --train_pred_file predtrain.out --test_pred_file predTest.out --with_rule_extraction --global_rules_outfile rules.rls --stats_file stats --console_file results.txt --root_folder ../dimlp/datafiles/IrisDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file spamTrainData.txt --train_class_file spamTrainClass.txt --test_data_file spamTestData.txt --test_class_file spamTestClass.txt --weights_file spam.wts --nb_attributes 57 --H2 5 --nb_classes 2 --train_pred_file spamTrainPred.out --test_pred_file spamTestPred.out --with_rule_extraction --global_rules_outfile spamTrn.rls --stats_file spamTrnStats --console_file spamTrnResult.txt --root_folder ../dimlp/datafiles/spamDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file isoletTrainData.txt --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --test_class_file isoletTestClass.txt --weights_file isoletV3.wts --nb_attributes 617 --H2 5 --nb_classes 26 --train_pred_file isoletTrainPredV3.out --test_pred_file isoletTestPredV3.out --with_rule_extraction --global_rules_outfile isoletTrnV3.rls --stats_file isoletTrnStatsV3 --console_file isoletTrnResultV3.txt --root_folder ../dimlp/datafiles/isoletDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file Train/X_train.txt --train_class_file Train/y_train.txt --test_data_file test/X_test.txt --test_class_file Test/y_test.txt --weights_file HAPT.wts --nb_attributes 561 --H2 5 --nb_classes 12 --train_pred_file Train/pred_train.out --test_pred_file Test/pred_test.out --with_rule_extraction --global_rules_outfile HAPTTrain.rls --stats_file HAPTTrnStats --console_file HAPTTrnResult.txt --root_folder ../dimlp/datafiles/HAPTDataset --attributes_file attributes.txt

*/
