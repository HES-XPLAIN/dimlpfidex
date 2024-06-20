#include "fidexGloFct.h"

/**
 * @brief Displays the parameters for fidexGlo.
 */
void showFidexGloParams() {
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are located with respect to the root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a JSON configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--test_data_file <str>", "Path to the file containing test sample(s) data, prediction (if no --test_pred_file) and true classes if launching with fidex (--with_fidex and if no --test_class_file)");
  printOptionDescription("--global_rules_file <str>", "Path to the file containing the global rules obtained with fidexGloRules algorithm.");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in the dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in the dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder");
  printOptionDescription("--test_pred_file <str>", "Path to the file containing predictions on the test portion of the dataset. If given, --test_data_file needs to have only the test data");
  printOptionDescription("--explanation_file <str>", "Path to the file where explanation(s) will be stored");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes. Mandatory if rules file contains attribute names; if not, do not add it");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--with_minimal_version <bool>", "Whether to use the minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found (default: False)");
  printOptionDescription("--with_fidex <bool>", "Whether to use the Fidex algorithm if no rule is found in global rules (default: False)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "If using fidex :" << std::endl
            << std::endl;

  std::cout << "Required :" << std::endl
            << std::endl;
  printOptionDescription("--train_data_file <str>", "Path to the file containing the train portion of the dataset");
  printOptionDescription("--train_class_file <str>", "Path to the file containing the train true classes of the dataset, not mandatory if classes are specified in train data file");
  printOptionDescription("--train_pred_file <str>", "Path to the file containing predictions on the train portion of the dataset");
  printOptionDescription("--weights_file <str>", "Path to the file containing the trained weights of the model (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Path to the file containing the trained rules to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)");

  std::cout << std::endl
            << "Optional :" << std::endl
            << std::endl;

  printOptionDescription("--test_class_file <str>", "Path to the file containing the test true classes of the dataset. Classes can be specified in test data file");
  printOptionDescription("--max_iterations <int [1,inf[>", "Maximum iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum number of samples covered by the generated rules (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use the covering strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during the covering_strategy (default: 0.75)");
  printOptionDescription("--nb_fidex_rules <int [1,inf[>", "Number of Fidex rules to compute per sample when launching the Fidex algorithm (default: 1)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Probability of dropping a dimension during rule extraction (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Probability of dropping a hyperplane during rule extraction (default: 0.0)");
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
  std::cout << "fidex.fidexGlo(\"--test_data_file datanormTest.txt --test_pred_file predTest.out --global_rules_file globalRules.rls --nb_attributes 16 --nb_classes 2 --explanation_file explanation.txt --root_folder dimlp/datafiles --with_fidex true --train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_class_file dataclass2Test.txt --weights_file weights.wts\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

/**
 * @brief Executes the Fidex algorithm to extract an explanation rule for a given sample.
 *
 * This function launches the Fidex algorithm based on the specified parameters, dataset, and hyperspace.
 * It constructs one or multiple explanation rules for the given sample. The results are stored in the
 * provided lines vector, and relevant information is printed to the console.
 *
 * @param lines Reference to a vector of strings where the resulting explanation rules and logs will be stored.
 * @param trainDataset Reference to the DataSetFid object containing the training data.
 * @param p Reference to the Parameters object containing all hyperparameters and settings.
 * @param hyperspace Reference to the Hyperspace object constructed from the hyperlocus.
 * @param mainSampleValues Reference to a vector of double values representing the main sample's attributes.
 * @param mainSamplePred Integer representing the predicted class of the main sample.
 * @param mainSamplePredValue Double representing the prediction value for the main sample.
 * @param mainSampleClass Integer representing the true class of the main sample (or -1 if not available).
 * @param attributeNames Reference to a vector of strings containing the attribute names.
 * @param classNames Reference to a vector of strings containing the class names.
 */
void executeFidex(std::vector<std::string> &lines, DataSetFid &trainDataset, Parameters &p, Hyperspace &hyperspace, std::vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass, const std::vector<std::string> &attributeNames, const std::vector<std::string> &classNames) {
  int nbFidexRules = p.getInt(NB_FIDEX_RULES);
  if (nbFidexRules == 1) {
    std::cout << "\nWe launch Fidex." << std::endl;
    lines.emplace_back("\nWe launch Fidex.\n");
  } else {
    std::cout << "\nWe launch Fidex " << std::to_string(nbFidexRules) << " times." << std::endl;
    lines.emplace_back("\nWe launch Fidex " + std::to_string(nbFidexRules) + " times.\n");
  }

  std::vector<Rule> rules;

  auto fidex = Fidex(trainDataset, p, hyperspace, true);

  for (int i = 0; i < nbFidexRules; i++) {
    Rule rule;

    // Launch fidexAlgo
    fidex.setMainSamplePredValue(mainSamplePredValue);
    fidex.launchFidex(rule, mainSampleValues, mainSamplePred, mainSampleClass);
    rules.push_back(rule);
  }
  if (nbFidexRules == 1) {
    std::cout << "\nLocal rule :" << std::endl;
    lines.emplace_back("Local rule :\n");
  } else {
    std::cout << "\nLocal rules :" << std::endl;
    lines.emplace_back("Local rules :\n");
  }
  for (Rule rule : rules) {
    std::cout << rule.toString(attributeNames, classNames) << std::endl;
    lines.emplace_back(rule.toString(attributeNames, classNames) + "\n");
  }
}

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidexGlo.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
 */
void checkParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultBool(WITH_FIDEX, false);
  p.setDefaultBool(WITH_MINIMAL_VERSION, false);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(GLOBAL_RULES_FILE);
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);

  // verifying logic between parameters, values range and so on...
  p.checkAttributeAndClassCounts();

  // If using Fidex :
  if (p.getBool(WITH_FIDEX)) {
    // setting default values
    p.setDefaultNbQuantLevels();
    p.setDefaultFidex();
    p.setDefaultInt(NB_FIDEX_RULES, 1);

    // this sections check if values comply with program logic

    // asserting mandatory parameters
    p.assertStringExists(TRAIN_DATA_FILE);
    p.assertStringExists(TRAIN_PRED_FILE);

    // verifying logic between parameters, values range and so on...
    p.checkParametersFidex();
    if (p.getInt(NB_FIDEX_RULES) < 1) {
      throw CommandArgumentException("Error : Number of Fidex rules must be strictly positive (>=1).");
    }
    p.checkParametersNormalization();

    if (p.isStringSet(TEST_CLASS_FILE) && !p.isStringSet(TEST_PRED_FILE)) {
      throw CommandArgumentException("Error : The test prediction data file(--test_pred_file) needs to be specified if the test class data file(--test_class_file) is given.");
    }

    if (p.getInt(NB_QUANT_LEVELS) <= 2) {
      throw CommandArgumentException("Error : Number of stairs in the staircase activation function must be greater than 2.");
    }
  }
}

/**
 * @brief Executes the FidexGlo algorithm with specified parameters to extract explanation rules for each test sample.
 *
 * For each test sample, FidexGlo extracts explanation rules from the global ruleset created by the fidexGloRules algorithm.
 * If no rule is found in the ruleset and the 'with_fidex' parameter is true, Fidex is called to obtain a local rule.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the test dataset.
 * - If using Fidex, train data needs to be provided, otherwise it's not useful. The notes below suppose train dataset is mandatory.
 * - True train class labels must be provided, either within the data file or separately through a class file. Test classes are given the same way if present.
 * - Train and test predictions are mandatory, either within the data file for test or separately through prediction file for both.
 * - The path of the file containing the global ruleset must be provided.
 * - If using Fidex, the weights file or rules_file (when training with decision trees) obtained from the model training must be provided.
 * - If using Fidex, normalization parameters can be specified to denormalize the rules if data were normalized beforehand.
 * - The parameter 'explanation_file' has to be provided to extract the explanations in a file.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - explanation_file : File containing the explanations for every test sample. An explanation is composed of one or many explanation rules.
 * - console_file : If specified, contains the console output.
 *
 * File formats:
 * - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Only attributes (floats).
 *   2. Attributes (floats) followed by an integer class ID.
 *   3. Attributes (floats) followed by one-hot encoded class.
 * - **Test data files**: These files can also include predictions. The format of each sample in the file will be as follows:
 *   - First Line: Contains data attributes. It may be followed by class information (either as an ID or in one-hot format).
 *   - Second Line: Contains prediction values.
 *   - Third Line (optional): Contains class information, only if it was not included in the first line and if present.
 * - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Integer class ID.
 *   2. One-hot encoded class.
 * - **Prediction files**: These files should contain one line per data sample, each line consisting of a series of numerical values separated
 *   by a space, a comma (CSV), a semicolon (;), or a tab representing the prediction scores for each class.
 * - **Global rule file**: This file is generated by fidexGloRules. The first line contains general statistics in the form:<br>
 *   'Number of rules : 1171, mean sample covering number per rule : 236.923997, mean number of antecedents per rule : 13.020495'<br>
 *   The second line indicates if a decision threshold has been used. If no, it says: 'No decision threshold is used.'
 *   and if yes, it says something like 'Using a decision threshold of 0.3 for class 0'.
 *   Then there is an empty line and each rule is numbered starting from 1 and separated from each other by an empty line. A rule is in the form:<br>
 *   %Rule 1: X2531>=175.95 X2200>=181.05 X1828>=175.95 X2590>=178.5 X1257>=183.6 X2277>=170.85 X1816>=173.4 X3040>=183.6 -> class 0<br>
 *   Train Covering size : 127<br>
 *   Train Fidelity : 1<br>
 *   Train Accuracy : 1<br>
 *   Train Confidence : 0.999919
 * - **Weights file**: This file should be obtained by training with Dimlp, SVM, MLP, or a CNN from dimlpfidex because an additional special Dimlp layer is needed.
 *   If the training was made with more than one network, each network is separated by a "Network <id>" marker.
 *   The first row represents bias values of the Dimlp layer and the second row are values of the weight matrix between the previous layer and the Dimlp layer.
 *   Each value is separated by a space. As an example, if the layers are of size 4, the biases are: b1 b2 b3 b4 and the weights are w1 w2 w3 w4.
 * - **Rule file**: This file should be obtained directly by training with Random Forests or Gradient Boosting from dimlpfidex because rules need to be extracted from the trees.
 * - **Attributes file**: Each line corresponds to one attribute, each attribute must be specified. Classes can be specified
 *   after the attributes but are not mandatory. Each attribute or class must be in one word without spaces (you can use _ to replace a space).
 *   The order is important as the first attribute/class name will represent the first attribute/class in the dataset.
 * - **Normalization file**: Each line contains the mean/median and standard deviation for an attribute.<br>
 *   Format: '2 : original mean: 0.8307, original std: 0.0425'<br>
 *   Attribute indices (index 2 here) can be replaced with attribute names, then an attribute file is required.
 *
 * Example of how to call the function:
 * @par
 * <tt>from dimlpfidex import fidex</tt>
 * @par
 * <tt>fidex.fidexGlo('-\-test_data_file datanormTest.txt -\-test_pred_file predTest.out -\-global_rules_file globalRules.rls -\-nb_attributes 16 -\-nb_classes 2 -\-explanation_file explanation.txt -\-root_folder dimlp/datafiles -\-with_fidex true -\-train_data_file datanormTrain.txt -\-train_pred_file predTrain.out -\-train_class_file dataclass2Train.txt -\-test_class_file dataclass2Test.txt -\-weights_file weights.wts')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, Fidex parameters, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */
int fidexGlo(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"fidexGlo"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }
    std::size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showFidexGloParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, GLOBAL_RULES_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE,
                                              TEST_PRED_FILE, EXPLANATION_FILE, CONSOLE_FILE,
                                              WITH_FIDEX, WITH_MINIMAL_VERSION, TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, WEIGHTS_FILE,
                                              RULES_FILE, TEST_CLASS_FILE, MAX_ITERATIONS, MIN_COVERING, COVERING_STRATEGY,
                                              MAX_FAILED_ATTEMPTS, MIN_FIDELITY, NB_FIDEX_RULES, LOWEST_MIN_FIDELITY, DROPOUT_DIM, DROPOUT_HYP, NB_QUANT_LEVELS,
                                              NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES, SEED};
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
    checkParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      std::string consoleFile = params->getString(CONSOLE_FILE);
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // ----------------------------------------------------------------------

    // Get parameters values

    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    int nbClasses = params->getInt(NB_CLASSES);
    std::string testSamplesDataFile = params->getString(TEST_DATA_FILE);
    bool withFidex = params->getBool(WITH_FIDEX);
    bool minimalVersion = params->getBool(WITH_MINIMAL_VERSION);
    float decisionThreshold;
    int positiveClassIndex;
    getThresholdFromRulesFile(params->getString(GLOBAL_RULES_FILE), decisionThreshold, positiveClassIndex);

    // ----------------------------------------------------------------------

    std::cout << "Importing files..." << std::endl
              << std::endl;

    // Get test data

    std::unique_ptr<DataSetFid> testDatas;
    if (!params->isStringSet(TEST_PRED_FILE)) { // If we have only one test data file with data and prediction
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
    } else { // We have a different file for test predictions
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
    }
    std::vector<std::vector<double>> testSamplesValues = testDatas->getDatas();
    std::vector<int> testSamplesPreds = testDatas->getPredictions();
    std::vector<std::vector<double>> testSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();

    int nbSamples = testDatas->getNbSamples();

    // Get attributes
    std::vector<std::string> attributeNames;
    std::vector<std::string> classNames;
    bool hasClassNames = false;
    if (params->isStringSet(ATTRIBUTES_FILE)) {
      testDatas->setAttributes(params->getString(ATTRIBUTES_FILE), nbAttributes, nbClasses);
      attributeNames = testDatas->getAttributeNames();
      hasClassNames = testDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = testDatas->getClassNames();
      }
    }

    // If we use Fidex
    std::vector<int> testSamplesClasses;
    std::unique_ptr<DataSetFid> trainDatas;
    std::vector<std::vector<double>> matHypLocus;
    bool hasTrueClasses;
    if (withFidex) {

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

      // Import files for Fidex
      std::cout << "Importing files for Fidex..." << std::endl;

      // Get test class data :
      hasTrueClasses = true;
      if (!params->isStringSet(TEST_CLASS_FILE)) {
        if (!testDatas->getHasClasses()) {
          hasTrueClasses = false;
        }
      } else {
        testDatas->setClassFromFile(params->getString(TEST_CLASS_FILE), nbClasses);
      }
      if (hasTrueClasses) {
        testSamplesClasses = testDatas->getClasses();
      }

      if (!params->isStringSet(TRAIN_CLASS_FILE)) {
        trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                        params->getString(TRAIN_DATA_FILE),
                                        params->getString(TRAIN_PRED_FILE),
                                        params->getInt(NB_ATTRIBUTES),
                                        params->getInt(NB_CLASSES),
                                        decisionThreshold,
                                        positiveClassIndex));

        if (!trainDatas->getHasClasses()) {
          throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
        }
      } else {
        trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                        params->getString(TRAIN_DATA_FILE),
                                        params->getString(TRAIN_PRED_FILE),
                                        params->getInt(NB_ATTRIBUTES),
                                        params->getInt(NB_CLASSES),
                                        decisionThreshold,
                                        positiveClassIndex,
                                        params->getString(TRAIN_CLASS_FILE)));
      }

      int nbTrainSamples = trainDatas->getNbSamples();
      if (params->getInt(MIN_COVERING) > nbTrainSamples) {
        throw CommandArgumentException("Error : Invalide value for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
      }

      // compute hyperspace

      std::cout << "Creation of hyperspace..." << std::endl;

      std::string weightsFile;
      if (params->isStringSet(WEIGHTS_FILE)) {
        weightsFile = params->getString(WEIGHTS_FILE);
      }
      std::string inputRulesFile;
      if (params->isStringSet(RULES_FILE)) {
        inputRulesFile = params->getString(RULES_FILE);
      }
      int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
      float hiKnot = params->getFloat(HI_KNOT);

      if (params->isStringSet(WEIGHTS_FILE)) {
        matHypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
      } else {
        matHypLocus = calcHypLocus(inputRulesFile, *testDatas);
      }

      // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
      auto nbIn = static_cast<int>(matHypLocus.size());

      // Check size of hyperlocus
      if (nbIn == 0 || nbIn % nbAttributes != 0) {
        throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributes - " + std::to_string(nbAttributes));
      }

      std::cout << "Hyperspace created." << std::endl
                << std::endl;
    }

    // Get rules
    std::vector<Rule> rules;
    std::vector<std::string> lines;
    std::string statsLine;
    std::fstream rulesData;
    std::string rulesFile = params->getString(GLOBAL_RULES_FILE);
    lines.emplace_back("Global statistics of the rule set : ");

    if (rulesFile.substr(rulesFile.find_last_of(".") + 1) == "json") {
      rules = Rule::fromJsonFile(rulesFile, decisionThreshold, positiveClassIndex);

      double meanCovering = 0;
      double meanNbAntecedentsPerRule = 0;
      auto nbRules = static_cast<int>(rules.size());

      for (Rule r : rules) {
        meanCovering += static_cast<double>(r.getCoveredSamples().size());
        meanNbAntecedentsPerRule += static_cast<double>(r.getAntecedents().size());
      }
      meanCovering /= nbRules;
      meanNbAntecedentsPerRule /= nbRules;

      statsLine += "Number of rules : " + std::to_string(nbRules);
      statsLine += ", mean sample covering number per rule : ";
      statsLine += std::to_string(meanCovering) + ", mean number of antecedents per rule : ";
      statsLine += std::to_string(meanNbAntecedentsPerRule) + "\n";

    } else {
      rulesData.open(rulesFile, std::ios::in); // Read data file
      if (rulesData.fail()) {
        throw FileNotFoundError("Error : file " + rulesFile + " not found.");
      }
      getline(rulesData, statsLine);
      statsLine += "\n";
      getRules(rules, rulesFile, *testDatas, decisionThreshold, positiveClassIndex);
    }
    lines.emplace_back(statsLine);

    std::cout << "Files imported" << std::endl
              << std::endl;
    if (nbSamples > 1) {
      std::cout << "Find explanation for each sample..." << std::endl
                << std::endl;
    }

    if (decisionThreshold != -1) {
      std::string classDecision;
      if (hasClassNames) {
        classDecision = classNames[positiveClassIndex];
      } else {
        classDecision = std::to_string(positiveClassIndex);
      }
      lines.emplace_back("Using a decision threshold of " + formattingDoubleToString(decisionThreshold) + " for class " + classDecision + "\n");
    }

    lines.emplace_back("\n--------------------------------------------------------------------\n");
    std::cout << "\n--------------------------------------------------------------------" << std::endl;

    // we search explanation for each sample

    if (nbSamples == 1) {
      lines.emplace_back("Explanation for the sample :\n");
      std::cout << "Explanation for the sample :" << std::endl
                << std::endl;
    }
    int nb_fidex = 0; // Number of times Fidex is used
    bool launchingFidex;
    for (int currentSample = 0; currentSample < nbSamples; currentSample++) {
      launchingFidex = false;
      if (nbSamples > 1) {
        lines.push_back("Explanation for sample " + std::to_string(currentSample) + " :\n");
        std::cout << "Explanation for sample " << std::to_string(currentSample) << " :" << std::endl
                  << std::endl;
      }
      int currentPredId = testSamplesPreds[currentSample];
      std::string currentPred;
      if (hasClassNames) {
        currentPred = classNames[currentPredId];
      } else {
        currentPred = std::to_string(currentPredId);
      }
      lines.emplace_back("The model predicts class " + currentPred + " with probability " + std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) + "\n");
      std::cout << "The model predicts class " << currentPred << " with probability " << std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) << std::endl
                << std::endl;
      // Find rules activated by this sample
      std::vector<int> activatedRules;
      getActivatedRules(activatedRules, rules, testSamplesValues[currentSample]);
      // Check which rules are correct
      std::vector<int> correctRules;
      std::vector<int> notCorrectRules;
      bool notShowUncorrectRules = false;
      if (activatedRules.empty()) { // If there is no activated rule
        std::cout << "There is no rule activated" << std::endl;
        std::cout << "We couldn't find any global explanation for this sample." << std::endl; // There is no explanation, we choose the model decision
        std::cout << "We choose the model prediction." << std::endl;
        std::cout << "The predicted class is " << std::to_string(testSamplesPreds[currentSample]) << std::endl;
        lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
        lines.emplace_back("We choose the model prediction.");
        lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
        if (withFidex) {
          launchingFidex = true;
          nb_fidex += 1;
        }

      } else { // There are some activated rules
        for (int v : activatedRules) {
          if (rules[v].getOutputClass() == testSamplesPreds[currentSample]) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          } else {
            notCorrectRules.push_back(v);
          }
        }
        if (correctRules.empty()) { // If there is no correct rule
          int ancientClass = rules[activatedRules[0]].getOutputClass();
          bool allSameClass = true; // Check if all the rules choose the same class
          for (int v : activatedRules) {
            if (rules[v].getOutputClass() != ancientClass) {
              allSameClass = false;
              break;
            }
          }

          if (allSameClass && !minimalVersion) {
            notShowUncorrectRules = true;
            if (activatedRules.size() > 1) {
              lines.emplace_back("We didn't find any rule with the same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't find any rule with the same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found " << std::to_string(activatedRules.size()) << " rules with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            } else {
              lines.emplace_back("We didn't find any rule with the same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't find any rule with the same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found 1 rule with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.emplace_back("R" + std::to_string(v + 1) + ": " + rules[activatedRules[v]].toString(attributeNames, classNames));
              std::cout << "R" << std::to_string(v + 1) << ": " << rules[activatedRules[v]].toString(attributeNames, classNames) << std::endl;
            }
          } else { // If minimalVersion or uncorrected rules are not all same class (only if we don't have correct rules)
            if (minimalVersion) {
              std::cout << "There is no correct activated rule for this sample." << std::endl;
            } else {
              std::cout << "There is no correct rule for this sample." << std::endl;
            }

            std::cout << "We couldn't find any global explanation for this sample." << std::endl; // There is no explanation, we choose the model decision
            std::cout << "We choose the model prediction." << std::endl;
            std::cout << "The predicted class is " << std::to_string(testSamplesPreds[currentSample]) << std::endl;
            lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
            lines.emplace_back("We choose the model prediction.");
            lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
            if (withFidex) {
              launchingFidex = true;
              nb_fidex += 1;
            }
          }

        } else { // There is an explanation which is caracterised by the correct rules
          if (correctRules.size() > 1) {
            lines.emplace_back("We have found " + std::to_string(correctRules.size()) + " global rules explaining the model prediction :\n"); // There is no explanation, we choose the model decision
            std::cout << "We have found " << std::to_string(correctRules.size()) << " global rules explaining the model prediction :" << std::endl
                      << std::endl; // There is no explanation, we choose the model decision
          } else {
            lines.emplace_back("We have found 1 global rule explaining the model prediction :\n"); // There is no explanation, we choose the model decision
            std::cout << "We have found 1 global rule explaining the model prediction :" << std::endl
                      << std::endl; // There is no explanation, we choose the model decision
          }
          for (int c = 0; c < correctRules.size(); c++) {
            lines.emplace_back("R" + std::to_string(c + 1) + ": " + rules[correctRules[c]].toString(attributeNames, classNames));
            std::cout << "R" << std::to_string(c + 1) << ": " << rules[correctRules[c]].toString(attributeNames, classNames) << std::endl;
          }
        }
      }
      if (minimalVersion) {
        notShowUncorrectRules = true;
      }
      if (!notShowUncorrectRules) {
        if (!notCorrectRules.empty()) {
          lines.emplace_back("\nActivated rules without correct decision class :");
          std::cout << "\nActivated rules without correct decision class :" << std::endl;
          for (int n = 0; n < notCorrectRules.size(); n++) {
            lines.emplace_back("F" + std::to_string(n + 1) + ": " + rules[notCorrectRules[n]].toString(attributeNames, classNames));
            std::cout << "F" << std::to_string(n + 1) + ": " << rules[notCorrectRules[n]].toString(attributeNames, classNames) << std::endl;
          }
        } else {
          lines.emplace_back("\nThere are no incorrect rules.");
          std::cout << "\nThere are no uncorrect rules." << std::endl;
        }
      }

      if (launchingFidex) {
        auto trainDataset = *trainDatas.get();
        Hyperspace hyperspace(matHypLocus);
        std::vector<double> mainSampleValues = testSamplesValues[currentSample];
        int mainSamplePred = testSamplesPreds[currentSample];
        double mainSamplePredValue = testSamplesOutputValuesPredictions[currentSample][mainSamplePred];
        int mainSampleClass;
        if (hasTrueClasses) {
          mainSampleClass = testSamplesClasses[currentSample];
        } else {
          mainSampleClass = -1;
        }
        executeFidex(lines, trainDataset, *params, hyperspace, mainSampleValues, mainSamplePred, mainSamplePredValue, mainSampleClass, attributeNames, classNames);
      }

      lines.emplace_back("\n--------------------------------------------------------------------\n");
      std::cout << "\n--------------------------------------------------------------------" << std::endl;
    }

    if (withFidex) {
      double fidex_mean;
      if (nb_fidex == 0) {
        fidex_mean = 0;
      } else {
        fidex_mean = (static_cast<double>(nb_fidex) / static_cast<double>(nbSamples)) * 100;
      }

      lines.emplace_back("\nFidex is used for " + std::to_string(fidex_mean) + "% of samples.");
      std::cout << "\nFidex is used for " + std::to_string(fidex_mean) + "% of samples." << std::endl;
    }

    // Output global explanation result
    if (params->isStringSet(EXPLANATION_FILE)) {
      std::ofstream outputFile(params->getString(EXPLANATION_FILE));
      if (outputFile.is_open()) {
        for (const auto &line : lines) {
          outputFile << line << "" << std::endl;
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + params->getString(EXPLANATION_FILE) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
