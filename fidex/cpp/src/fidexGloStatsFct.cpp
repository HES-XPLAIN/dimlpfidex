#include "fidexGloStatsFct.h"

void showStatsParams() {
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

  printOptionDescription("--test_data_file <str>", "Test data file");
  printOptionDescription("--test_pred_file <str>", "Test prediction file");
  printOptionDescription("--test_class_file <str>", "Test true class file, not mandatory if classes are specified in test data file");
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
  printOptionDescription("--stats_file <str>", "Output statistic file name");
  printOptionDescription("--global_rules_outfile <str>", "Global ruleset output file with stats on test set, if you want to compute statistics of global rules on tests set");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of positive class to compute true/false positive/negative rates, index starts at 0. If it is specified in the rules file, it has to be the same value.");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "fidex.fidexGloStats(\"--test_data_file datanormTest.txt --test_pred_file predTest.out --test_class_file dataclass2Test.txt --global_rules_file globalRules.rls --nb_attributes 16 --nb_classes 2 --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

void getCovering(std::vector<int> &sampleIds, const Rule &rule, std::vector<std::vector<double>> &testValues) {
  // Get covering index samples
  int attr;
  bool ineq;
  double val;
  for (int id = 0; id < testValues.size(); id++) {
    bool notCovered = false;
    for (const auto &antecedent : rule.getAntecedants()) { // For each antecedant
      attr = antecedent.getAttribute();
      ineq = antecedent.getInequality();
      val = antecedent.getValue();
      if (ineq == 0 && testValues[id][attr] >= val) { // If the inequality is not verified
        notCovered = true;
      }
      if (ineq == 1 && testValues[id][attr] < val) {
        notCovered = true;
      }
    }
    if (!notCovered) {
      sampleIds.push_back(id);
    }
  }
}

void computeTFPN(int decision, int positiveClassIndex, int testTrueClass, int &nbTruePositive, int &nbFalsePositive, int &nbTrueNegative, int &nbFalseNegative) {
  if (decision == positiveClassIndex) { // Positive prediction
    if (decision == testTrueClass) {
      nbTruePositive += 1;
    } else {
      nbFalsePositive += 1;
    }
  } else { // Negative prediction
    if (testTrueClass == positiveClassIndex) {
      nbFalseNegative += 1;
    } else {
      nbTrueNegative += 1;
    }
  }
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkStatsParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultInt(POSITIVE_CLASS_INDEX, -1);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(TEST_PRED_FILE);
  p.assertStringExists(GLOBAL_RULES_FILE);
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);

  // verifying logic between parameters, values range and so on...
  p.checkAttributeAndClassCounts();
  if (p.getInt(POSITIVE_CLASS_INDEX) < 0 && p.getInt(POSITIVE_CLASS_INDEX) != -1) {
    throw CommandArgumentException("Error : Positive class index must be positive (>=0)");
  }

  if (p.getInt(POSITIVE_CLASS_INDEX) >= p.getInt(NB_CLASSES)) {
    throw CommandArgumentException("Error : The index of positive class cannot be greater or equal to the number of classes (" + std::to_string(p.getInt(NB_CLASSES)) + ").");
  }
}

int fidexGloStats(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"fidexGloStats"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }
    std::size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showStatsParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, TEST_PRED_FILE, TEST_CLASS_FILE,
                                              GLOBAL_RULES_FILE, GLOBAL_RULES_OUTFILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE,
                                              STATS_FILE, GLOBAL_RULES_OUTFILE, CONSOLE_FILE, POSITIVE_CLASS_INDEX};
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
    checkStatsParametersLogicValues(*params);

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
    std::string testDataFile = params->getString(TEST_DATA_FILE);
    std::string testDataFilePred = params->getString(TEST_PRED_FILE);
    std::string rulesFile = params->getString(GLOBAL_RULES_FILE);

    // Get decision threshold and positive class index
    float decisionThreshold;
    int positiveClassIndex;
    getThresholdFromRulesFile(rulesFile, decisionThreshold, positiveClassIndex);

    int positiveClassIndexParam = params->getInt(POSITIVE_CLASS_INDEX);
    if (positiveClassIndexParam != -1) {
      if (positiveClassIndex != -1 && positiveClassIndex != positiveClassIndexParam) {
        throw CommandArgumentException("The positive class index must be the same as the one specified in the rules file (" + std::to_string(positiveClassIndex) + ").");
      }
      positiveClassIndex = positiveClassIndexParam;
    }

    // ----------------------------------------------------------------------

    std::cout << "Importing files..." << std::endl
              << std::endl;

    // Get test data

    std::unique_ptr<DataSetFid> testDatas;
    if (!params->isStringSet(TEST_CLASS_FILE)) {
      testDatas.reset(new DataSetFid("testDatas from FidexGloStats", testDataFile, testDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      if (!testDatas->getHasClasses()) {
        throw CommandArgumentException("The test true classes file has to be given with option --test_class_file or classes have to be given in the test data file.");
      }
    } else {
      testDatas.reset(new DataSetFid("testDatas from FidexGloStats", testDataFile, testDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex, params->getString(TEST_CLASS_FILE)));
    }

    std::vector<std::vector<double>> &testData = testDatas->getDatas();
    std::vector<int> &testPreds = testDatas->getPredictions();
    std::vector<int> &testTrueClasses = testDatas->getClasses();

    std::vector<std::vector<double>> &testOutputValuesPredictions = testDatas->getOutputValuesPredictions();
    int nbTestData = testDatas->getNbSamples();

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

    // Get rules
    std::vector<std::string> lines;                             // Lines for the output stats
    lines.emplace_back("Global statistics of the rule set : "); // Lines for the output stats

    // Get statistic line at the top of the rulesfile
    std::string statsLine;
    std::fstream rulesData;
    std::vector<Rule> rules;

    if (rulesFile.substr(rulesFile.find_last_of(".") + 1) == "json") {
      rules = Rule::fromJsonFile(rulesFile);

      double meanCovering = 0;
      double meanNbAntecedantsPerRule = 0;
      auto nbRules = static_cast<int>(rules.size());

      for (Rule r : rules) {
        meanCovering += static_cast<double>(r.getCoveredSamples().size());
        meanNbAntecedantsPerRule += static_cast<double>(r.getAntecedants().size());
      }
      meanCovering /= nbRules;
      meanNbAntecedantsPerRule /= nbRules;

      // TODO add missing line
      statsLine += "Number of rules : " + std::to_string(nbRules);
      statsLine += ", mean sample covering number per rule : ";
      statsLine += std::to_string(meanCovering) + ", mean number of antecedents per rule : ";
      statsLine += std::to_string(meanNbAntecedantsPerRule) + "\n";

    } else {
      rulesData.open(rulesFile, std::ios::in); // Read data file
      if (rulesData.fail()) {
        throw FileNotFoundError("Error : file " + rulesFile + " not found.");
      }
      getline(rulesData, statsLine);
      statsLine += "\n";
      getRules(rules, rulesFile, *testDatas);
    }
    lines.emplace_back(statsLine);

    std::cout << "Data imported." << std::endl
              << std::endl;

    // Compute global statistics on test set

    std::cout << "Compute statistics..." << std::endl
              << std::endl;
    double fidelity = 0;       // Global rule fidelity rate (wrong only if there is activated rules but no correct one(with respect to prediction))
    double accuracy = 0;       // Global rule accuracy (true if there is at least one fidel activated rule and the model is right, or if there is no activated rules and the model is right or if all the activated rules decide the same class and this class is the true one)
    double explainabilityRate; // True if there is an activated rule
    double explainabilityTotal = 0;
    double defaultRuleRate = 0;             // True if there is no activated rule
    double meanNbCorrectActivatedRules = 0; // Mean number of correct activated rules per sample
    double meanNbWrongActivatedRules = 0;   // Mean number of wrong activated rules per sample
    int nbActivatedRulesAndModelAgree = 0;
    double accuracyWhenActivatedRulesAndModelAgree = 0; // Model accuracy when activated rules and model agree (sample has correct activated rules or no activated rules, then percentage of good predictions on them by the model)
    int nbFidelRules = 0;
    double accuracyWhenRulesAndModelAgree = 0; // Model accuracy when activated rules and model agree (sample has correct activated rules, then percentage of good predictions on them by the model)
    double modelAccuracy = 0;
    std::vector<double> testValues;
    int testPred;
    int testTrueClass;

    int nbTruePositive = 0;  // Correct positive prediction
    int nbTrueNegative = 0;  // Correct negative prediction
    int nbFalsePositive = 0; // Wrong positive prediction
    int nbFalseNegative = 0; // Wrong negative prediction
    int nbPositive = 0;
    int nbNegative = 0;

    int nbTruePositiveRules = 0;  // Correct positive rule prediction
    int nbTrueNegativeRules = 0;  // Correct negative rule prediction
    int nbFalsePositiveRules = 0; // Wrong positive rule prediction
    int nbFalseNegativeRules = 0; // Wrong negative rule prediction

    for (int t = 0; t < nbTestData; t++) { // For each test value
      testValues = testData[t];
      testPred = testPreds[t];
      testTrueClass = testTrueClasses[t];
      if (positiveClassIndex != -1) {
        if (testTrueClass == positiveClassIndex) {
          nbPositive += 1;
        } else {
          nbNegative += 1;
        }
      }

      if (testPred == testTrueClass) {
        modelAccuracy++;
      }
      if (positiveClassIndex != -1) {
        computeTFPN(testPred, positiveClassIndex, testTrueClass, nbTruePositive, nbFalsePositive, nbTrueNegative, nbFalseNegative);
      }

      // Find rules activated by this sample
      bool noCorrectRuleWithAllSameClass = false; // If there is no correct rule activated but all rules have same class
      std::vector<int> activatedRules;
      getActivatedRules(activatedRules, rules, testValues);

      // Check which rules are correct
      std::vector<int> correctRules;
      if (activatedRules.empty()) { // If there is no activated rule -> we would launch Fidex and so it will be fidel
        defaultRuleRate++;
        fidelity++; // It is true to the model because we choose his prediction
        nbFidelRules++;
        if (testPred == testTrueClass) { // If the model is right, it's true for the accuracy
          accuracy++;
          accuracyWhenRulesAndModelAgree++;
        }
      } else { // There is some activated rules
        for (int v : activatedRules) {
          if (rules[v].getOutputClass() == testPred) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          }
        }
        if (correctRules.empty()) { // If there is no correct rule
          meanNbWrongActivatedRules += static_cast<double>(activatedRules.size());

          int ancientClass = rules[activatedRules[0]].getOutputClass();
          bool allSameClass = true; // Check if all the rules choose the same class
          for (int v : activatedRules) {
            if (rules[v].getOutputClass() != ancientClass) {
              allSameClass = false;
              break;
            }
          }
          if (allSameClass) {
            explainabilityTotal++; // If all decisions are the same, we have an explanation
            int decision = rules[activatedRules[0]].getOutputClass();
            if (decision == testTrueClass) { // If those decisions are the true class, this is accurate
              accuracy++;
            }
            // The rules' decision is different from the model's
            noCorrectRuleWithAllSameClass = true;
            if (positiveClassIndex != -1) {
              computeTFPN(decision, positiveClassIndex, testTrueClass, nbTruePositiveRules, nbFalsePositiveRules, nbTrueNegativeRules, nbFalseNegativeRules);
            }
          }

        } else { // There is an explanation which is caracterised by the correct rules

          fidelity++; // It is true to the model because we found a correct activated rule
          explainabilityTotal++;
          nbActivatedRulesAndModelAgree++;
          nbFidelRules++;
          if (testPred == testTrueClass) {
            accuracy++;                                // If the model is right, it's true for the accuracy because we found a fidel rule
            accuracyWhenActivatedRulesAndModelAgree++; // It is true for the accuracy because prediction is correct
            accuracyWhenRulesAndModelAgree++;          // It is true for the accuracy because prediction is correct
          }
          meanNbCorrectActivatedRules += static_cast<double>(correctRules.size());
          meanNbWrongActivatedRules += static_cast<double>(activatedRules.size() - correctRules.size());
        }
      }

      if (!noCorrectRuleWithAllSameClass && positiveClassIndex != -1) { // The rules' decision is the same as the model's, if we can find a correct rule or if we need to compute Fidex
        computeTFPN(testPred, positiveClassIndex, testTrueClass, nbTruePositiveRules, nbFalsePositiveRules, nbTrueNegativeRules, nbFalseNegativeRules);
      }
    }

    fidelity /= nbTestData;
    accuracy /= nbTestData;
    explainabilityRate = explainabilityTotal / nbTestData;
    defaultRuleRate /= nbTestData;
    meanNbCorrectActivatedRules /= nbTestData;
    meanNbWrongActivatedRules /= nbTestData;
    modelAccuracy /= nbTestData;
    accuracyWhenRulesAndModelAgree /= nbFidelRules;
    accuracyWhenActivatedRulesAndModelAgree /= nbActivatedRulesAndModelAgree;

    lines.push_back("Statistics with a test set of " + std::to_string(nbTestData) + " samples :\n");
    if (decisionThreshold < 0.0) {
      lines.emplace_back("No decision threshold is used.");
    } else {
      lines.push_back("Decision threshold used : " + std::to_string(decisionThreshold));
    }

    if (positiveClassIndex < 0) {
      lines.emplace_back("No positive index class is used.");
    } else {
      lines.push_back("Positive index class used : " + std::to_string(positiveClassIndex));
    }

    lines.push_back("The global rule fidelity rate is : " + std::to_string(fidelity));
    lines.push_back("The global rule accuracy is : " + std::to_string(accuracy));
    lines.push_back("The explainability rate (when we can find one or more rules, either correct ones or activated ones which all agree on the same class) is : " + std::to_string(explainabilityRate));
    lines.push_back("The default rule rate (when we can't find any rule activated for a sample) is : " + std::to_string(defaultRuleRate));
    lines.push_back("The mean number of correct(fidel) activated rules per sample is : " + std::to_string(meanNbCorrectActivatedRules));
    lines.push_back("The mean number of wrong(not fidel) activated rules per sample is : " + std::to_string(meanNbWrongActivatedRules));
    lines.push_back("The model test accuracy is : " + std::to_string(modelAccuracy));
    lines.push_back("The model test accuracy when rules and model agree is : " + std::to_string(accuracyWhenRulesAndModelAgree));
    lines.push_back("The model test accuracy when activated rules and model agree is : " + std::to_string(accuracyWhenActivatedRulesAndModelAgree));

    if (positiveClassIndex != -1) {
      if (hasClassNames) {
        lines.push_back("\nWith positive class " + classNames[positiveClassIndex] + " :");
      } else {
        lines.push_back("\nWith positive class " + std::to_string(positiveClassIndex) + " :");
      }
      lines.emplace_back("\nComputation with model decision :");
      lines.push_back("The number of true positive test samples is : " + std::to_string(nbTruePositive));
      lines.push_back("The number of false positive test samples is : " + std::to_string(nbFalsePositive));
      lines.push_back("The number of true negative test samples is : " + std::to_string(nbTrueNegative));
      lines.push_back("The number of false negative test samples is : " + std::to_string(nbFalseNegative));
      lines.push_back("The false positive rate is : " + ((nbNegative != 0) ? std::to_string(float(nbFalsePositive) / static_cast<float>(nbNegative)) : "N/A"));
      lines.push_back("The false negative rate is : " + ((nbPositive != 0) ? std::to_string(float(nbFalseNegative) / static_cast<float>(nbPositive)) : "N/A"));
      lines.push_back("The precision is : " + ((nbTruePositive + nbFalsePositive != 0) ? std::to_string(float(nbTruePositive) / static_cast<float>(nbTruePositive + nbFalsePositive)) : "N/A"));
      lines.push_back("The recall is : " + ((nbTruePositive + nbFalseNegative != 0) ? std::to_string(float(nbTruePositive) / static_cast<float>(nbTruePositive + nbFalseNegative)) : "N/A"));

      lines.emplace_back("\nComputation with rules decision :");
      lines.push_back("The number of true positive test samples is : " + std::to_string(nbTruePositiveRules));
      lines.push_back("The number of false positive test samples is : " + std::to_string(nbFalsePositiveRules));
      lines.push_back("The number of true negative test samples is : " + std::to_string(nbTrueNegativeRules));
      lines.push_back("The number of false negative test samples is : " + std::to_string(nbFalseNegativeRules));
      lines.push_back("The false positive rate is : " + ((nbNegative != 0) ? std::to_string(float(nbFalsePositiveRules) / static_cast<float>(nbNegative)) : "N/A"));
      lines.push_back("The false negative rate is : " + ((nbPositive != 0) ? std::to_string(float(nbFalseNegativeRules) / static_cast<float>(nbPositive)) : "N/A"));
      lines.push_back("The precision is : " + ((nbTruePositiveRules + nbFalsePositiveRules != 0) ? std::to_string(float(nbTruePositiveRules) / static_cast<float>(nbTruePositiveRules + nbFalsePositiveRules)) : "N/A"));
      lines.push_back("The recall is : " + ((nbTruePositiveRules + nbFalseNegativeRules != 0) ? std::to_string(float(nbTruePositiveRules) / static_cast<float>(nbTruePositiveRules + nbFalseNegativeRules)) : "N/A"));
    }

    for (std::string l : lines) {
      std::cout << l << std::endl;
    }

    // Output statistics
    if (params->isStringSet(STATS_FILE)) {
      std::ofstream outputFile(params->getString(STATS_FILE));
      if (outputFile.is_open()) {
        for (const std::string &line : lines) {
          outputFile << line + "" << std::endl;
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + params->getString(STATS_FILE) + ".");
      }
    }

    // Compute rules statistics on test set
    if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
      std::string ruleFile = params->getString(GLOBAL_RULES_OUTFILE);

      if (ruleFile.substr(ruleFile.find_last_of('.') + 1) == "json") {
        Rule::toJsonFile(ruleFile, rules, decisionThreshold, positiveClassIndex);
      } else {
        std::ofstream outputFile(ruleFile);
        if (outputFile.is_open()) {
          outputFile << statsLine;

          if (decisionThreshold < 0.0) {
            outputFile << "No decision threshold is used.\n";
          } else {
            outputFile << "Decision threshold used : " + std::to_string(decisionThreshold) << "\n";
            outputFile << "Positive index class used : " << positiveClassIndex << "\n";
          }

          for (int r = 0; r < rules.size(); r++) { // For each rule
            std::vector<int> sampleIds;
            getCovering(sampleIds, rules[r], testData);
            size_t coverSize = sampleIds.size();
            double ruleFidelity = 0;   // porcentage of correct covered samples predictions with respect to the rule prediction
            double ruleAccuracy = 0;   // porcentage of correct covered samples predictions with respect to the samples true class
            double ruleConfidence = 0; // mean output prediction score of covered samples
            for (int sampleId : sampleIds) {
              int samplePred = testPreds[sampleId];
              double sampleOutputPred = 0.0;
              int classRule = rules[r].getOutputClass();
              sampleOutputPred = testOutputValuesPredictions[sampleId][classRule];
              int rulePred = rules[r].getOutputClass();
              int trueClass = testTrueClasses[sampleId];
              if (samplePred == rulePred) {
                ruleFidelity += 1;
              }
              if (samplePred == trueClass) {
                ruleAccuracy += 1;
              }
              ruleConfidence += sampleOutputPred;
            }
            if (coverSize != 0) {
              ruleFidelity /= static_cast<double>(coverSize);
              ruleAccuracy /= static_cast<double>(coverSize);
              ruleConfidence /= static_cast<double>(coverSize);
            }
            std::vector<std::string> trainStats = splitString(rules[r].toString(attributeNames, classNames), "\n");
            outputFile << "\n"
                       << "Rule " << std::to_string(r + 1) << ": " << trainStats[0] << "" << std::endl;
            outputFile << trainStats[1] << " --- Test Covering size : " << coverSize << "" << std::endl;
            if (coverSize == 0) {
              outputFile << trainStats[2] << "" << std::endl;
              outputFile << trainStats[3] << "" << std::endl;
              outputFile << trainStats[4] << "" << std::endl;
              outputFile << "" << std::endl;
            } else {
              outputFile << trainStats[2] << " --- Test Fidelity : " << formattingDoubleToString(ruleFidelity) << "" << std::endl;
              outputFile << trainStats[3] << " --- Test Accuracy : " << formattingDoubleToString(ruleAccuracy) << "" << std::endl;
              outputFile << trainStats[4] << " --- Test Confidence : " << formattingDoubleToString(ruleConfidence) << "" << std::endl;
              outputFile << "" << std::endl;
            }
          }
          outputFile.close();
        } else {
          throw CannotOpenFileError("Error : Couldn't open global rules file with statistics on test set " + params->getString(GLOBAL_RULES_OUTFILE) + ".");
        }
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

/* Exemples pour lancer le code :

./fidexGloStats --test_data_file datanorm --test_pred_file dimlp.out --test_class_file dataclass2 --global_rules_file globalRules.txt --nb_attributes 16 --nb_classes 2 --stats_file stats.txt --root_folder ../fidex/datafiles
./fidexGloStats --test_data_file datanormTest --test_pred_file dimlpDatanormTest.out --test_class_file dataclass2Test --global_rules_file globalRulesDatanorm.txt --nb_attributes 16 --nb_classes 2 --stats_file stats.txt --root_folder ../fidex/datafiles
./fidexGloStats --test_data_file covidTestData.txt --test_pred_file covidTestPred.out --test_class_file covidTestClass.txt --global_rules_file globalRulesCovid.txt --nb_attributes 16 --nb_classes 2 --stats_file globalStats.txt --root_folder ../dimlp/datafiles/covidDataset
./fidexGloStats --test_data_file spamTestData.txt --test_pred_file spamTestPred.out --test_class_file spamTestClass.txt --global_rules_file globalRulesSpam.txt --nb_attributes 16 --nb_classes 2 --stats_file globalStats.txt --root_folder ../dimlp/datafiles/spamDataset
./fidexGloStats --test_data_file isoletTestData.txt --test_pred_file isoletTestPred.out --test_class_file isoletTestClass.txt --global_rules_file globalRulesIsolet.txt --nb_attributes 16 --nb_classes 2 --stats_file globalStats.txt --root_folder ../dimlp/datafiles/isoletDataset
./fidexGloStats --test_data_file Test/X_test.txt --test_pred_file Test/pred_testV2.out --test_class_file Test/y_test.txt --global_rules_file globalRulesHAPTV2.txt --nb_attributes 16 --nb_classes 2 --stats_file globalStatsV2.txt --root_folder ../dimlp/datafiles/HAPTDataset

*/
