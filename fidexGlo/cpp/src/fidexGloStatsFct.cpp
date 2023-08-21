#include "fidexGloStatsFct.h"

using namespace std;

void showStatsParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidexGloStats -T <test data file> -P <test prediction file> -C <test true class file> ";
  std::cout << "-R <rules input file> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  std::cout << "-A <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it\n";
  std::cout << "-O <stats output file>\n";
  std::cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file

  std::cout << "\n-------------------------------------------------\n\n";
}

int fidexGloStats(const string &command) {

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

        case 'O':
          statsFileTemp = arg;
          statsFileInit = true;
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

    if (consoleFileInit) {
      consoleFileTemp = root + consoleFileTemp;
      consoleFile = &consoleFileTemp[0];
    }

    if (attributFileInit) {
      attributFileTemp = root + attributFileTemp;
      attributFile = &attributFileTemp[0];
    }

    // ----------------------------------------------------------------------

    if (!testDataFileInit) {
      throw CommandArgumentException("The test data file has to be given with option -T");
    }
    if (!testDataFilePredInit) {
      throw CommandArgumentException("The test predictions data file has to be given with option -P");
    }
    if (!testDataFileTrueClassInit) {
      throw CommandArgumentException("The test true class data file has to be given with option -T");
    }
    if (!rulesFileInit) {
      throw CommandArgumentException("The rules file has to be given with option -R");
    }

    // ----------------------------------------------------------------------

    // Get console results to file
    std::ofstream ofs;
    std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    std::cout << "Import data..." << endl
              << endl;

    // Get test data

    std::unique_ptr<DataSetFid> testDatas(new DataSetFid(testDataFile, testDataFilePred, testDataFileTrueClass));

    vector<vector<double>> *testData = testDatas->getDatas();
    vector<int> *testPreds = testDatas->getPredictions();
    vector<int> *testTrueClasses = testDatas->getTrueClasses();
    const auto nbClass = testDatas->getNbClasses();

    const auto nbTestData = static_cast<int>((*testData).size());
    if ((*testPreds).size() != nbTestData || (*testTrueClasses).size() != nbTestData) {
      throw FileFormatError("All the test files need to have the same amount of datas");
    }
    auto nbTestAttributs = static_cast<int>((*testData)[0].size());
    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (attributFileInit) {
      std::unique_ptr<Attribute> attributesData(new Attribute(attributFile));
      attributeNames = (*attributesData->getAttributes());
      if (attributeNames.size() < nbTestAttributs) {
        throw FileContentError("Error : in file " + std::string(attributFile) + ", there is not enough attribute names");
      } else if (attributeNames.size() == nbTestAttributs) {
        hasClassNames = false;
      } else if (attributeNames.size() != nbTestAttributs + nbClass) {
        throw FileContentError("Error : in file " + std::string(attributFile) + ", there is not the good amount of attribute and class names");
      } else {
        hasClassNames = true;
        auto firstEl = attributeNames.end() - nbClass;
        auto lastEl = attributeNames.end();
        classNames.insert(classNames.end(), firstEl, lastEl);
        attributeNames.erase(firstEl, lastEl);
      }
    }

    // Get rules

    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;                                                            // Lines for the output stats
    vector<string> stringRules;
    getRules(rules, lines, stringRules, rulesFile, attributFileInit, attributeNames, hasClassNames, classNames);

    std::cout << "Data imported..." << endl
              << endl;

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

    for (int t = 0; t < nbTestData; t++) { // For each test value
      testValues = (*testData)[t];
      testPred = (*testPreds)[t];
      testTrueClass = (*testTrueClasses)[t];
      if (testPred == testTrueClass) {
        modelAccuracy++;
      }

      // Find rules activated by this sample
      vector<int> activatedRules;
      getActivatedRules(activatedRules, &rules, &testValues);

      // Check which rules are correct
      vector<int> correctRules;
      if (activatedRules.empty()) { // If there is no activated rule
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
            explainabilityTotal++;                                   // If all decisions are the same, we have an explanation
            if (get<2>(rules[activatedRules[0]]) == testTrueClass) { // If those decisions are the true class, this is accurate
              accuracy++;
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

    for (string l : lines) {
      cout << l << endl;
    }

    // Output statistics
    if (statsFileInit) {
      ofstream outputFile(statsFile);
      if (outputFile.is_open()) {
        for (const string &line : lines) {
          outputFile << line + "\n";
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + std::string(statsFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec\n";

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    cerr << msg << endl;
  }

  return 0;
}

/* Exemples pour lancer le code :

./fidexGloStats -T datanorm -P dimlp.out -C dataclass2 -R globalRules.txt -O stats.txt -S ../fidexGlo/datafiles
./fidexGloStats -T datanormTest -P dimlpDatanormTest.out -C dataclass2Test -R globalRulesDatanorm.txt -O stats.txt -S ../fidexGlo/datafiles
./fidexGloStats -T covidTestData.txt -P covidTestPred.out -C covidTestClass.txt -R globalRulesCovid.txt -O globalStats.txt -S ../dimlp/datafiles/covidDataset
./fidexGloStats -T spamTestData.txt -P spamTestPred.out -C spamTestClass.txt -R globalRulesSpam.txt -O globalStats.txt -S ../dimlp/datafiles/spamDataset
./fidexGloStats -T isoletTestData.txt -P isoletTestPred.out -C isoletTestClass.txt -R globalRulesIsolet.txt -O globalStats.txt -S ../dimlp/datafiles/isoletDataset
./fidexGloStats -T Test/X_test.txt -P Test/pred_testV2.out -C Test/y_test.txt -R globalRulesHAPTV2.txt -O globalStatsV2.txt -S ../dimlp/datafiles/HAPTDataset

*/
