#include "fidexFct.h"

using namespace std;

void showFidexParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;
  std::cout << "fidex --train_data_file <train data file> --train_pred_file <train prediction file> --train_class_file <train true class file, not mendatory if classes are specified in train data file> ";
  std::cout << "--test_data_file <test sample(s) data file with data, prediction(if no --test_pred_file) and true class(if no --test_class_file)> ";
  std::cout << "--weights_file <weights file when not using bagging> [Not mendatory if a rules file is given with --rules_file] ";
  std::cout << "--weights_generic_filename <weights file in case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] ";
  std::cout << "--rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file or a weights_generic_filename is given] ";
  std::cout << "--rules_outfile <Rule(s) output file> ";
  std::cout << "--nb_attributes <number of attributes> ";
  std::cout << "--nb_classes <number of classes> ";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "--nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << std::endl;
  std::cout << "--test_pred_file <test prediction file> ";
  std::cout << "--test_class_file <test true class file> If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)" << std::endl;
  std::cout << "--attributes_file <file of attributes>" << std::endl;
  std::cout << "--stats_file <output statistic file>" << std::endl;
  std::cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "--max_iterations <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)>" << std::endl;
  std::cout << "--min_covering <minimum covering number (2 by default)>" << std::endl;
  std::cout << "--covering_strategy <if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)>" << std::endl;
  std::cout << "--max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (30 by default)>" << std::endl;
  std::cout << "--min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)>" << std::endl;
  std::cout << "--lowest_min_fidelity <minimal min_fidelity to which we agree to go down during covering_strategy (0.75 by default)>" << std::endl;
  std::cout << "--dropout_dim <dimension dropout parameter (None by default)>" << std::endl;
  std::cout << "--dropout_hyp <hyperplan dropout parameter (None by default)>" << std::endl;
  std::cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  std::cout << "--decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "--positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << std::endl;
  std::cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  std::cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << std::endl;
  std::cout << "--seed <seed (0=random, default)>";

  std::cout << "\n-------------------------------------------------\n"
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

int fidex(const string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf

  try {
    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    string s;
    stringstream ss(" " + command);

    while (getline(ss, s, ' ')) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 3) {
      showFidexParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, TEST_DATA_FILE,
                                              WEIGHTS_FILE, WEIGHTS_GENERIC_FILENAME, RULES_FILE, RULES_OUTFILE, NB_ATTRIBUTES, NB_CLASSES,
                                              ROOT_FOLDER, TEST_PRED_FILE, TEST_CLASS_FILE, ATTRIBUTES_FILE,
                                              STATS_FILE, CONSOLE_FILE, MAX_ITERATIONS, MIN_COVERING, COVERING_STRATEGY,
                                              MAX_FAILED_ATTEMPTS, MIN_FIDELITY, LOWEST_MIN_FIDELITY, DROPOUT_DIM, DROPOUT_HYP,
                                              NB_QUANT_LEVELS, DECISION_THRESHOLD, POSITIVE_CLASS_INDEX, NORMALIZATION_FILE, MUS,
                                              SIGMAS, NORMALIZATION_INDICES, NB_DIMLP_NETS, SEED};
    if (commandList[2].compare("--json_config_file") == 0) {
      if (commandList.size() < 4) {
        throw CommandArgumentException("JSON config file name/path is missing");
      }

      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[3], validParams));
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
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    cout << *params;

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

    string weightsFile;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFile = params->getString(WEIGHTS_FILE);
    }

    string inputRulesFile;
    if (params->isStringSet(RULES_FILE)) {
      inputRulesFile = params->getString(RULES_FILE);
    }

    vector<string> weightsFiles;
    if (params->isStringSet(WEIGHTS_GENERIC_FILENAME)) {
      weightsFiles = params->getWeightsFiles();
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
    vector<int> mainSamplesPreds;
    vector<int> mainSamplesTrueClass;
    vector<vector<double>> mainSamplesValues;
    vector<vector<double>> mainSamplesOutputValuesPredictions;
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
    vector<string> attributeNames;
    vector<string> classNames;
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

    vector<int> normalizationIndices;
    vector<double> mus;
    vector<double> sigmas;

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

    std::cout << "Files imported" << endl
              << endl;
    std::cout << "----------------------------------------------" << endl
              << endl;

    float temps2;
    clock_t d1;
    clock_t d2;

    d1 = clock();

    // compute hyperspace
    std::cout << "Creation of hyperspace..." << std::endl;

    std::vector<std::vector<double>> matHypLocus;

    if (params->isStringSet(WEIGHTS_FILE)) {
      matHypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
    } else if (params->isStringSet(WEIGHTS_GENERIC_FILENAME)) {
      std::cout << "\nParameters of hyperLocus :\n"
                << std::endl;
      std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
      std::cout << "- Interval : [-" << hiKnot << "," << hiKnot << "]" << std::endl
                << std::endl;
      std::cout << "Computation of all hyperlocus" << std::endl;
      for (string wf : weightsFiles) {
        std::vector<std::vector<double>> hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot, false); // Get hyperlocus
        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end());                    // Concatenate hypLocus to matHypLocus
      }
      std::cout << "All hyperlocus created" << std::endl;
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, *testDatas);
    }

    Hyperspace hyperspace(matHypLocus); // Initialize hyperbox and get hyperplans

    const size_t nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributes != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributes) + ".");
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    auto fidex = Fidex(*trainDatas, *params, hyperspace, true);

    // Stats
    double meanFidelity = 0;
    double meanConfidence = 0;
    double meanCovSize = 0;
    double meanNbAntecedentsPerRule = 0;
    double meanAccuracy = 0;
    vector<tuple<int, Rule>> results;

    // We compute rule for each test sample
    for (int currentSample = 0; currentSample < nbTestSamples; currentSample++) {
      Rule rule;

      vector<double> mainSampleValues = mainSamplesValues[currentSample];
      int mainSamplePred = mainSamplesPreds[currentSample];
      double mainSamplePredValue = mainSamplesOutputValuesPredictions[currentSample][mainSamplePred];
      int mainSampleClass;
      if (hasTrueClasses) {
        mainSampleClass = mainSamplesTrueClass[currentSample];
      } else {
        mainSampleClass = -1;
      }

      if (nbTestSamples > 1) {
        std::cout << "Computation of rule for sample " << currentSample << " : " << endl
                  << endl;
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
        std::cout << "Discriminating hyperplans generated." << endl
                  << endl;
      }

      results.push_back(make_tuple(currentSample, rule));

      std::cout << std::endl;
      std::cout << "Extracted rule :" << std::endl;
      std::cout << rule.toString(attributeNames, classNames) << std::endl;

      if (rule.getCoveringSize() < minNbCover) {
        std::cout << "The minimum covering of " << minNbCover << " is not achieved." << std::endl;
      }
      int nbIt = fidex.getNbIt();
      std::cout << "Result found after " << nbIt << " iterations." << std::endl;

      std::cout << "-------------------------------------------------" << std::endl;
    }

    // Stats
    meanFidelity /= static_cast<double>(nbTestSamples);
    meanConfidence /= static_cast<double>(nbTestSamples);
    meanCovSize /= static_cast<double>(nbTestSamples);
    meanNbAntecedentsPerRule /= static_cast<double>(nbTestSamples);
    meanAccuracy /= static_cast<double>(nbTestSamples);

    if (params->isStringSet(STATS_FILE)) {
      writeStatsFile(params->getString(STATS_FILE), nbTestSamples, meanCovSize,
                     meanNbAntecedentsPerRule, meanFidelity, meanAccuracy, meanConfidence);
    }

    d2 = clock();
    temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;

    std::cout << "\nTime without data import = " << temps2 << " sec" << std::endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;

    writeResultsFile(params->getString(RULES_OUTFILE), results, attributeNames, classNames);

    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }

  return 0;
}

// TODO
void writeResultsFile(string const &filename, vector<tuple<int, Rule>> &sampleRuleDict, vector<string> attributesNames, vector<string> classNames) {
  ofstream ofs(filename);

  if (!ofs.is_open() || ofs.fail()) {
    throw FileNotFoundError("JSON file to be written named '" + filename + "' couldn't be opened, cannot proceed.");
  }

  if (filename.substr(filename.find_last_of(".") + 1) == "json") {
      Json jsonData({});
    for (auto &row : sampleRuleDict) {
      jsonData["results"].push_back({{"sampleId", get<0>(row)},{"rule", get<1>(row)}});
    }
    ofs << setw(4) << jsonData << endl;

  } else {
    for (auto &row : sampleRuleDict) {
      ofs << "Rule for sample " << get<0>(row) << " :\n\n"
          << get<1>(row).toString(attributesNames, classNames)
          << "\n-------------------------------------------------\n\n";
    }
  }
}

// TODO
void writeStatsFile(string const &filename, int nbTestSamples, double meanCovSize,
                    double meanNbAntecedentsPerRule, double meanFidelity, double meanAccuracy, double meanConfidence) {
  ofstream outputStatsFile(filename);

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
    throw CannotOpenFileError("Error : Couldn't open stats extraction file " + filename + ".");
  }
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
