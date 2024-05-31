#include "dimlpRulFct.h"

const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDimlpRulParams()

{
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are located with respect to the root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--train_data_file <str>", "Train data file");
  printOptionDescription("--weights_file <str>", "Weights file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of input neurons");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of output neurons");
  printOptionDescription("--hidden_layers_file <str>", "Hidden layers file name");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--attributes_file <str>", "File of attributes");
  printOptionDescription("--valid_data_file <str>", "Validation data file");
  printOptionDescription("--test_data_file <str>", "Test data file");
  printOptionDescription("--train_class_file <str>", "Train true class file");
  printOptionDescription("--test_class_file <str>", "Test true class file");
  printOptionDescription("--valid_class_file <str>", "Validation true class file");
  printOptionDescription("--global_rules_outfile <str>", "Rules output file (default: dimlp.rls)");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--stats_file <str>", "Output file name with train, test and validation accuracy");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
  printOptionDescription("--normalization_file <str>", "File containing the mean and std of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]inf,inf[>>", "Mean or median of each attribute index to denormalize in the rules");
  printOptionDescription("--sigmas <list<float ]inf,inf[>>", "Standard deviation of each attribute index to denormalize in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "dimlp.dimlpRul(\"--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --weights_file dimlpDatanorm.wts --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --global_rules_outfile globalRules.rls --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpRulParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultString(GLOBAL_RULES_OUTFILE, "dimlp.rls", true);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(WEIGHTS_FILE);
  p.assertStringExists(HIDDEN_LAYERS_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersNormalization();
}

int dimlpRul(const std::string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"dimlpRul"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpRulParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, WEIGHTS_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER,
                                              ATTRIBUTES_FILE, VALID_DATA_FILE, TEST_DATA_FILE, TRAIN_CLASS_FILE,
                                              TEST_CLASS_FILE, VALID_CLASS_FILE, GLOBAL_RULES_OUTFILE, CONSOLE_FILE,
                                              STATS_FILE, HIDDEN_LAYERS_FILE, NB_QUANT_LEVELS, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }
      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &e) {
        throw CommandArgumentException("Some value inside your JSON config file '" + commandList[2] + "' is out of range.\n(Probably due to a too large or too tiny numeric value).");
      } catch (const std::exception &e) {
        std::string msg(e.what());
        throw CommandArgumentException("Unknown JSON config file error: " + msg);
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkDimlpRulParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // ----------------------------------------------------------------------

    // Get parameters values

    int nbIn = params->getInt(NB_ATTRIBUTES);
    int nbOut = params->getInt(NB_CLASSES);
    std::string learnFile = params->getString(TRAIN_DATA_FILE);
    std::string weightFile = params->getString(WEIGHTS_FILE);
    std::string rulesFile = params->getString(GLOBAL_RULES_OUTFILE);
    int quant = params->getInt(NB_QUANT_LEVELS);

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch;
    StringInt archInd;
    params->readHiddenLayersFile(arch, archInd);

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

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

        arch.GoToBeg();
        for (int p = 1; p <= arch.GetNbEl(); p++, arch.GoToNext()) {
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

        arch.GoToBeg();
        for (int p = 1; p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p + 1] = arch.GetVal();

          if (vecNbNeurons[p + 1] == 0) {
            throw InternalError("The number of neurons must be greater than 0.");
          }
        }
      }
    }

    // ----------------------------------------------------------------------

    if (params->isStringSet(TRAIN_CLASS_FILE)) {
      DataSet train(learnFile, nbIn, nbOut);
      DataSet trainClass(params->getString(TRAIN_CLASS_FILE), nbIn, nbOut);

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

    if (params->isStringSet(VALID_DATA_FILE)) {
      if (params->isStringSet(VALID_CLASS_FILE)) {
        DataSet valid(params->getString(VALID_DATA_FILE), nbIn, nbOut);
        DataSet validClass(params->getString(VALID_CLASS_FILE), nbIn, nbOut);

        Valid = valid;
        ValidClass = validClass;
      }

      else {
        DataSet data(params->getString(VALID_DATA_FILE), nbIn, nbOut);

        DataSet valid(data.GetNbEx());
        DataSet validClass(data.GetNbEx());

        data.ExtractDataAndTarget(valid, nbIn, validClass, nbOut);

        Valid = valid;
        ValidClass = validClass;

        data.Del();
      }
    }

    if (params->isStringSet(TEST_DATA_FILE)) {
      if (params->isStringSet(TEST_CLASS_FILE)) {
        DataSet test(params->getString(TEST_DATA_FILE), nbIn, nbOut);
        DataSet testClass(params->getString(TEST_CLASS_FILE), nbIn, nbOut);

        Test = test;
        TestClass = testClass;
      }

      else {
        DataSet data(params->getString(TEST_DATA_FILE), nbIn, nbOut);

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

    std::cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << errTrain;
    std::cout << "\n\n*** ACCURACY ON TRAINING SET = " << accTrain << "" << std::endl;

    if (Valid.GetNbEx() > 0) {
      errValid = net->Error(Valid, ValidClass, &accValid);

      std::cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << errValid;
      std::cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "" << std::endl;
    }

    if (Test.GetNbEx() > 0) {
      errTest = net->Error(Test, TestClass, &accTest);

      std::cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << errTest;
      std::cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (params->isStringSet(STATS_FILE)) {
      std::ofstream accFile(params->getString(STATS_FILE));
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
        throw CannotOpenFileError("Error : could not open accuracy file " + params->getString(STATS_FILE));
      }
    }

    std::cout << "\n-------------------------------------------------\n"
              << std::endl;

    // ----------------------------------------------------------------------

    All = Train;

    std::cout << "Extraction Part :: " << std::endl;

    if (Valid.GetNbEx() > 0) {
      DataSet all2(All, Valid);
      All = all2;
    }

    std::cout << "\n\n****************************************************\n"
              << std::endl;
    std::cout << "*** RULE EXTRACTION" << std::endl;

    std::vector<std::string> attributeNames;

    if (params->isStringSet(ATTRIBUTES_FILE)) {
      AttrName attr(params->getString(ATTRIBUTES_FILE), nbIn, nbOut);

      if (attr.ReadAttr())
        std::cout << "\n\n"
                  << params->getString(ATTRIBUTES_FILE) << ": Read file of attributes.\n"
                  << std::endl;

      Attr = attr;
      attributeNames = Attr.GetListAttr();
    }

    std::vector<int> normalizationIndices;
    std::vector<double> mus;
    std::vector<double> sigmas;

    // Get mus, sigmas and normalizationIndices from normalizationFile for denormalization :
    if (params->isStringSet(NORMALIZATION_FILE)) {
      auto results = parseNormalizationStats(params->getString(NORMALIZATION_FILE), params->getInt(NB_ATTRIBUTES), attributeNames);
      normalizationIndices = std::get<0>(results);
      mus = std::get<2>(results);
      sigmas = std::get<3>(results);
      params->setIntVector(NORMALIZATION_INDICES, normalizationIndices);
      params->setDoubleVector(MUS, mus);
      params->setDoubleVector(SIGMAS, sigmas);
    }

    RealHyp ryp(All, net, quant, nbIn,
                vecNbNeurons[1] / nbIn, nbWeightLayers);

    std::filebuf buf;

    if (buf.open(rulesFile, std::ios_base::out) == nullptr) {
      throw CannotOpenFileError("Error : Cannot open rules file " + rulesFile);
    }

    std::ostream rulesFileost(&buf);

    if (params->isDoubleVectorSet(MUS)) {
      ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                         Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
    } else {
      ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                         Test, TestClass, Attr, rulesFileost);
    }

    if (ryp.TreeAborted()) {
      RealHyp2 ryp2(All, net, quant, nbIn,
                    vecNbNeurons[1] / nbIn, nbWeightLayers);

      if (params->isDoubleVectorSet(MUS)) {
        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
      } else {
        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost);
      }
    }

    std::cout << "\n\n"
              << rulesFile << ": "
              << "Written.\n"
              << std::endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();

    Train.Del();
    TrainClass.Del();

    if (Test.GetNbEx() > 0) {
      Test.Del();
      TestClass.Del();
    }

    if (Valid.GetNbEx() > 0) {
      Valid.Del();
      ValidClass.Del();
    }

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << std::endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : dimlp.dimlpRul("dimlpRul --train_data_file datanormTrain --train_class_file dataclass2Train --weights_file dimlpDatanorm.wts --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --global_rules_outfile dimlpDatanormRul.rls --stats_file dimlpDatanormRulStats --console_file dimlpDatanormRulResult.txt --root_folder dimlp/datafiles")
