#include "densClsFct.h"

////////////////////////////////////////////////////////////

/**
 * @brief Displays the parameters for densCls.
 */
void showDensClsParams()

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
  printOptionDescription("--weights_file <str>", "Path to the file containing the weights of the model trained with dimlpBT");
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
  printOptionDescription("--train_pred_outfile <str>", "Path to the file where the train predictions will be stored (default: densClsTrain.out)");
  printOptionDescription("--test_data_file <str>", "Path to the file containing the test portion of the dataset");
  printOptionDescription("--test_class_file <str>", "Path to the file containing the test true classes of the dataset.");
  printOptionDescription("--test_pred_outfile <str>", "Path to the file where the test predictions will be stored (default: densClsTest.out)");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes");
  printOptionDescription("--stats_file <str>", "Path to the file where the global train and test accuracy will be stored");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--with_rule_extraction <bool>", "Whether to extract rules with dimlpBT algorithm (default: False)");
  printOptionDescription("--global_rules_outfile <str>", "Path to the file where the output rule(s) will be stored");
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
  std::cout << "dimlp.densCls(\"--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --weights_file dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile testPred.out --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of densCls.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
 */
void checkDensClsParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultBool(WITH_RULE_EXTRACTION, false);
  p.setDefaultString(TRAIN_PRED_OUTFILE, "densClsTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "densClsTest.out", true);

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
 * @brief Executes the Dense Classification (densCls) process with specified parameters. This function obtains train and, optionally, test predictions and accuracy for a model trained with dimlpBT. It can also perform rule extraction with the Dimlp algorithm.
 *
 * The function performs the following steps:
 * 1. Parses the command string to extract parameters.
 * 2. Sets up the neural network and other necessary objects.
 * 3. Computes the global accuracy on the training and test datasets if provided.
 * 4. Saves the network's predictions and optionally extracts rules.
 * 5. Saves the total execution time.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the training dataset and weights file.
 * - The hidden layers configuration file must also be provided to specify the network architecture.
 * - True train class labels must be provided, either within the data file or separately through a class file. Test classes are given the same way if present.
 * - Normalization parameters can be specified to denormalize the dimlp explaining rules if data were normalized beforehand.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - train_pred_outfile: File containing the model's train predictions.
 * - test_pred_outfile: File containing the model's test predictions.
 * - stats_file: File containing global train accuracy and test accuracy.
 * - global_rules_outfile: If specified and if computing rules, contains the explanation rules with statistics.
 * - console_file: If specified, contains the console output.
 *
 * File formats:
 * - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Only attributes (floats).
 *   2. Attributes (floats) followed by an integer class ID.
 *   3. Attributes (floats) followed by one-hot encoded class.
 * - **Class files**: These files should contain one class sample per line, with integers separated by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Integer class ID.
 *   2. One-hot encoded class.
 * - **Weights file**: This file should be obtained by training with DimlpBT and not with DimlpTrn(!). Each network is separated by a "Network <id>" marker.
 *   For each network, the first row represents bias values of the Dimlp layer and the second row are values of the weight matrix between the previous layer and the Dimlp layer.
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
 * <tt>dimlp.densCls('-\-train_data_file datanormTrain.txt -\-train_class_file dataclass2Train.txt -\-test_data_file datanormTest.txt -\-test_class_file dataclass2Test.txt -\-nb_attributes 16 -\-hidden_layers_file hidden_layers.out -\-nb_classes 2 -\-weights_file dimlpDatanormBT.wts -\-with_rule_extraction true -\-global_rules_outfile globalRules.rls -\-train_pred_outfile predTrain.out -\-test_pred_outfile testPred.out -\-stats_file stats.txt -\-root_folder dimlp/datafiles')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */
int densCls(const std::string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"densCls"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDensClsParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;

    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, WEIGHTS_FILE, NB_ATTRIBUTES, NB_CLASSES,
                                              ROOT_FOLDER, ATTRIBUTES_FILE, TEST_DATA_FILE, TRAIN_CLASS_FILE, TEST_CLASS_FILE,
                                              CONSOLE_FILE, TRAIN_PRED_OUTFILE, TEST_PRED_OUTFILE, STATS_FILE, HIDDEN_LAYERS_FILE, WITH_RULE_EXTRACTION,
                                              GLOBAL_RULES_OUTFILE, NB_QUANT_LEVELS, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES};
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
    checkDensClsParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // ----------------------------------------------------------------------

    // Get parameters values
    std::string weightFileSave = "dimlp.wts";

    int nbIn = params->getInt(NB_ATTRIBUTES);
    int nbOut = params->getInt(NB_CLASSES);
    std::string learnFile = params->getString(TRAIN_DATA_FILE);
    std::string predTrainFile = params->getString(TRAIN_PRED_OUTFILE);
    std::string predTestFile = params->getString(TEST_PRED_OUTFILE);
    std::string weightFile = params->getString(WEIGHTS_FILE);
    int nbDimlpNets = countNetworksInFile(weightFile);
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

    if (params->isStringSet(TRAIN_CLASS_FILE) != false) {
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

    auto net = std::make_shared<BagDimlp>(quant, nbLayers, vecNbNeurons, nbDimlpNets, weightFileSave);

    net->DefNetsWithWeights(weightFile);

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
      std::ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
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

      All = Train;

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
