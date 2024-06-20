#include "dimlpTrnFct.h"

////////////////////////////////////////////////////////////

/**
 * @brief Displays the parameters for dimlpTrn.
 */
void showDimlpTrnParams()

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

  printOptionDescription("--train_data_file <str>", "Path to the file containing the train portion of the dataset");
  printOptionDescription("--train_class_file <str>", "Path to the file containing the train true classes of the dataset, not mandatory if classes are specified in train data file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in the dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in the dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder");
  printOptionDescription("--train_pred_outfile <str>", "Path to the file where the train predictions will be stored (default: dimlpTrain.out)");
  printOptionDescription("--test_data_file <str>", "Path to the file containing the test portion of the dataset");
  printOptionDescription("--test_class_file <str>", "Path to the file containing the test true classes of the dataset");
  printOptionDescription("--test_pred_outfile <str>", "Path to the file where the test predictions will be stored (default: dimlpTest.out)");
  printOptionDescription("--valid_data_file <str>", "Path to the file containing the validation portion of the dataset");
  printOptionDescription("--valid_class_file <str>", "Path to the file containing the validation true classes of the dataset");
  printOptionDescription("--valid_pred_outfile <str>", "Path to the file where the validation predictions will be stored (default: dimlpValidation.out)");
  printOptionDescription("--weights_file <str>", "Path to the file containing pretrained weights");
  printOptionDescription("--weights_outfile <str>", "Path to the file where the output trained weights of the model will be stored (default: dimlp.wts)");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes");
  printOptionDescription("--stats_file <str>", "Path to the file where the train, test and validation accuracy will be stored (default: statsDimlpTrn.txt)");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--first_hidden_layer <int k*nb_attributes, k in [1,inf[>", "Number of neurons in the first hidden layer (default: nb_attributes)");
  printOptionDescription("--hidden_layers <list<int [1,inf[>>", "Number of neurons in each hidden layer, from the second layer to the last");
  printOptionDescription("--hidden_layers_outfile <str>", "Path to the file where output hidden layers sizes will be stored (default: hidden_layers.out)");
  printOptionDescription("--with_rule_extraction <bool>", "Whether to extract rules with dimlp algorithm (default: False)");
  printOptionDescription("--global_rules_outfile <str>", "Path to the file where the output rule(s) will be stored");
  printOptionDescription("--learning_rate <float ]0,inf[>", "Back-propagation learning parameter (default: 0.1)");
  printOptionDescription("--momentum <float [0,inf[>", "Back-propagation momentum parameter (default: 0.6)");
  printOptionDescription("--flat <float [0,inf[>", "Back-propagation flat spot elimination parameter (default: 0.01)");
  printOptionDescription("--error_thresh <float [0,inf[>", "Error threshold to stop training");
  printOptionDescription("--acc_thresh <float ]0,1]>", "Accuracy threshold to stop training");
  printOptionDescription("--abs_error_thresh <float [0,inf[>", "Absolute difference error threshold, 0 if not using this stopping criteria (default: 0)");
  printOptionDescription("--nb_epochs <int [1,inf[>", "Number of model training epochs (default: 1500)");
  printOptionDescription("--nb_epochs_error <int [1,inf[>", "Number of training epochs before showing error (default: 10)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in the staircase activation function (default: 50)");
  printOptionDescription("--normalization_file <str>", "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]-inf,inf[>>", "Mean or median of each attribute index to be denormalized in the rules");
  printOptionDescription("--sigmas <list<float ]-inf,inf[>>", "Standard deviation of each attribute index to be denormalized in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to be denormalized in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");
  printOptionDescription("--seed <int [0,inf[>", "Seed for random number generation, 0=random. Anything else than 0 is an arbitrary seed that can be reused to obtain the same randomly generated sequence and therefore getting same results (default: 0)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "dimlp.dimlpTrn(\"--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --weights_outfile dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpTrn.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
 */
void checkDimlpTrnParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultString(TRAIN_PRED_OUTFILE, "dimlpTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpTest.out", true);
  p.setDefaultString(VALID_PRED_OUTFILE, "dimlpValidation.out", true);
  p.setDefaultString(WEIGHTS_OUTFILE, "dimlp.wts", true);
  p.setDefaultString(STATS_FILE, "statsDimlpTrn.txt", true);
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

/**
 * @brief Executes the Dimlp training process with specified parameters and optionally performs rule extraction with the Dimlp algorithm.
 *
 * The function performs the following steps:
 * 1. Parses the command string to extract parameters.
 * 2. Sets up the neural network and other necessary objects.
 * 3. Trains the network with the provided training dataset.
 * 4. Saves the network's predictions and accuracies for the training, test, and validation datasets if specified.
 * 5. Optionally extracts rules from the trained network.
 * 6. Saves the configuration of hidden layers and the total execution time.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the train dataset.
 * - True train class labels must be provided, either within the data file or separately through a class file. Test and validation classes are given the same way if present.
 * - Validation data and classes can optionally be given in the same way to train with validation.
 * - Data should be normalized beforehand. You can use the normalization function to do so as it generates a normalization file that will allow you to easily denormalize the rules obtained by Fidex or Dimlp.
 * - Normalization parameters can be specified to denormalize the Dimlp explaining rules if data were normalized beforehand.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - weights_outfile : File containing the training weights of the model.
 * - train_pred_outfile : File containing the model's train predictions.
 * - test_pred_outfile : If specified, contains the model's test predictions.
 * - valid_pred_outfile : If specified, contains the model's validation predictions.
 * - stats_file : If specified, contains train accuracy and possibly test and validation accuracy.
 * - hidden_layers_outfile : File containing the number of nodes in each hidden layer.
 * - global_rules_outfile : If specified and if computing rules, contains the explanation rules with statistics.
 * - console_file : If specified, contains the console output.
 *
 * File formats:
 * - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Only attributes (floats).
 *   2. Attributes (floats) followed by an integer class ID.
 *   3. Attributes (floats) followed by one-hot encoded class.
 * - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Integer class ID.
 *   2. One-hot encoded class.
 * - **Weights file**: It can contain the weights of one or more networks (for DimlpBT). If there is more than one network, each network is separated by a "Network <id>" marker.
 *   For each network, the biases of the first layer are on the first line followed by the weights of the first layer on the second line.
 *   This pattern continues for each layer of the network. Please note that no verification is made on the format. For example, with 2 networks:<br>
 *   Network 1<br>
 *   0.1 0.2 0.3  # Biases of the first layer<br>
 *   0.4 0.5 0.6 0.7 0.8 0.9  # Weights of the first layer<br>
 *   0.15 0.25  # Biases of the second layer<br>
 *   0.35 0.45 0.55 0.65  # Weights of the second layer<br>
 *   Network 2<br>
 *   0.11 0.22 0.33  # Biases of first layer of the network 2<br>
 *   0.44 0.55 0.66 0.77 0.88 0.99  # Weights of first layer of the network 2<br>
 *   0.155 0.255  # Biases of second layer of the network 2<br>
 *   0.355 0.455 0.555 0.655 # Weights of second layer of the network 2
 * - **Attributes file**: Each line corresponds to one attribute, each attribute must be specified. Classes can be specified
 *   after the attributes but are not mandatory. Each attribute or class must be in one word without spaces (you can use _ to replace a space).
 *   The order is important as the first attribute/class name will represent the first attribute/class in the dataset.
 * - **Normalization file**: Each line contains the mean/median and standard deviation for an attribute.<br>
 *   Format: '2 : original mean: 0.8307, original std: 0.0425'<br>
 *   Attribute indices (index 2 here) can be replaced with attribute names, then an attribute file is required.
 *
 * Example of how to call the function:
 * @par
 * <tt>from dimlpfidex import dimlp</tt>
 * @par
 * <tt>dimlp.dimlpTrn('-\-train_data_file datanormTrain.txt -\-train_class_file dataclass2Train.txt -\-test_data_file datanormTest.txt -\-test_class_file dataclass2Test.txt -\-nb_attributes 16 -\-hidden_layers 5 -\-nb_classes 2 -\-weights_outfile dimlpDatanormBT.wts -\-with_rule_extraction true -\-global_rules_outfile globalRules.rls -\-train_pred_outfile predTrain.out -\-test_pred_outfile predTest.out -\-root_folder dimlp/datafiles')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, training parameters, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */
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
