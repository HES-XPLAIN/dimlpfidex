#include "fidexFct.h"

/**
 * @brief Displays the parameters for fidex.
 */
void showFidexParams() {
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
  printOptionDescription("--train_pred_file <str>", "Path to the file containing predictions on the train portion of the dataset");
  printOptionDescription("--test_data_file <str>", "Path to the file containing the test sample(s) data, prediction (if no --test_pred_file) and true class(if no --test_class_file)");
  printOptionDescription("--weights_file <str>", "Path to the file containing the trained weights of the model (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Path to the file containing the trained rules to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)");
  printOptionDescription("--rules_outfile <str>", "Path to the file where the output rule(s) will be stored. If a .json extension is given, rules are saved in JSON format");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in the dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in the dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder");
  printOptionDescription("--test_class_file <str>", "Path to the file containing the test true classes of the dataset. If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)");
  printOptionDescription("--test_pred_file <str>", "Path to the file containing predictions on the test portion of the dataset");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes");
  printOptionDescription("--stats_file <str>", "Path to the file where statistics concerning the algorithm execution will be stored");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--max_iterations <int [1,inf[>", "Maximum iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum number of samples covered by the generated rules (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use the covering strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during the covering_strategy (default: 0.75)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Probability of dropping a dimension during rule extraction (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Probability of dropping a hyperplane during rule extraction (default: 0.0)");
  printOptionDescription("--decision_threshold <float [0,1]>", "The decision threshold used for predictions, you need to specify the index of the positive class if you want to use it");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of the positive class for the usage of a decision threshold, index starts at 0");
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
  std::cout << "fidex.fidex(\"--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_data_file testSampleDataCombine.txt --nb_attributes 16 --nb_classes 2 --weights_file weights.wts --rules_outfile rules.rls --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidex.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
 */
void checkFidexParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultDecisionThreshold();
  p.setDefaultFidex();

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(TRAIN_PRED_FILE);
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(RULES_OUTFILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersFidex();
  p.checkParametersDecisionThreshold();
  p.checkParametersNormalization();

  if (p.isStringSet(TEST_CLASS_FILE) && !p.isStringSet(TEST_PRED_FILE)) {
    throw CommandArgumentException("Error : The test prediction data file(--test_pred_file) needs to be specified if the test class data file(--test_class_file) is given.");
  }
}

/**
 * @brief Executes the Fidex algorithm with specified parameters to extract an explanation rule for one or several given samples.
 *
 * Fidex is based on the training samples and hyperlocus and directed by the given parameters,
 * including dropout and the maximum number of iterations allowed.
 * It works by identifying hyperplanes in the feature space that discriminate between different
 * classes of samples and constructing a rule for each test sample based on these hyperplanes covering this sample
 * and as many other samples as possible.
 *
 * The %Fidex algorithm is computed until a rule is created or until the max failed attempts limit is reached.<br>
 * - First attempt to generate a rule with a covering greater or equal to 'min_covering' and a fidelity greater or equal to 'min_fidelity'.<br>
 * - If the attempt failed and the 'covering_strategy' is on, Fidex is computed to find a rule with the max possible minimal covering that can be lower than 'min_covering'.<br>
 * - If all attempts failed, the targeted fidelity is gradually lowered until it succeed or 'lowest_min_fidelity' is reached.<br>
 * - Each failed attempt on lowest minimal fidelity are counted.<br>
 * - If the max failed attempts limit is reached, then the rule couldn't be computed for this sample.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the train and test datasets.
 * - True train class labels must be provided, either within the data file or separately through a class file. Test classes are given the same way if present.
 * - Train and test predictions are mandatory, either within the data file for test or separately through prediction file for both.
 * - The weights file or rules_file (when training with decision trees) obtained from the model training must be provided.
 * - The path of the file containing the computed rules must be provided. It can be generated as a JSON if a JSON extension is specified.
 * - Normalization parameters can be specified to denormalize the rules if data were normalized beforehand.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - rules_outfile : File containing the obtained explanation rule for every test sample. It's generated as a JSON if a JSON extension is specified.
 * - stats_file : If specified, contains the statistics of the test set :
 *   1. The mean covering size per rule.
 *   2. The mean number of antecedents per rule.
 *   3. The mean rule fidelity rate.
 *   4. The mean rule accuracy.
 *   5. The mean rule confidence.
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
 * <tt>fidex.fidex('-\-train_data_file datanorm -\-train_pred_file dimlp.out -\-train_class_file dataclass2 -\-test_data_file testSampleDataCombine -\-nb_attributes 16 -\-nb_classes 2 -\-weights_file dimlp.wts -\-nb_quant_levels 50 -\-rules_outfile rule.txt -\-stats_file stats -\-max_iterations 100 -\-min_covering 25 -\-dropout_dim 0.5 -\-dropout_hyp 0.5 -\-root_folder fidex/datafiles')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, Fidex parameters, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */

int fidex(const std::string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf

  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"fidex"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showFidexParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, TEST_DATA_FILE,
                                              WEIGHTS_FILE, RULES_FILE, RULES_OUTFILE, NB_ATTRIBUTES, NB_CLASSES,
                                              ROOT_FOLDER, TEST_PRED_FILE, TEST_CLASS_FILE, ATTRIBUTES_FILE,
                                              STATS_FILE, CONSOLE_FILE, MAX_ITERATIONS, MIN_COVERING, COVERING_STRATEGY,
                                              MAX_FAILED_ATTEMPTS, MIN_FIDELITY, LOWEST_MIN_FIDELITY, DROPOUT_DIM, DROPOUT_HYP,
                                              NB_QUANT_LEVELS, DECISION_THRESHOLD, POSITIVE_CLASS_INDEX, NORMALIZATION_FILE, MUS,
                                              SIGMAS, NORMALIZATION_INDICES, SEED};
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
    checkFidexParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // ----------------------------------------------------------------------

    // Get parameters values

    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    int nbClasses = params->getInt(NB_CLASSES);
    std::string trainDataFile = params->getString(TRAIN_DATA_FILE);
    std::string trainDataFilePred = params->getString(TRAIN_PRED_FILE);
    std::string mainSamplesDataFile = params->getString(TEST_DATA_FILE);
    std::string ruleFile = params->getString(RULES_OUTFILE);
    float decisionThreshold = params->getFloat(DECISION_THRESHOLD);
    int positiveClassIndex = params->getInt(POSITIVE_CLASS_INDEX);
    int minNbCover = params->getInt(MIN_COVERING);
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    float hiKnot = params->getFloat(HI_KNOT);

    std::string weightsFile;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFile = params->getString(WEIGHTS_FILE);
    }

    std::string inputRulesFile;
    if (params->isStringSet(RULES_FILE)) {
      inputRulesFile = params->getString(RULES_FILE);
    }

    // ----------------------------------------------------------------------

    // Import files

    std::cout << "Import files..." << std::endl;

    float importTime;
    clock_t impt1;
    clock_t impt2;

    impt1 = clock();

    std::unique_ptr<DataSetFid> trainDatas;
    if (!params->isStringSet(TRAIN_CLASS_FILE)) {
      trainDatas.reset(new DataSetFid("trainDatas from Fidex", trainDataFile, trainDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      if (!trainDatas->getHasClasses()) {
        throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
      }
    } else {
      trainDatas.reset(new DataSetFid("trainDatas from Fidex", trainDataFile, trainDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex, params->getString(TRAIN_CLASS_FILE)));
    }

    int nbTrainSamples = trainDatas->getNbSamples();

    if (minNbCover > nbTrainSamples) {
      throw CommandArgumentException("Error : invalide type for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
    }

    // Get test data
    std::vector<int> mainSamplesPreds;
    std::vector<int> mainSamplesTrueClass;
    std::vector<std::vector<double>> mainSamplesValues;
    std::vector<std::vector<double>> mainSamplesOutputValuesPredictions;
    std::unique_ptr<DataSetFid> testDatas;
    bool hasTrueClasses;
    if (!params->isStringSet(TEST_PRED_FILE)) { // If we have only one test data file with data, pred and class
      testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      mainSamplesValues = testDatas->getDatas();
      mainSamplesPreds = testDatas->getPredictions();
      mainSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();
      hasTrueClasses = testDatas->getHasClasses();
      if (hasTrueClasses) {
        mainSamplesTrueClass = testDatas->getClasses();
      }

    } else { // We have different files for test predictions and test classes

      if (params->isStringSet(TEST_CLASS_FILE)) {
        testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex, params->getString(TEST_CLASS_FILE)));
      } else {
        testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      }
      mainSamplesValues = testDatas->getDatas();
      mainSamplesPreds = testDatas->getPredictions();
      mainSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();

      // Classes :
      if (params->isStringSet(TEST_CLASS_FILE)) {
        hasTrueClasses = true;
        mainSamplesTrueClass = testDatas->getClasses();
      } else {
        hasTrueClasses = false;
      }
    }

    int nbTestSamples = testDatas->getNbSamples();

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

    impt2 = clock();
    importTime = (float)(impt2 - impt1) / CLOCKS_PER_SEC;

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

    std::cout << "\nImport time = " << importTime << " sec" << std::endl;

    std::cout << "Files imported" << std::endl
              << std::endl;
    std::cout << "----------------------------------------------" << std::endl
              << std::endl;

    float temps2;
    clock_t d1;
    clock_t d2;

    d1 = clock();

    std::vector<std::string> lines;
    // compute hyperspace
    std::cout << "Creation of hyperspace..." << std::endl;

    std::vector<std::vector<double>> matHypLocus;

    if (params->isStringSet(WEIGHTS_FILE)) {
      matHypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, *testDatas);
    }

    Hyperspace hyperspace(matHypLocus); // Initialize hyperbox and get hyperplans

    const size_t nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributes != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributes - " + std::to_string(nbAttributes) + ".");
    }

    std::cout << "Hyperspace created" << std::endl
              << std::endl;

    auto fidex = Fidex(*trainDatas, *params, hyperspace, true);

    // Stats
    double meanFidelity = 0;
    double meanConfidence = 0;
    double meanCovSize = 0;
    double meanNbAntecedentsPerRule = 0;
    double meanAccuracy = 0;
    std::vector<Rule> rules;

    // We compute rule for each test sample
    for (int currentSample = 0; currentSample < nbTestSamples; currentSample++) {
      Rule rule;

      std::vector<double> mainSampleValues = mainSamplesValues[currentSample];
      int mainSamplePred = mainSamplesPreds[currentSample];
      double mainSamplePredValue = mainSamplesOutputValuesPredictions[currentSample][mainSamplePred];
      int mainSampleClass;
      if (hasTrueClasses) {
        mainSampleClass = mainSamplesTrueClass[currentSample];
      } else {
        mainSampleClass = -1;
      }

      lines.push_back("Rule for sample " + std::to_string(currentSample) + " :\n");

      if (nbTestSamples > 1) {
        std::cout << "Computation of rule for sample " << currentSample << " : " << std::endl
                  << std::endl;
      }

      if (nbTestSamples == 1) {
        std::cout << "Searching for discriminating hyperplans..." << std::endl;
      }

      // Launch fidexAlgo
      fidex.setMainSamplePredValue(mainSamplePredValue);
      fidex.launchFidex(rule, mainSampleValues, mainSamplePred, mainSampleClass, true);

      meanFidelity += rule.getFidelity();
      meanAccuracy += rule.getAccuracy();
      meanConfidence += rule.getConfidence();
      meanCovSize += static_cast<double>(rule.getCoveringSize());
      meanNbAntecedentsPerRule += static_cast<double>(rule.getNbAntecedents());

      if (nbTestSamples == 1) {
        std::cout << "Discriminating hyperplans generated." << std::endl
                  << std::endl;
      }

      std::stringstream stream;
      lines.push_back(rule.toString(attributeNames, classNames));
      rules.push_back(rule);
      std::cout << std::endl;
      std::cout << "Extracted rule :" << std::endl;
      std::cout << rule.toString(attributeNames, classNames) << std::endl;

      if (rule.getCoveringSize() < minNbCover) {
        std::cout << "The minimum covering of " << minNbCover << " is not achieved." << std::endl;
      }
      int nbIt = fidex.getNbIt();
      std::cout << "Result found after " << nbIt << " iterations." << std::endl;

      std::cout << "-------------------------------------------------" << std::endl;

      if (nbTestSamples > 1) {
        lines.emplace_back("-------------------------------------------------\n");
      }
    }

    // Stats
    meanFidelity /= static_cast<double>(nbTestSamples);
    meanConfidence /= static_cast<double>(nbTestSamples);
    meanCovSize /= static_cast<double>(nbTestSamples);
    meanNbAntecedentsPerRule /= static_cast<double>(nbTestSamples);
    meanAccuracy /= static_cast<double>(nbTestSamples);

    if (params->isStringSet(STATS_FILE)) {
      std::ofstream outputStatsFile(params->getString(STATS_FILE));

      if (outputStatsFile.is_open()) {

        outputStatsFile << "Statistics with a test set of " << nbTestSamples << " samples :\n"
                        << std::endl;

        if (params->getFloat(DECISION_THRESHOLD) < 0.0) {
          outputStatsFile << "No decision threshold is used.\n";
        } else {
          outputStatsFile << "Using a decision threshold of " << params->getFloat(DECISION_THRESHOLD) << " for class " << params->getInt(POSITIVE_CLASS_INDEX) << "\n";
        }

        outputStatsFile << "The mean covering size per rule is : " << meanCovSize << "" << std::endl;
        outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntecedentsPerRule << "" << std::endl;
        outputStatsFile << "The mean rule fidelity rate is : " << meanFidelity << "" << std::endl;
        outputStatsFile << "The mean rule accuracy is : " << meanAccuracy << "" << std::endl;
        outputStatsFile << "The mean rule confidence is : " << meanConfidence << "" << std::endl;
        outputStatsFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open stats extraction file " + params->getString(STATS_FILE) + ".");
      }
    }

    if (ruleFile.substr(ruleFile.find_last_of('.') + 1) == "json") {
      Rule::toJsonFile(ruleFile, rules, params->getFloat(DECISION_THRESHOLD), params->getInt(POSITIVE_CLASS_INDEX));
    } else {
      std::ofstream outputFile(ruleFile);

      if (outputFile.is_open()) {
        if (params->getFloat(DECISION_THRESHOLD) < 0.0) {
          outputFile << "No decision threshold is used.\n";
        } else {
          outputFile << "Using a decision threshold of " << params->getFloat(DECISION_THRESHOLD) << " for class " << params->getInt(POSITIVE_CLASS_INDEX) << "\n";
        }

        outputFile << std::endl;

        for (const auto &line : lines) {
          outputFile << line << "" << std::endl;
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open rule extraction file " + ruleFile + ".");
      }
    }

    d2 = clock();
    temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;

    std::cout << "\nTime without data import = " << temps2 << " sec" << std::endl;

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
