#include "fidexGloFct.h"

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;

  std::cout << "fidexGlo --test_data_file <test sample(s) data file with data and prediction(if no --test_pred_file), classes may been added here if launching with fidex(--with_fidex)> ";
  std::cout << "--global_rules_file <ruleset input file> ";
  std::cout << "--nb_attributes <number of attributes> ";
  std::cout << "--nb_classes <number of classes> ";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "--attributes_file <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it" << std::endl;
  std::cout << "--test_pred_file <test prediction file> if given, --test_data_file needs to have only test datas" << std::endl;
  std::cout << "--explanation_file <Explanation(s) output file>" << std::endl;
  std::cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "--decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "--positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << std::endl;
  std::cout << "--with_fidex <with Fidex if not rule is given (False by default)>" << std::endl;
  std::cout << "--with_minimal_version <minimal version, only correct activated rules, launch Fidex when no such rule is found(if with fidex) (False by default)>" << std::endl
            << std::endl;
  std::cout << "If using fidex :" << std::endl;
  std::cout << "Obligatory :" << std::endl;
  std::cout << "--train_data_file <train data file>" << std::endl;
  std::cout << "--train_pred_file <train prediction file>" << std::endl;
  std::cout << "--train_class_file <train true class file, not mendatory if classes are specified in train data file>" << std::endl;
  std::cout << "--weights_file <weights file when not using bagging> [Not mendatory if a rules file is given with --rules_file] ";
  std::cout << "--weights_generic_filename <weights file in case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] ";
  std::cout << "--rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file or a weights_generic_filename is given] ";
  std::cout << "Options :" << std::endl;
  std::cout << "--test_class_file <test true class file, classes can be specified in test data file>" << std::endl;
  std::cout << "--nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << std::endl;
  std::cout << "--max_iterations <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)>" << std::endl;
  std::cout << "--min_covering <minimum covering number (2 by default)>" << std::endl;
  std::cout << "--covering_strategy <if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)>" << std::endl;
  std::cout << "--max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (30 by default)>" << std::endl;
  std::cout << "--min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)>" << std::endl;
  std::cout << "--lowest_min_fidelity <minimal min_fidelity to which we agree to go down during covering_strategy (0.75 by default)>" << std::endl;
  std::cout << "--dropout_dim <dimension dropout parameter (None by default)>" << std::endl;
  std::cout << "--dropout_hyp <hyperplan dropout parameter (None by default)>" << std::endl;
  std::cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  std::cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  std::cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << std::endl;
  std::cout << "--seed <seed (0=random, default)>";

  std::cout << "\n-------------------------------------------------\n"
            << std::endl;
}

void executeFidex(std::vector<std::string> &lines, DataSetFid &trainDataset, Parameters &p, Hyperspace &hyperspace, vector<double> &mainSampleValues, int mainSamplePred, double mainSamplePredValue, int mainSampleClass, const vector<string> &attributeNames, const vector<string> &classNames) {

  std::cout << "\nWe launch Fidex." << std::endl;
  lines.emplace_back("\nWe launch Fidex.\n");

  Rule rule;

  auto fidex = Fidex(trainDataset, p, hyperspace, true);

  // Launch fidexAlgo
  fidex.setMainSamplePredValue(mainSamplePredValue);
  fidex.launchFidex(rule, mainSampleValues, mainSamplePred, mainSampleClass);

  std::cout << "\nLocal rule :" << std::endl;
  lines.emplace_back("Local rule :\n");
  std::cout << rule.toString(attributeNames, classNames) << std::endl;
  lines.emplace_back(rule.toString(attributeNames, classNames) + "\n");
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultFloat(DECISION_THRESHOLD, -1.0f);
  p.setDefaultInt(POSITIVE_CLASS_INDEX, -1);
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
  p.checkParametersDecisionThreshold();

  // If using Fidex :
  if (p.getBool(WITH_FIDEX)) {
    // setting default values
    p.setDefaultNbQuantLevels();
    p.setDefaultFidex();

    // this sections check if values comply with program logic

    // asserting mandatory parameters
    p.assertStringExists(TRAIN_DATA_FILE);
    p.assertStringExists(TRAIN_PRED_FILE);

    // verifying logic between parameters, values range and so on...
    p.checkParametersFidex();
    p.checkParametersNormalization();

    if (p.isStringSet(TEST_CLASS_FILE) && !p.isStringSet(TEST_PRED_FILE)) {
      throw CommandArgumentException("Error : The test prediction data file(--test_pred_file) needs to be specified if the test class data file(--test_class_file) is given.");
    }

    if (p.getInt(NB_QUANT_LEVELS) <= 2) {
      throw CommandArgumentException("Error : Number of stairs in staircase activation function must be greater than 2.");
    }
  }
}

int fidexGlo(const string &command) {
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

    while (std::getline(ss, s, ' ')) {
      commandList.push_back(s);
    }
    std::size_t nbParam = commandList.size();
    if (nbParam < 2) {
      showParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, GLOBAL_RULES_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE,
                                              TEST_PRED_FILE, EXPLANATION_FILE, CONSOLE_FILE, DECISION_THRESHOLD, POSITIVE_CLASS_INDEX,
                                              WITH_FIDEX, WITH_MINIMAL_VERSION, TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, WEIGHTS_FILE,
                                              WEIGHTS_GENERIC_FILENAME, RULES_FILE, TEST_CLASS_FILE, MAX_ITERATIONS, MIN_COVERING, COVERING_STRATEGY,
                                              MAX_FAILED_ATTEMPTS, MIN_FIDELITY, LOWEST_MIN_FIDELITY, DROPOUT_DIM, DROPOUT_HYP, NB_QUANT_LEVELS,
                                              NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES, NB_DIMLP_NETS, SEED};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
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
    checkParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      string consoleFile = params->getString(CONSOLE_FILE);
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
    double decisionThreshold = params->getFloat(DECISION_THRESHOLD);
    int positiveClassIndex = params->getInt(POSITIVE_CLASS_INDEX);
    bool withFidex = params->getBool(WITH_FIDEX);
    bool minimalVersion = params->getBool(WITH_MINIMAL_VERSION);

    // ----------------------------------------------------------------------

    std::cout << "Importing files..." << endl
              << endl;

    // Get test data

    std::unique_ptr<DataSetFid> testDatas;
    if (!params->isStringSet(TEST_PRED_FILE)) { // If we have only one test data file with data and prediction
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
    } else { // We have a different file for test predictions
      testDatas.reset(new DataSetFid("testDatas from FidexGlo", testSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
    }
    vector<vector<double>> testSamplesValues = testDatas->getDatas();
    vector<int> testSamplesPreds = testDatas->getPredictions();
    vector<vector<double>> testSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();

    int nbSamples = testDatas->getNbSamples();

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

    // If we use Fidex
    std::vector<int> testSamplesClasses;
    std::unique_ptr<DataSetFid> trainDatas;
    vector<vector<double>> matHypLocus;
    bool hasTrueClasses;
    if (withFidex) {

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

      // Import files for Fidex
      cout << "Importing files for Fidex..." << endl;

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
                                        params->getFloat(DECISION_THRESHOLD),
                                        params->getInt(POSITIVE_CLASS_INDEX)));

        if (!trainDatas->getHasClasses()) {
          throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
        }
      } else {
        trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                        params->getString(TRAIN_DATA_FILE),
                                        params->getString(TRAIN_PRED_FILE),
                                        params->getInt(NB_ATTRIBUTES),
                                        params->getInt(NB_CLASSES),
                                        params->getFloat(DECISION_THRESHOLD),
                                        params->getInt(POSITIVE_CLASS_INDEX),
                                        params->getString(TRAIN_CLASS_FILE)));
      }

      int nbTrainSamples = trainDatas->getNbSamples();
      if (params->getInt(MIN_COVERING) > nbTrainSamples) {
        throw CommandArgumentException("Error : invalide type for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
      }

      // compute hyperspace

      cout << "Creation of hyperspace..." << endl;

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
      int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
      float hiKnot = params->getFloat(HI_KNOT);

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
        if (params->isStringSet(ATTRIBUTES_FILE)) {
          matHypLocus = calcHypLocus(inputRulesFile, nbAttributes, attributeNames);
        } else {
          matHypLocus = calcHypLocus(inputRulesFile, nbAttributes);
        }
      }

      // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
      auto nbIn = static_cast<int>(matHypLocus.size());

      // Check size of hyperlocus
      if (nbIn == 0 || nbIn % nbAttributes != 0) {
        throw InternalError("Error : the size of hyperLocus - " + to_string(nbIn) + " is not a multiple of the number of attributs - " + to_string(nbAttributes));
      }

      cout << "Hyperspace created." << endl
           << endl;
    }

    // Get rules
    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;
    lines.emplace_back("Global statistics of the rule set : ");
    vector<string> stringRules;

    getRules(rules, lines, stringRules, params->getString(GLOBAL_RULES_FILE), params->isStringSet(ATTRIBUTES_FILE), attributeNames, hasClassNames, classNames);

    std::cout << "Files imported" << endl
              << endl;
    if (nbSamples > 1) {
      std::cout << "Find explanation for each sample..." << endl
                << endl;
    }

    if (params->getFloat(DECISION_THRESHOLD) != -1) {
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
      lines.emplace_back("The model predict class " + currentPred + " with probability " + std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) + "\n");
      std::cout << "The model predict class " << currentPred << " with probability " << std::to_string(testSamplesOutputValuesPredictions[currentSample][currentPredId]) << std::endl
                << std::endl;
      // Find rules activated by this sample
      vector<int> activatedRules;
      getActivatedRules(activatedRules, rules, testSamplesValues[currentSample]);
      // Check which rules are correct
      vector<int> correctRules;
      vector<int> notcorrectRules;
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
          if (get<2>(rules[v]) == testSamplesPreds[currentSample]) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          } else {
            notcorrectRules.push_back(v);
          }
        }
        if (correctRules.empty()) { // If there is no correct rule
          int ancientClass = get<2>(rules[activatedRules[0]]);
          bool allSameClass = true; // Check if all the rules choose the same class
          for (int v : activatedRules) {
            if (get<2>(rules[v]) != ancientClass) {
              allSameClass = false;
              break;
            }
          }
          if (allSameClass && !minimalVersion) {
            notShowUncorrectRules = true;
            if (activatedRules.size() > 1) {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found " << std::to_string(activatedRules.size()) << " rules with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            } else {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found 1 rule with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.emplace_back("R" + std::to_string(v + 1) + ": " + stringRules[activatedRules[v]]);
              std::cout << "R" << std::to_string(v + 1) << ": " << stringRules[activatedRules[v]] << std::endl;
            }
          } else {
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
            lines.emplace_back("R" + std::to_string(c + 1) + ": " + stringRules[correctRules[c]]);
            std::cout << "R" << std::to_string(c + 1) << ": " << stringRules[correctRules[c]] << std::endl;
          }
        }
      }
      if (minimalVersion) {
        notShowUncorrectRules = true;
      }
      if (!notShowUncorrectRules) {
        if (!notcorrectRules.empty()) {
          lines.emplace_back("\nActivated rules without correct decision class :");
          std::cout << "\nActivated rules without correct decision class :" << std::endl;
          for (int n = 0; n < notcorrectRules.size(); n++) {
            lines.emplace_back("F" + std::to_string(n + 1) + ": " + stringRules[notcorrectRules[n]]);
            std::cout << "F" << std::to_string(n + 1) + ": " << stringRules[notcorrectRules[n]] << std::endl;
          }
        } else {
          lines.emplace_back("\nThere is no uncorrect rules.");
          std::cout << "\nThere is no uncorrect rules." << std::endl;
        }
      }

      if (launchingFidex) {
        auto trainDataset = *trainDatas.get();
        Hyperspace hyperspace(matHypLocus);
        vector<double> mainSampleValues = testSamplesValues[currentSample];
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
      ofstream outputFile(params->getString(EXPLANATION_FILE));
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
    std::cerr << e.what() << endl;
    return -1;
  }

  return 0;
}

// Exemple pour lancer le code : ./fidexGlo --test_data_file datanormTest --test_pred_file dimlpDatanormTest.out --global_rules_file globalRulesDatanorm.txt --nb_attributes 16 --nb_classes 2 --explanation_file explanation.txt --root_folder ../fidex/datafiles --with_fidex true --train_data_file datanormTrain --train_pred_file dimlpDatanormTrain.out --train_class_file dataclass2Train --test_class_file dataclass2Test --weights_file dimlpDatanorm.wts
