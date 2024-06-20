#include "dimlpRulFct.h"

////////////////////////////////////////////////////////////

/**
 * @brief Displays the parameters for dimlpRul.
 */
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

  printOptionDescription("--train_data_file <str>", "Path to the file containing the train portion of the dataset");
  printOptionDescription("--train_class_file <str>", "Path to the file containing the train true classes of the dataset, not mandatory if classes are specified in train data file");
  printOptionDescription("--weights_file <str>", "Path to the file containing the weights of the model trained with dimlpTrn");
  printOptionDescription("--hidden_layers_file <str>", "Path to the file containing hidden layers sizes");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in the dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in the dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder");
  printOptionDescription("--test_data_file <str>", "Path to the file containing the test portion of the dataset");
  printOptionDescription("--test_class_file <str>", "Path to the file containing the test true classes of the dataset");
  printOptionDescription("--valid_data_file <str>", "Path to the file containing the validation portion of the dataset");
  printOptionDescription("--valid_class_file <str>", "Path to the file containing the validation true classes of the dataset");
  printOptionDescription("--global_rules_outfile <str>", "Path to the file where the output rule(s) will be stored (default: dimlp.rls)");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes");
  printOptionDescription("--stats_file <str>", "Path to the file where the train, test and validation accuracy will be stored");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in the staircase activation function (default: 50)");
  printOptionDescription("--normalization_file <str>", "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]-inf,inf[>>", "Mean or median of each attribute index to be denormalized in the rules");
  printOptionDescription("--sigmas <list<float ]-inf,inf[>>", "Standard deviation of each attribute index to be denormalized in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to be denormalized in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");

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
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of dimlpRul.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
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

/**
 * @brief Executes the Dimlp rule extraction process with specified parameters to obtain explaining rules and statistics for train, test and validation datasets for a model trained with dimlpTrn.
 *
 * The function performs the following steps:
 * 1. Parses the command string to extract parameters.
 * 2. Sets up the neural network and other necessary objects.
 * 3. Loads the weights from the provided weights file.
 * 4. Computes the error and accuracy on the training, validation, and test datasets if provided.
 * 5. Extracts rules from the trained network.
 * 6. Saves the rules and the total execution time.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the training dataset and weights file.
 * - The hidden layers configuration file must also be provided to specify the network architecture.
 * - True train class labels must be provided, either within the data file or separately through a class file. Test and validation classes are given the same way if present.
 * - Normalization parameters can be specified to denormalize the Dimlp explaining rules if data were normalized beforehand.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - global_rules_outfile: File containing the extracted explaining rules with statistics.
 * - stats_file: If specified, contains accuracy statistics for training, validation, and test datasets if provided.
 * - console_file: If specified, contains the console output.
 *
 * File formats:
 * - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Only attributes (floats).
 *   2. Attributes (floats) followed by an integer class ID.
 *   3. Attributes (floats) followed by one-hot encoded class.
 * - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Integer class ID.
 *   2. One-hot encoded class.
 * - **Weights file**: This file should be obtained by training with DimlpTrn and not with DimlpBT(!).
 *   The first row represents bias values of the Dimlp layer and the second row are values of the weight matrix between the previous layer and the Dimlp layer.
 *   Each value is separated by a space. As an example, if the layers are of size 4, the biases are: b1 b2 b3 b4 and the weights are w1 w2 w3 w4.
 * - **Hidden layers file**: This file contains the number of nodes in each hidden layer. In each line there is the layer id and the number of its nodes separated by a space. Ex :<br>
 *   1 16<br>
 *   2 5
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
 * <tt>dimlp.dimlpRul('-\-train_data_file datanormTrain.txt -\-train_class_file dataclass2Train.txt -\-weights_file dimlpDatanorm.wts -\-test_data_file datanormTest.txt -\-test_class_file dataclass2Test.txt -\-nb_attributes 16 -\-hidden_layers_file hidden_layers.out -\-nb_classes 2 -\-global_rules_outfile globalRules.rls -\-stats_file stats.txt -\-root_folder dimlp/datafiles')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, rule extraction parameters, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */
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
