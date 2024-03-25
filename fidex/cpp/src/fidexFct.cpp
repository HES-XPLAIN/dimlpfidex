#include "fidexFct.h"

void showFidexParams() {
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
  printOptionDescription("--train_pred_file <str>", "Train prediction file");
  printOptionDescription("--train_class_file <str>", "Train true class file, not mandatory if classes are specified in train data file");
  printOptionDescription("--test_data_file <str>", "Test sample(s) data file with data, prediction(if no --test_pred_file) and true class(if no --test_class_file)");
  printOptionDescription("--weights_file <str>", "Weights file (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)");
  printOptionDescription("--rules_outfile <str>", "Rule(s) output file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--test_pred_file <str>", "Test prediction file");
  printOptionDescription("--test_class_file <str>", "Test true class file. If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)");
  printOptionDescription("--attributes_file <str>", "File of attributes");
  printOptionDescription("--stats_file <str>", "Output statistic file name");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--max_iterations <int [1,inf[>", "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum covering number (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Dimension dropout parameter (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Hyperplan dropout parameter (default: 0.0)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
  printOptionDescription("--decision_threshold <float [0,1]>", "Decision threshold for predictions, you need to specify the index of positive class if you want to use it");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of positive class for the usage of decision threshold, index starts at 0");
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
  std::cout << "fidex.fidex(\"--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_data_file testSampleDataCombine.txt --nb_attributes 16 --nb_classes 2 --weights_file weights.wts --rules_outfile rules.rls --stats_file stats.txt --root_folder fidex/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
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
        throw CommandArgumentException("JSON config file name/path is invalid");
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
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributes) + ".");
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
      meanNbAntecedentsPerRule += static_cast<double>(rule.getNbAntecedants());

      if (nbTestSamples == 1) {
        std::cout << "Discriminating hyperplans generated." << std::endl
                  << std::endl;
      }

      std::stringstream stream;
      lines.push_back(rule.toString(attributeNames, classNames));
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

    std::ofstream outputFile(ruleFile);
    if (outputFile.is_open()) {
      for (const auto &line : lines) {
        outputFile << line << "" << std::endl;
      }
      outputFile.close();
    } else {
      throw CannotOpenFileError("Error : Couldn't open rule extraction file " + ruleFile + ".");
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

/*

  Exemple pour lancer le code :

  ./fidex --train_data_file datanorm --train_pred_file dimlp.out --train_class_file dataclass2 --test_data_file testSampleDataCombine --nb_attributes 16 --nb_classes 2 --weights_file dimlp.wts --nb_quant_levels 50 --rules_outfile rule.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../fidex/datafiles

  ./fidex --train_data_file datanorm --train_pred_file dimlp.out --train_class_file dataclass2 --test_data_file testData.txt --nb_attributes 16 --nb_classes 2 --test_pred_file testPred.txt --test_class_file testClass.txt --weights_file dimlp.wts --nb_quant_levels 50 --rules_outfile rule.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../fidex/datafiles
  ./fidex --train_data_file isoletTrainData.txt --train_pred_file isoletTrainPredV2.out --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --nb_attributes 16 --nb_classes 2 --test_pred_file isoletTestPredV2.out --test_class_file isoletTestClass.txt --weights_file isoletV2.wts --nb_quant_levels 50 --rules_outfile ruleFidex.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../dimlp/datafiles/isoletDataset
  ./fidex --train_data_file Train/X_train.txt --train_pred_file Train/pred_trainV2.out --train_class_file Train/y_train.txt --test_data_file Test/X_test.txt --nb_attributes 16 --nb_classes 2 --test_pred_file Test/pred_testV2.out --test_class_file Test/y_test.txt --weights_file HAPTV2.wts --nb_quant_levels 50 --rules_outfile ruleFidexV2.txt --stats_file stats --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../dimlp/datafiles/HAPTDataset


  #include <profileapi.h>

  LARGE_INTEGER frequency;        - ticks per second
  LARGE_INTEGER t1, t2;           - ticks
  double elapsedTime;

  get ticks per second
  QueryPerformanceFrequency(&frequency);

  start timer
  QueryPerformanceCounter(&t1);

  do something
  ...

  stop timer
  QueryPerformanceCounter(&j2WB2);

  compute and print the elapsed time in millisec
  elapsedTimeWB2 = (j2WB2.QuadPart - j1WB2.QuadPart) * 1000.0 / frequencyWB2.QuadPart;
  std::printf("\npart 2 : Full execution time = %f sec\n", elapsedTimeWB2);

*/
