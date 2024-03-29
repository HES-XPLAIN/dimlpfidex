#include "dimlpBTFct.h"
const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDimlpBTParams()

{
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are localised with respect to root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--train_data_file <str>", "Train data file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of input neurons");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of output neurons");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--nb_dimlp_nets <int [1,inf[>", "Number of networks (default: 25)");
  printOptionDescription("--attributes_file <str>", "File of attributes");
  printOptionDescription("--test_data_file <str>", "Test data file");
  printOptionDescription("--train_class_file <str>", "Train true class file");
  printOptionDescription("--test_class_file <str>", "Test true class file");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--weights_outfilename <str>", "Output weights file name file (default: dimlpBT.wts");
  printOptionDescription("--train_pred_outfile <str>", "Output train prediction file name (default: dimlpBTTrain.out)");
  printOptionDescription("--test_pred_outfile <str>", "Output test prediction file name (default: dimlpBTTest.out)");
  printOptionDescription("--stats_file <str>", "Output file name with train, test and validation accuracy and with the global train and test accuracy");
  printOptionDescription("--first_hidden_layer <int k*nb_attributes, k in [1,inf[>", "Number of neurons in the first hidden layer (default: nb_attributes)");
  printOptionDescription("--hidden_layers <list<int [1,inf[>>", "Number of neurons in each hidden layer, from the second layer through to the last");
  printOptionDescription("--hidden_layers_outfile <str>", "Output hidden layers file name (default: hidden_layers.out)");
  printOptionDescription("--with_rule_extraction <bool>", "Whether to extract rules with dimlpBT algorithm");
  printOptionDescription("--global_rules_outfile <str>", "Rules output file");
  printOptionDescription("--momentum <float [0,inf[>", "Back-propagation momentum parameter (default: 0.6)");
  printOptionDescription("--flat <float [0,inf[>", "Back-propagation flat spot elimination parameter (default: 0.01)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
  printOptionDescription("--error_thresh <float [0,inf[>", "Error threshold to stop training");
  printOptionDescription("--acc_thresh <float ]0,1]>", "Accuracy threshold to stop training");
  printOptionDescription("--abs_error_thresh <float [0,inf[>", "Absolute difference error threshold, 0 if not using this stopping criteria (default: 0)");
  printOptionDescription("--nb_epochs <int [1,inf[>", "Number of epochs (default: 1500)");
  printOptionDescription("--nb_epochs_error <int [1,inf[>", "Number of epochs to show error (default: 10)");
  printOptionDescription("--nb_ex_per_net <int [1,inf[>", "Number of examples for one single network, 0 for all examples, it is not recommended to change this value (default: 0)");
  printOptionDescription("--normalization_file <str>", "File containing the mean and std of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]inf,inf[>>", "Mean or median of each attribute index to denormalize in the rules");
  printOptionDescription("--sigmas <list<float ]inf,inf[>>", "Standard deviation of each attribute index to denormalize in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");
  printOptionDescription("--seed <int [0,inf[>", "Seed, 0=random (default: 0)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "dimlp.dimlpBT(\"--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_outfile dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
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
void checkDimlpBTParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultString(TRAIN_PRED_OUTFILE, "dimlpBTTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpBTTest.out", true);
  p.setDefaultString(WEIGHTS_OUTFILE, "dimlpBT.wts", true);
  p.setDefaultNbQuantLevels();
  p.setDefaultDimlpTrn();
  p.setDefaultInt(NB_DIMLP_NETS, 25);
  p.setDefaultInt(NB_EX_PER_NET, 0);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersDimlpTrn();
  p.checkParametersNormalization();

  if (p.getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of dimlp nets must be strictly positive (>=1).");
  }

  if (p.getInt(NB_EX_PER_NET) < 0) {
    throw CommandArgumentException("Error : Number of examples per net must be positive (>=0, 0=all examples).");
  }
}

int dimlpBT(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {
    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"dimlpBT"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpBTParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, NB_DIMLP_NETS, ATTRIBUTES_FILE,
                                              TEST_DATA_FILE, TRAIN_CLASS_FILE, TEST_CLASS_FILE, CONSOLE_FILE, WEIGHTS_OUTFILE,
                                              TRAIN_PRED_OUTFILE, TEST_PRED_OUTFILE, STATS_FILE, FIRST_HIDDEN_LAYER, HIDDEN_LAYERS, HIDDEN_LAYERS_OUTFILE, WITH_RULE_EXTRACTION, GLOBAL_RULES_OUTFILE,
                                              LEARNING_RATE, MOMENTUM, FLAT, NB_QUANT_LEVELS, ERROR_THRESH, ACC_THRESH, ABS_ERROR_THRESH,
                                              NB_EPOCHS, NB_EPOCHS_ERROR, NB_EX_PER_NET, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES, SEED};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }

      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("JSON config file name/path is invalid");
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkDimlpBTParametersLogicValues(*params);

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
    std::string predTrainFile = params->getString(TRAIN_PRED_OUTFILE);
    std::string predTestFile = params->getString(TEST_PRED_OUTFILE);
    std::string genericWeightsFile = params->getString(WEIGHTS_OUTFILE);

    float eta = params->getFloat(LEARNING_RATE);
    float mu = params->getFloat(MOMENTUM);
    float flat = params->getFloat(FLAT);
    float errThres = params->getFloat(ERROR_THRESH);
    float accThres = params->getFloat(ACC_THRESH);
    float deltaErr = params->getFloat(ABS_ERROR_THRESH);
    int showErr = params->getInt(NB_EPOCHS_ERROR);
    int epochs = params->getInt(NB_EPOCHS);
    int quant = params->getInt(NB_QUANT_LEVELS);
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbExInOne = params->getInt(NB_EX_PER_NET);
    int seed = params->getInt(SEED);

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch = params->getArch();
    StringInt archInd = params->getArchInd();

    std::string weightFile = "dimlp.wts";
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
    auto net = std::make_shared<BagDimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                          quant, showErr, epochs, nbLayers, vecNbNeurons,
                                          nbDimlpNets, weightFile, seed);

    if (nbExInOne == 0)
      nbExInOne = Train.GetNbEx();
    else if (nbExInOne > Train.GetNbEx())
      nbExInOne = Train.GetNbEx();

    net->MakeDataSets(Train, TrainClass, nbExInOne);
    if (params->isStringSet(STATS_FILE)) {
      std::ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
        accFile << "Accuracy for Bag training : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + params->getString(STATS_FILE));
      }
    }
    net->TrainAll(Test, TestClass, genericWeightsFile, params->getString(STATS_FILE), seed);
    float acc;
    float accTest;

    net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile);
    std::cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n"
              << std::endl;

    if (Test.GetNbEx() != 0) {
      net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile);
      std::cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (params->isStringSet(STATS_FILE)) {
      std::ofstream accFile(params->getString(STATS_FILE), std::ios::app);
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
        throw CannotOpenFileError("Error : could not open accuracy file " + params->getString(STATS_FILE));
      }
    }

    if (params->getBool(WITH_RULE_EXTRACTION)) {
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

      All = Train;
      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        std::cout << "Extraction Part :: " << std::endl;
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

      std::cout << "\n\n****************************************************\n"
                << std::endl;
      std::cout << "*** RULE EXTRACTION" << std::endl;

      std::shared_ptr<VirtualHyp> globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                                 vecNbNeurons[1] / nbIn);

      RealHyp ryp(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                  All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                  nbWeightLayers);

      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        std::filebuf buf;

        if (buf.open(params->getString(GLOBAL_RULES_OUTFILE), std::ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + params->getString(GLOBAL_RULES_OUTFILE));
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

          std::shared_ptr<VirtualHyp> globVirt2 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt2, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost);
          }
        }

        std::cout << "\n\n"
                  << params->getString(GLOBAL_RULES_OUTFILE) << ": "
                  << "Written.\n"
                  << std::endl;
      } else {
        if (params->isDoubleVectorSet(MUS)) {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, std::cout, mus, sigmas, normalizationIndices);
        } else {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, std::cout);
        }

        if (ryp.TreeAborted()) {

          std::shared_ptr<VirtualHyp> globVirt3 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt3, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, std::cout, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, std::cout);
          }
        }
      }
    }

    // Save hidden layers sizes
    params->writeHiddenLayersFile();

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

// Exemple to launch the code : dimlp.dimlpBT("dimlpBT --train_data_file datanormTrain --train_class_file dataclass2Train --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_outfilename dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile dimlpDatanormBTRul.rls --train_pred_outfile dimlpDatanormBTTrain.out --test_pred_outfile dimlpDatanormBTTest.out --stats_file dimlpDatanormBTStats --console_file dimlpDatanormBTResult.txt --root_folder dimlp/datafiles");
