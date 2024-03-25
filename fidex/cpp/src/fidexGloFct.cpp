#include "fidexGloFct.h"

using namespace std;

void showParams() {
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

  printOptionDescription("--test_data_file <str>", "Test sample(s) data file with data and prediction(if no --test_pred_file), classes may been added here if launching with fidex(--with_fidex)");
  printOptionDescription("--global_rules_file <str>", "Ruleset input file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--attributes_file <str>", "File of attributes> Mandatory if rules file contains attribute names, if not, do not add it");
  printOptionDescription("--test_pred_file <str>", "Test prediction file> if given, --test_data_file needs to have only test datas");
  printOptionDescription("--explanation_file <str>", "Explanation(s) output file name");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--decision_threshold <float [0,1]>", "Decision threshold for predictions, you need to specify the index of positive class if you want to use it");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of positive class for the usage of decision threshold, index starts at 0");
  printOptionDescription("--with_fidex <bool>", "Whether to use Fidex if no rule is found in global rules (default: False)");
  printOptionDescription("--with_minimal_version <bool>", "Whether to use minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found (default: False)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "If using fidex :" << std::endl
            << std::endl;

  std::cout << "Required :" << std::endl
            << std::endl;
  printOptionDescription("--train_data_file <str>", "Train data file");
  printOptionDescription("--train_pred_file <str>", "Train prediction file");
  printOptionDescription("--train_class_file <str>", "Train true class file, not mandatory if classes are specified in train data file");
  printOptionDescription("--weights_file <str>", "Weights file when not using bagging (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)");

  std::cout << std::endl
            << "Optional :" << std::endl
            << std::endl;

  printOptionDescription("--test_class_file <str>", "Test true class file, classes can be specified in test data file");
  printOptionDescription("--max_iterations <int [1,inf[>", "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum covering number (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Dimension dropout parameter (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Hyperplan dropout parameter (default: 0.0)");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");
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
  std::cout << "fidex.fidexGlo(\"--test_data_file datanormTest.txt --test_pred_file predTest.out --global_rules_file globalRules.rls --nb_attributes 16 --nb_classes 2 --explanation_file explanation.txt --root_folder dimlp/datafiles --with_fidex true --train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_class_file dataclass2Test.txt --weights_file weights.wts\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
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
    vector<string> commandList = {"fidexGlo"};
    string s;
    stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }
    std::size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, GLOBAL_RULES_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE,
                                              TEST_PRED_FILE, EXPLANATION_FILE, CONSOLE_FILE, DECISION_THRESHOLD, POSITIVE_CLASS_INDEX,
                                              WITH_FIDEX, WITH_MINIMAL_VERSION, TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, WEIGHTS_FILE,
                                              RULES_FILE, TEST_CLASS_FILE, MAX_ITERATIONS, MIN_COVERING, COVERING_STRATEGY,
                                              MAX_FAILED_ATTEMPTS, MIN_FIDELITY, LOWEST_MIN_FIDELITY, DROPOUT_DIM, DROPOUT_HYP, NB_QUANT_LEVELS,
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
        throw InternalError("Error : the size of hyperLocus - " + to_string(nbIn) + " is not a multiple of the number of attributs - " + to_string(nbAttributes));
      }

      cout << "Hyperspace created." << endl
           << endl;
    }

    // Get rules
    vector<Rule> rules;
    vector<string> lines;
    std::string statsLine;
    lines.emplace_back("Global statistics of the rule set : ");
    getRules(rules, params->getString(GLOBAL_RULES_FILE), *testDatas);
    // Get stats line
    fstream rulesData;
    rulesData.open(params->getString(GLOBAL_RULES_FILE), ios::in); // Read data file
    if (rulesData.fail()) {
      throw FileNotFoundError("Error : file " + params->getString(GLOBAL_RULES_FILE) + " not found.");
    }
    getline(rulesData, statsLine);
    lines.emplace_back(statsLine);

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
          if (rules[v].getOutputClass() == testSamplesPreds[currentSample]) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          } else {
            notcorrectRules.push_back(v);
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
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found " << std::to_string(activatedRules.size()) << " rules with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            } else {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
              std::cout << "We didn't found any rule with same prediction as the model (class " << std::to_string(testSamplesPreds[currentSample]) << "), but we found 1 rule with class " << std::to_string(ancientClass) << " :" << std::endl
                        << std::endl;
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.emplace_back("R" + std::to_string(v + 1) + ": " + rules[activatedRules[v]].toString(attributeNames, classNames));
              std::cout << "R" << std::to_string(v + 1) << ": " << rules[activatedRules[v]].toString(attributeNames, classNames) << std::endl;
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
            lines.emplace_back("R" + std::to_string(c + 1) + ": " + rules[correctRules[c]].toString(attributeNames, classNames));
            std::cout << "R" << std::to_string(c + 1) << ": " << rules[correctRules[c]].toString(attributeNames, classNames) << std::endl;
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
            lines.emplace_back("F" + std::to_string(n + 1) + ": " + rules[notcorrectRules[n]].toString(attributeNames, classNames));
            std::cout << "F" << std::to_string(n + 1) + ": " << rules[notcorrectRules[n]].toString(attributeNames, classNames) << std::endl;
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
