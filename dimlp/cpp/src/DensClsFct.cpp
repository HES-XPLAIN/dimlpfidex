#include "DensClsFct.h"
using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDensCls()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DensCls --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--weights_generic_filename <weights generic name file> (for instance give DimlpBT) ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons> ";
  cout << "--nb_dimlp_nets <number of networks>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--train_pred_file <output train prediction file (densCls.out by default)>" << std::endl;
  cout << "--test_pred_file <output test prediction file (densClsTest.out by default)>" << std::endl;
  cout << "--stats_file <output file with global train and test accuracy>" << std::endl;
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--with_rule_extraction (RULE EXTRACTION)" << std::endl;
  cout << "--global_rules_outfile <extraction ruleFile>" << std::endl; // If we want to extract rules in a rulesFile instead of console
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

enum ParameterDensClsEnum {
  TRAIN_DATA_FILE,
  WEIGHTS_GENERIC_FILENAME,
  NB_ATTRIBUTES,
  NB_CLASSES,
  NB_DIMLP_NETS,
  ROOT_FOLDER,
  ATTRIBUTES_FILE,
  TEST_DATA_FILE,
  TRAIN_CLASS_FILE,
  TEST_CLASS_FILE,
  CONSOLE_FILE,
  TRAIN_PRED_FILE,
  TEST_PRED_FILE,
  STATS_FILE,
  H,
  WITH_RULE_EXTRACTION,
  GLOBAL_RULES_OUTFILE,
  NB_QUANT_LEVELS,
  INVALID
};

const std::unordered_map<std::string, ParameterDensClsEnum> parameterMap = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"weights_generic_filename", WEIGHTS_GENERIC_FILENAME},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"nb_dimlp_nets", NB_DIMLP_NETS},
    {"root_folder", ROOT_FOLDER},
    {"attributes_file", ATTRIBUTES_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"console_file", CONSOLE_FILE},
    {"train_pred_file", TRAIN_PRED_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"stats_file", STATS_FILE},
    {"H", H},
    {"with_rule_extraction", WITH_RULE_EXTRACTION},
    {"global_rules_outfile", GLOBAL_RULES_OUTFILE},
    {"nb_quant_levels", NB_QUANT_LEVELS}};

int densCls(const string &command) {

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

    int nbDimlpNets = 0;

    int ruleExtr = 0;

    int nbIn = 0;
    int nbOut = 0;
    int quant = 50;

    string learnFileTemp;
    bool learnFileInit = false;
    string testFileTemp;
    bool testFileInit = false;
    string validFileTemp;
    bool validFileInit = false;
    string weightFileTemp;
    bool weightFileInit = false;
    string weightFileSaveTemp = "dimlp.wts";
    string predTrainFileTemp = "densCls.out";
    string predTestFileTemp = "densClsTest.out";
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

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;

    StringInt arch;
    StringInt archInd;

    if (nbParam <= 1) {
      GiveAllParamDensCls();
      return 0;
    }

    int p = 1; // We skip "DensCls"
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

        ParameterDensClsEnum option;
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

        case WEIGHTS_GENERIC_FILENAME:
          weightFileTemp = arg;
          weightFileInit = true;
          break;

        case NB_QUANT_LEVELS:
          if (CheckInt(arg))
            quant = atoi(arg);
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

        default:
          throw CommandArgumentException("Illegal option : " + param);
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
    const char *predTrainFile = nullptr;
    const char *predTestFile = nullptr;
    const char *rulesFile = nullptr;
    const char *consoleFile = nullptr;
    const char *accuracyFile = nullptr;
    const char *learnTar = nullptr;
    const char *testTar = nullptr;
    const char *validTar = nullptr;
    const char *attrFile = nullptr;
    const char *weightFileSave = nullptr;

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

    weightFileSaveTemp = root + weightFileSaveTemp;
    weightFileSave = &weightFileSaveTemp[0];

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    if (quant == 0) {
      throw CommandArgumentException("The number of quantized levels must be greater than 0.");
    }

    if (nbIn == 0) {
      throw CommandArgumentException("The number of input neurons must be given with option --nb_attributes.");
    }

    if (nbOut == 0) {
      throw CommandArgumentException("The number of output neurons must be given with option --nb_classes.");
    }

    if (weightFileInit == false) {
      throw CommandArgumentException("Give a file of weights with --weights_generic_filename selection please.");
    }

    if (nbDimlpNets == 0) {
      throw CommandArgumentException("Give the number of networks with --weights_generic_filename selection please.");
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

    auto net = std::make_shared<BagDimlp>(quant, nbLayers, vecNbNeurons, nbDimlpNets, weightFileSave);

    net->DefNetsWithWeights(weightFile);

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
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
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
      if (attrFileInit != false) {
        AttrName attr(attrFile, nbIn, nbOut);

        if (attr.ReadAttr())
          cout << "\n\n"
               << attrFile << ": Read file of attributes.\n"
               << std::endl;

        Attr = attr;
      }

      All = Train;

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
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

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : densCls("DensCls --train_data_file datanormTrain --train_class_file dataclass2Train --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --H2 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_generic_filename dimlpDatanormBT --with_rule_extraction --global_rules_outfile dimlpDatanormDensClsRul.rls --train_pred_file dimlpDatanormDensClsTrain.out --test_pred_file dimlpDatanormDensClsTest.out --stats_file dimlpDatanormDensClsStats --console_file dimlpDatanormDensClsResult.txt --root_folder dimlp/datafiles");
