#include "dimlpTrnFct.h"

const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDimlpTrnParams()

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
  printOptionDescription("--attributes_file <str>", "File of attributes");
  printOptionDescription("--valid_data_file <str>", "Validation data file");
  printOptionDescription("--test_data_file <str>", "Test data file");
  printOptionDescription("--weights_file <str>", "Pretrained weights file");
  printOptionDescription("--train_class_file <str>", "Train true class file");
  printOptionDescription("--test_class_file <str>", "Test true class file");
  printOptionDescription("--valid_class_file <str>", "Validation true class file");
  printOptionDescription("--weights_outfile <str>", "Output weights file name (default: dimlp.wts)");
  printOptionDescription("--train_pred_outfile <str>", "Output train prediction file name (default: dimlpTrain.out)");
  printOptionDescription("--test_pred_outfile <str>", "Output test prediction file name (default: dimlpTest.out)");
  printOptionDescription("--valid_pred_outfile <str>", "Output validation prediction file name (default: dimlpValidation.out)");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--stats_file <str>", "Output file name with train, test and validation accuracy");
  printOptionDescription("--first_hidden_layer <int k*nb_attributes, k in [1,inf[>", "Number of neurons in the first hidden layer (default: nb_attributes)");
  printOptionDescription("--hidden_layers <list<int [1,inf[>>", "Number of neurons in each hidden layer, from the second layer through to the last");
  printOptionDescription("--hidden_layers_outfile <str>", "Output hidden layers file name (default: hidden_layers.out)");
  printOptionDescription("--with_rule_extraction <bool>", "Whether to extract rules with dimlp algorithm");
  printOptionDescription("--global_rules_outfile <str>", "Rules output file");
  printOptionDescription("--learning_rate <float ]0,inf[>", "Back-propagation learning parameter (default: 0.1)");
  printOptionDescription("--momentum <float [0,inf[>", "Back-propagation momentum parameter (default: 0.6)");
  printOptionDescription("--flat <float [0,inf[>", "Back-propagation flat spot elimination parameter (default: 0.01)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
  printOptionDescription("--error_thresh <float [0,inf[>", "Error threshold to stop training");
  printOptionDescription("--acc_thresh <float ]0,1]>", "Accuracy threshold to stop training");
  printOptionDescription("--abs_error_thresh <float [0,inf[>", "Absolute difference error threshold, 0 if not using this stopping criteria (default: 0)");
  printOptionDescription("--nb_epochs <int [1,inf[>", "Number of epochs (default: 1500)");
  printOptionDescription("--nb_epochs_error <int [1,inf[>", "Number of epochs to show error (default: 10)");
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
  std::cout << "dimlp.dimlpTrn(\"--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --weights_outfile dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

////////////////////////////////////////////////////////////

static void SaveOutputs(
    DataSet &data,
    std::shared_ptr<Dimlp> net,
    int nbOut,
    int nbWeightLayers,
    const std::string &outfile)

{
  std::filebuf buf;

  if (buf.open(outfile, std::ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  std::cout << "\n\n"
            << outfile << ": "
            << "Writing ..." << std::endl;

  std::ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "" << std::endl;
  }

  std::cout << outfile << ": "
            << "Written.\n"
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpTrnParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultString(TRAIN_PRED_OUTFILE, "dimlpTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpTest.out", true);
  p.setDefaultString(VALID_PRED_OUTFILE, "dimlpValidation.out", true);
  p.setDefaultString(WEIGHTS_OUTFILE, "dimlp.wts", true);
  p.setDefaultNbQuantLevels();
  p.setDefaultDimlpTrn();

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersDimlpTrn();
  p.checkParametersNormalization();
}

int dimlpTrn(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"dimlpTrn"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpTrnParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE, VALID_DATA_FILE,
                                              TEST_DATA_FILE, WEIGHTS_FILE, TRAIN_CLASS_FILE, TEST_CLASS_FILE, VALID_CLASS_FILE, WEIGHTS_OUTFILE,
                                              TRAIN_PRED_OUTFILE, TEST_PRED_OUTFILE, VALID_PRED_OUTFILE, CONSOLE_FILE, STATS_FILE, FIRST_HIDDEN_LAYER, HIDDEN_LAYERS, HIDDEN_LAYERS_OUTFILE, WITH_RULE_EXTRACTION,
                                              GLOBAL_RULES_OUTFILE, LEARNING_RATE, MOMENTUM, FLAT, NB_QUANT_LEVELS, ERROR_THRESH, ACC_THRESH,
                                              ABS_ERROR_THRESH, NB_EPOCHS, NB_EPOCHS_ERROR, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES, SEED};
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
    checkDimlpTrnParametersLogicValues(*params);

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
    std::string predValidationFile = params->getString(VALID_PRED_OUTFILE);
    std::string outputWeightFile = params->getString(WEIGHTS_OUTFILE);

    float eta = params->getFloat(LEARNING_RATE);
    float mu = params->getFloat(MOMENTUM);
    float flat = params->getFloat(FLAT);
    float errThres = params->getFloat(ERROR_THRESH);
    float accThres = params->getFloat(ACC_THRESH);
    float deltaErr = params->getFloat(ABS_ERROR_THRESH);
    int showErr = params->getInt(NB_EPOCHS_ERROR);
    int epochs = params->getInt(NB_EPOCHS);
    int quant = params->getInt(NB_QUANT_LEVELS);
    int seed = params->getInt(SEED);

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

    std::shared_ptr<Dimlp> net;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch = params->getArch();
    StringInt archInd = params->getArchInd();

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

    if (!params->isStringSet(WEIGHTS_FILE))
      net = std::make_shared<Dimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                    quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile, seed);

    else
      net = std::make_shared<Dimlp>(params->getString(WEIGHTS_FILE), eta, mu, flat, errThres, accThres,
                                    deltaErr, quant, showErr, epochs,
                                    nbLayers, vecNbNeurons, outputWeightFile, seed);

    if (params->isStringSet(STATS_FILE)) {
      std::ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
        accFile << "Accuracy : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + params->getString(STATS_FILE));
      }
    }

    net->Train(Train, TrainClass, Test, TestClass, Valid, ValidClass, params->getString(STATS_FILE));

    SaveOutputs(Train, net, nbOut, nbWeightLayers, predTrainFile); // Get train predictions
    if (params->isStringSet(TEST_DATA_FILE)) {
      SaveOutputs(Test, net, nbOut, nbWeightLayers, predTestFile); // Get test predictions
    }
    if (params->isStringSet(VALID_DATA_FILE)) {
      SaveOutputs(Valid, net, nbOut, nbWeightLayers, predValidationFile); // Get validation predictions
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

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
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
      RealHyp ryp1(All, net, quant, nbIn,
                   vecNbNeurons[1] / nbIn, nbWeightLayers);
      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        std::filebuf buf;

        if (buf.open(params->getString(GLOBAL_RULES_OUTFILE), std::ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + params->getString(GLOBAL_RULES_OUTFILE));
        }
        std::ostream rulesFileost(&buf);
        if (params->isDoubleVectorSet(MUS)) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost);
          }

        } else

          std::cout << "\n\n"
                    << params->getString(GLOBAL_RULES_OUTFILE) << ": "
                    << "Written.\n"
                    << std::endl;
      }

      else {
        if (params->isDoubleVectorSet(MUS)) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, std::cout, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, std::cout);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

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

/* Exemples to launch the code :

./dimlpTrn --train_data_file irisTrainData.txt --train_class_file irisTrainClass.txt --test_data_file irisTestData.txt --test_class_file irisTestClass.txt --weights_outfile weights.wts --nb_attributes 4 --hidden_layers 5 --nb_classes 3 --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --with_rule_extraction true --global_rules_outfile rules.rls --stats_file stats --console_file results.txt --root_folder ../dimlp/datafiles/IrisDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file spamTrainData.txt --train_class_file spamTrainClass.txt --test_data_file spamTestData.txt --test_class_file spamTestClass.txt --weights_outfile spam.wts --nb_attributes 57 --hidden_layers 5 --nb_classes 2 --train_pred_outfile spamTrainPred.out --test_pred_outfile spamTestPred.out --with_rule_extraction true --global_rules_outfile spamTrn.rls --stats_file spamTrnStats --console_file spamTrnResult.txt --root_folder ../dimlp/datafiles/spamDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file isoletTrainData.txt --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --test_class_file isoletTestClass.txt --weights_outfile isoletV3.wts --nb_attributes 617 --hidden_layers 5 --nb_classes 26 --train_pred_outfile isoletTrainPredV3.out --test_pred_outfile isoletTestPredV3.out --with_rule_extraction true --global_rules_outfile isoletTrnV3.rls --stats_file isoletTrnStatsV3 --console_file isoletTrnResultV3.txt --root_folder ../dimlp/datafiles/isoletDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file Train/X_train.txt --train_class_file Train/y_train.txt --test_data_file test/X_test.txt --test_class_file Test/y_test.txt --weights_outfile HAPT.wts --nb_attributes 561 --hidden_layers 5 --nb_classes 12 --train_pred_outfile Train/pred_train.out --test_pred_outfile Test/pred_test.out --with_rule_extraction true --global_rules_outfile HAPTTrain.rls --stats_file HAPTTrnStats --console_file HAPTTrnResult.txt --root_folder ../dimlp/datafiles/HAPTDataset --attributes_file attributes.txt

*/
