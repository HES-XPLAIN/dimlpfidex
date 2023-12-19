#include "fidexGloStatsFct.h"

using namespace std;

void showStatsParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;
  std::cout << "fidexGloStats -T <test data file> -P <test prediction file> -C <test true class file> ";
  std::cout << "-R <rules input file> ";
  std::cout << "-a <number of attributes>";
  std::cout << "-b <number of classes>";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "-A <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it" << std::endl;
  std::cout << "-O <stats output file>" << std::endl;
  std::cout << "-F <global rules output file with stats on test set> If you want to compute statistics of global rules on tests set" << std::endl;
  std::cout << "-r <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "-t <decision threshold for predictions, use if it was used in FidexGlo, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "-x <index of positive class sample to compute true/false positive/negative rates (None by default, put 0 for first class)>" << std::endl; // If we want to compute TP, FP, TN, FN

  std::cout << "\n-------------------------------------------------\n"
            << std::endl;
}

void getCovering(vector<int> &sampleIds, tuple<vector<tuple<int, bool, double>>, int, int, double, double> *rule, vector<vector<double>> *testValues) {
  // Get covering index samples
  int attr;
  bool ineq;
  double val;
  for (int id = 0; id < (*testValues).size(); id++) {
    bool notCovered = false;
    for (const auto &antecedent : get<0>(*rule)) { // For each antecedant
      attr = get<0>(antecedent);
      ineq = get<1>(antecedent);
      val = get<2>(antecedent);
      if (ineq == 0 && (*testValues)[id][attr] >= val) { // If the inequality is not verified
        notCovered = true;
      }
      if (ineq == 1 && (*testValues)[id][attr] < val) {
        notCovered = true;
      }
    }
    if (!notCovered) {
      sampleIds.push_back(id);
    }
  }
}

void computeTFPN(int decision, int indexPositiveClass, int testTrueClass, int &nbTruePositive, int &nbFalsePositive, int &nbTrueNegative, int &nbFalseNegative) {
  if (decision == indexPositiveClass) { // Positive prediction
    if (decision == testTrueClass) {
      nbTruePositive += 1;
    } else {
      nbFalsePositive += 1;
    }
  } else { // Negative prediction
    if (testTrueClass == indexPositiveClass) {
      nbFalseNegative += 1;
    } else {
      nbTrueNegative += 1;
    }
  }
}

int fidexGloStats(const string &command) {
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
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    size_t nbParam = commandList.size();

    // Parameters declaration

    string testDataFileTemp; // Parameter after -T
    bool testDataFileInit = false;
    string testDataFilePredTemp;
    bool testDataFilePredInit = false;
    string testDataFileTrueClassTemp;
    bool testDataFileTrueClassInit = false;

    int nb_attributes = -1;
    int nb_classes = -1;

    string rulesFileTemp;
    bool rulesFileInit = false;
    string statsFileTemp;
    bool statsFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;
    bool globalRulesStatsFileInit = false;
    string globalRulesStatsFileTemp;

    bool hasDecisionThreshold = false;
    double decisionThreshold = -1;
    bool hasIndexPositiveClass = false;
    int indexPositiveClass = -1;

    // Import parameters

    if (nbParam <= 1) {
      showStatsParams();
      return 0;
    }

    int p = 1; // We skip "fidexGlo"
    while (p < nbParam) {
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }
        char option = commandList[p - 1][1];
        const char *arg = &(commandList[p])[0];
        const char *lastArg = &(commandList[p - 1])[0];
        switch (option) { // Get letter after the -

        case 'T':
          testDataFileTemp = arg; // Parameter after -T
          testDataFileInit = true;
          break;

        case 'P':
          testDataFilePredTemp = arg;
          testDataFilePredInit = true;
          break;

        case 'C':
          testDataFileTrueClassTemp = arg;
          testDataFileTrueClassInit = true;
          break;

        case 'R':
          rulesFileTemp = arg;
          rulesFileInit = true;
          break;

        case 'a':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            nb_attributes = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested.");
          }
          break;

        case 'b':
          if (CheckPositiveInt(arg) && atoi(arg) > 0) {
            nb_classes = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", strictly positive integer requested.");
          }
          break;

        case 'O':
          statsFileTemp = arg;
          statsFileInit = true;
          break;

        case 'F':
          globalRulesStatsFileTemp = arg;
          globalRulesStatsFileInit = true;
          break;

        case 'r':
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case 'A':
          attributFileTemp = arg;
          attributFileInit = true;
          break;

        case 'S':
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case 't':
          if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
            hasDecisionThreshold = true;
            decisionThreshold = atof(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", float included in [0,1] requested.");
          }
          break;

        case 'x':
          if (CheckPositiveInt(arg)) {
            hasIndexPositiveClass = true;
            indexPositiveClass = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + string(lastArg) + ", positive integer requested.");
          }
          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option : " + string(lastArg));
        }
      }

      p++;
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *testDataFile = nullptr;
    const char *testDataFilePred = nullptr;
    const char *testDataFileTrueClass = nullptr;
    const char *rulesFile = nullptr;
    const char *statsFile = nullptr;
    const char *globalRulesStatsFile = nullptr;
    const char *consoleFile = nullptr;
    const char *attributFile = nullptr;

    string root = "";
    if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
      root = rootFolderTemp + "/";
#elif defined(_WIN32)
      root = rootFolderTemp + "\\";
#endif
    }

    if (testDataFileInit) {
      testDataFileTemp = root + testDataFileTemp;
      testDataFile = &testDataFileTemp[0];
    }

    if (testDataFilePredInit) {
      testDataFilePredTemp = root + testDataFilePredTemp;
      testDataFilePred = &testDataFilePredTemp[0];
    }

    if (testDataFileTrueClassInit) {
      testDataFileTrueClassTemp = root + testDataFileTrueClassTemp;
      testDataFileTrueClass = &testDataFileTrueClassTemp[0];
    }

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      rulesFile = &rulesFileTemp[0];
    }

    if (statsFileInit) {
      statsFileTemp = root + statsFileTemp;
      statsFile = &statsFileTemp[0];
    }

    if (globalRulesStatsFileInit) {
      globalRulesStatsFileTemp = root + globalRulesStatsFileTemp;
      globalRulesStatsFile = &globalRulesStatsFileTemp[0];
    }

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }

    if (attributFileInit) {
      attributFileTemp = root + attributFileTemp;
      attributFile = &attributFileTemp[0];
    }

    if (hasDecisionThreshold && !hasIndexPositiveClass) {
      throw CommandArgumentException("The positive class index has to be given with option -x if the decision threshold is given (-t).");
    }

    // ----------------------------------------------------------------------

    if (!testDataFileInit) {
      throw CommandArgumentException("The test data file has to be given with option -T.");
    }
    if (!testDataFilePredInit) {
      throw CommandArgumentException("The test predictions data file has to be given with option -P.");
    }
    if (!testDataFileTrueClassInit) {
      throw CommandArgumentException("The test true class data file has to be given with option -T.");
    }
    if (!rulesFileInit) {
      throw CommandArgumentException("The rules file has to be given with option -R.");
    }
    if (nb_attributes == -1) {
      throw CommandArgumentException("The number of attributes has to be given with option -a.");
    }
    if (nb_classes == -1) {
      throw CommandArgumentException("The number of classes has to be given with option -b.");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    std::cout << "Import data..." << endl
              << endl;

    // Get test data

    std::unique_ptr<DataSetFid> testDatas(new DataSetFid("testDatas from FidexGloStats", testDataFile, testDataFilePred, nb_attributes, nb_classes, decisionThreshold, indexPositiveClass, testDataFileTrueClass));

    vector<vector<double>> *testData = testDatas->getDatas();
    vector<int> *testPreds = testDatas->getPredictions();
    vector<int> *testTrueClasses = testDatas->getClasses();

    vector<vector<double>> *testOutputValuesPredictions = nullptr;
    testOutputValuesPredictions = testDatas->getOutputValuesPredictions();
    int nbTestData = testDatas->getNbSamples();

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (attributFileInit) {
      testDatas->setAttributes(attributFile, nb_attributes, nb_classes);
      attributeNames = (*testDatas->getAttributeNames());
      hasClassNames = testDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = (*testDatas->getClassNames());
      }
    }
    // Get rules

    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;                                                            // Lines for the output stats
    vector<string> statsLines;
    lines.emplace_back("Global statistics of the rule set : "); // Lines for the output stats
    vector<string> stringRules;
    getRules(rules, statsLines, stringRules, rulesFile, attributFileInit, attributeNames, hasClassNames, classNames);
    for (auto l : statsLines) {
      lines.emplace_back(l);
    }
    std::cout << "Data imported." << endl
              << endl;

    // Compute global statistics on test set

    std::cout << "Compute statistics..." << endl
              << endl;
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
    vector<double> testValues;
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
      testValues = (*testData)[t];
      testPred = (*testPreds)[t];
      testTrueClass = (*testTrueClasses)[t];
      if (hasIndexPositiveClass) {
        if (testTrueClass == indexPositiveClass) {
          nbPositive += 1;
        } else {
          nbNegative += 1;
        }
      }

      if (testPred == testTrueClass) {
        modelAccuracy++;
      }
      if (hasIndexPositiveClass) {
        computeTFPN(testPred, indexPositiveClass, testTrueClass, nbTruePositive, nbFalsePositive, nbTrueNegative, nbFalseNegative);
      }

      // Find rules activated by this sample
      bool noCorrectRuleWithAllSameClass = false; // If there is no correct rule activated but all rules have same class
      vector<int> activatedRules;
      getActivatedRules(activatedRules, &rules, &testValues);

      // Check which rules are correct
      vector<int> correctRules;
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
          if (get<2>(rules[v]) == testPred) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          }
        }
        if (correctRules.empty()) { // If there is no correct rule
          meanNbWrongActivatedRules += static_cast<double>(activatedRules.size());

          int ancientClass = get<2>(rules[activatedRules[0]]);
          bool allSameClass = true; // Check if all the rules choose the same class
          for (int v : activatedRules) {
            if (get<2>(rules[v]) != ancientClass) {
              allSameClass = false;
              break;
            }
          }
          if (allSameClass) {
            explainabilityTotal++; // If all decisions are the same, we have an explanation
            int decision = get<2>(rules[activatedRules[0]]);
            if (decision == testTrueClass) { // If those decisions are the true class, this is accurate
              accuracy++;
            }
            // The rules' decision is different from the model's
            noCorrectRuleWithAllSameClass = true;
            if (hasIndexPositiveClass) {
              computeTFPN(decision, indexPositiveClass, testTrueClass, nbTruePositiveRules, nbFalsePositiveRules, nbTrueNegativeRules, nbFalseNegativeRules);
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

      if (!noCorrectRuleWithAllSameClass && hasIndexPositiveClass) { // The rules' decision is the same as the model's, if we can find a correct rule or if we need to compute Fidex
        computeTFPN(testPred, indexPositiveClass, testTrueClass, nbTruePositiveRules, nbFalsePositiveRules, nbTrueNegativeRules, nbFalseNegativeRules);
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
    lines.push_back("The global rule fidelity rate is : " + std::to_string(fidelity));
    lines.push_back("The global rule accuracy is : " + std::to_string(accuracy));
    lines.push_back("The explainability rate (when we can find one or more rules, either correct ones or activated ones which all agree on the same class) is : " + std::to_string(explainabilityRate));
    lines.push_back("The default rule rate (when we can't find any rule activated for a sample) is : " + std::to_string(defaultRuleRate));
    lines.push_back("The mean number of correct(fidel) activated rules per sample is : " + std::to_string(meanNbCorrectActivatedRules));
    lines.push_back("The mean number of wrong(not fidel) activated rules per sample is : " + std::to_string(meanNbWrongActivatedRules));
    lines.push_back("The model test accuracy is : " + std::to_string(modelAccuracy));
    lines.push_back("The model test accuracy when rules and model agree is : " + std::to_string(accuracyWhenRulesAndModelAgree));
    lines.push_back("The model test accuracy when activated rules and model agree is : " + std::to_string(accuracyWhenActivatedRulesAndModelAgree));

    if (hasIndexPositiveClass) {
      if (hasClassNames) {
        lines.push_back("\nWith positive class " + classNames[indexPositiveClass] + " :");
      } else {
        lines.push_back("\nWith positive class " + std::to_string(indexPositiveClass) + " :");
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

    for (string l : lines) {
      cout << l << endl;
    }

    // Output statistics
    if (statsFileInit) {
      ofstream outputFile(statsFile);
      if (outputFile.is_open()) {
        for (const string &line : lines) {
          outputFile << line + "" << std::endl;
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + std::string(statsFile) + ".");
      }
    }

    // Compute rules statistics on test set
    if (globalRulesStatsFileInit) {
      ofstream outputFile(globalRulesStatsFile);
      if (outputFile.is_open()) {
        for (string l : statsLines) {
          outputFile << l;
        }
        if (!attributFileInit) {
          outputFile << "The name of the attributes and classes are not specified" << std::endl;
        } else if (!hasClassNames) {
          outputFile << "The name of the attributes is specified" << std::endl;
        } else {
          outputFile << "The name of the attributes and classes are specified" << std::endl;
        }

        for (int r = 0; r < rules.size(); r++) { // For each rule
          vector<int> sampleIds;
          getCovering(sampleIds, &rules[r], testData);
          size_t coverSize = sampleIds.size();
          double ruleFidelity = 0;   // porcentage of correct covered samples predictions with respect to the rule prediction
          double ruleAccuracy = 0;   // porcentage of correct covered samples predictions with respect to the samples true class
          double ruleConfidence = 0; // mean output prediction score of covered samples
          for (int sampleId : sampleIds) {
            int samplePred = (*testPreds)[sampleId];
            double sampleOutputPred = 0.0;
            int classRule = get<2>(rules[r]);
            sampleOutputPred = (*testOutputValuesPredictions)[sampleId][classRule];
            int rulePred = get<2>(rules[r]);
            int trueClass = (*testTrueClasses)[sampleId];
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
          vector<string> trainStats = splitString(stringRules[r], "\n");
          outputFile << "\n"
                     << trainStats[0] << "" << std::endl;
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
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + std::string(statsFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    cerr << msg << endl;
    return -1;
  }

  return 0;
}

/* Exemples pour lancer le code :

./fidexGloStats -T datanorm -P dimlp.out -C dataclass2 -R globalRules.txt -a 16 -b 2 -O stats.txt -S ../fidexGlo/datafiles
./fidexGloStats -T datanormTest -P dimlpDatanormTest.out -C dataclass2Test -R globalRulesDatanorm.txt -a 16 -b 2 -O stats.txt -S ../fidexGlo/datafiles
./fidexGloStats -T covidTestData.txt -P covidTestPred.out -C covidTestClass.txt -R globalRulesCovid.txt -a 16 -b 2 -O globalStats.txt -S ../dimlp/datafiles/covidDataset
./fidexGloStats -T spamTestData.txt -P spamTestPred.out -C spamTestClass.txt -R globalRulesSpam.txt -a 16 -b 2 -O globalStats.txt -S ../dimlp/datafiles/spamDataset
./fidexGloStats -T isoletTestData.txt -P isoletTestPred.out -C isoletTestClass.txt -R globalRulesIsolet.txt -a 16 -b 2 -O globalStats.txt -S ../dimlp/datafiles/isoletDataset
./fidexGloStats -T Test/X_test.txt -P Test/pred_testV2.out -C Test/y_test.txt -R globalRulesHAPTV2.txt -a 16 -b 2 -O globalStatsV2.txt -S ../dimlp/datafiles/HAPTDataset

*/
