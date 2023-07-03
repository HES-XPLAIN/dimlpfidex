#include "fidexGloFct.h"

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";

  std::cout << "fidexGlo -S <test sample(s) data file with data and prediction(if no -p)> ";
  std::cout << "-R <rules input file> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-F <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  std::cout << "-A <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it\n";
  std::cout << "-p <test prediction file>, -S needs to have only test datas\n";
  std::cout << "-O <Rule output file>\n";
  std::cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file

  std::cout << "\n-------------------------------------------------\n\n";
}

int fidexGlo(const string &command) {

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
    std::size_t nbParam = commandList.size();

    // Parameters declaration

    string testSamplesDataFileTemp;
    bool testSamplesDataFileInit = false;
    string testSamplesPredFileTemp; // Test predictions
    bool testSamplesPredFileInit = false;
    string rulesFileTemp;
    bool rulesFileInit = false;
    string explanationFileTemp;
    bool explanationFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
    string rootFolderTemp;
    bool rootFolderInit = false;
    string attributFileTemp; // attribut file
    bool attributFileInit = false;

    // Import parameters

    if (nbParam <= 1) {
      showParams();
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

        switch (option) { // Get letter after the -
        case 'S':
          testSamplesDataFileTemp = arg;
          testSamplesDataFileInit = true;
          break;

        case 'p':
          testSamplesPredFileTemp = arg;
          testSamplesPredFileInit = true;
          break;

        case 'R':
          rulesFileTemp = arg;
          rulesFileInit = true;
          break;

        case 'O':
          explanationFileTemp = arg;
          explanationFileInit = true;
          break;

        case 'r':
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case 'A':
          attributFileTemp = arg;
          attributFileInit = true;
          break;

        case 'F':
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option: " + std::string(&option, 1));
        }
      }

      p++;
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *testSamplesDataFile = nullptr;
    const char *testSamplesPredFile = nullptr;
    const char *rulesFile = nullptr;
    const char *explanationFile = nullptr;
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

    if (testSamplesDataFileInit) {
      testSamplesDataFileTemp = root + testSamplesDataFileTemp;
      testSamplesDataFile = &testSamplesDataFileTemp[0];
    }

    if (testSamplesPredFileInit) {
      testSamplesPredFileTemp = root + testSamplesPredFileTemp;
      testSamplesPredFile = &testSamplesPredFileTemp[0];
    }

    if (rulesFileInit) {
      rulesFileTemp = root + rulesFileTemp;
      rulesFile = &rulesFileTemp[0];
    }

    if (explanationFileInit) {
      explanationFileTemp = root + explanationFileTemp;
      explanationFile = &explanationFileTemp[0];
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
    if (!testSamplesDataFileInit) {
      throw CommandArgumentException("The test samples data file has to be given with option -S");
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

    std::cout << "Import datas..." << endl
              << endl;

    // Get test data
    vector<int> testSamplesPreds;
    vector<vector<double>> testSamplesValues;
    vector<vector<double>> testSamplesOutputValuesPredictions;

    if (!testSamplesPredFileInit) { // If we have only one test data file with data and prediction

      vector<double> testSampleValues;
      vector<double> testSampleOutputValuesPredictions;
      int testSamplePred;
      fstream testData;
      testData.open(testSamplesDataFile, ios::in); // Read data file
      if (testData.fail()) {
        throw FileNotFoundError("Error : file " + std::string(testSamplesDataFile) + " not found");
      }
      string line;
      bool firstLine = true;
      while (!testData.eof()) {
        getline(testData, line);
        if (!checkStringEmpty(line)) {
          std::stringstream myLine(line);
          double value;
          testSampleValues.clear();
          while (myLine >> value) {
            testSampleValues.push_back(value);
          }
          testSamplesValues.push_back(testSampleValues);
        } else if (firstLine) {
          throw FileFormatError("Error : in file " + std::string(testSamplesDataFile) + ", first line is empty");
        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              cout << line << endl;
              throw FileFormatError("Error : file " + std::string(testSamplesDataFile) + " is not on good format, there is more than one empty line between 2 samples");
            }
          }
          break; // If there is just an empty line at the end of the file
        }
        if (testData.eof()) {
          throw FileContentError("Error : file " + std::string(testSamplesDataFile) + " has not enough prediction data");
        }
        getline(testData, line);
        if (!checkStringEmpty(line)) {
          std::stringstream myLine(line);
          double value;
          testSampleOutputValuesPredictions.clear();
          while (myLine >> value) {
            testSampleOutputValuesPredictions.push_back(value);
          }
          testSamplesOutputValuesPredictions.push_back(testSampleOutputValuesPredictions);
          testSamplePred = static_cast<int>(std::max_element(testSampleOutputValuesPredictions.begin(), testSampleOutputValuesPredictions.end()) - testSampleOutputValuesPredictions.begin());
          testSamplesPreds.push_back(testSamplePred);

        } else {
          while (!testData.eof()) {
            getline(testData, line);
            if (!checkStringEmpty(line)) {
              throw FileFormatError("Error : file " + std::string(testSamplesDataFile) + " is not on good format, there is empty lines inbetween data");
            }
          }
          throw FileContentError("Error : file " + std::string(testSamplesDataFile) + " has not enough prediction data");
        }
        if (!testData.eof()) {
          getline(testData, line);
          if (!checkStringEmpty(line)) {
            throw FileFormatError("Error : in file " + std::string(testSamplesDataFile) + ", you need to have empty lines between samples");
          }
        }
        firstLine = false;
      }
      testData.close(); // close data file
    } else {            // We have a different file for test predictions
      std::unique_ptr<DataSetFid> testDatas(new DataSetFid(testSamplesDataFile, testSamplesPredFile));
      testSamplesValues = (*testDatas->getDatas());
      testSamplesPreds = (*testDatas->getPredictions());
      testSamplesOutputValuesPredictions = (*testDatas->getOutputValuesPredictions());

      // Check if there is good number of lines
      if (testSamplesPreds.size() != testSamplesValues.size()) {
        throw FileContentError("Error : in file " + std::string(testSamplesPredFile) + ", you need to specify as many predictions as there is datas");
      }
    }

    size_t nbSamples = testSamplesValues.size();
    size_t nbTestAttributs = testSamplesValues[0].size();
    size_t nbClass = testSamplesOutputValuesPredictions[0].size();

    for (int spl = 0; spl < nbSamples; spl++) {
      if (testSamplesValues[spl].size() != nbTestAttributs) {
        throw FileContentError("Error : in file " + std::string(testSamplesDataFile) + ", all test datas need to have the same number of variables");
      }
    }

    for (int spl = 0; spl < nbSamples; spl++) {
      if (testSamplesOutputValuesPredictions[spl].size() != nbClass) {
        throw FileContentError("Error : in file " + std::string(testSamplesDataFile) + ", all test datas need to have the same number of prediction values");
      }
    }

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

    std::cout << "Files imported" << endl
              << endl;

    std::cout << "Find explanation for each sample..." << endl
              << endl;

    // we search explanation for each sample

    for (int currentSample = 0; currentSample < nbSamples; currentSample++) {

      lines.push_back("Explanation for sample " + std::to_string(currentSample) + " :\n");

      // Find rules activated by this sample
      vector<int> activatedRules;
      getActivatedRules(activatedRules, &rules, &testSamplesValues[currentSample]);

      // Check which rules are correct
      vector<int> correctRules;
      vector<int> notcorrectRules;
      bool notShowUncorrectRules = false;
      if (activatedRules.empty()) { // If there is no activated rule
        cout << "\nThere is no class activated" << endl;
        lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
        lines.emplace_back("We choose the model prediction.");
        lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
      } else { // There is some activated rules
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
          if (allSameClass) {
            notShowUncorrectRules = true;
            if (activatedRules.size() > 1) {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
            } else {
              lines.emplace_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.emplace_back("R" + std::to_string(v + 1) + ": " + stringRules[activatedRules[v]]);
            }
          } else {
            cout << "\nThere is no correct rule for this sample." << endl;
            lines.emplace_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
            lines.emplace_back("We choose the model prediction.");
            lines.emplace_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
          }

        } else { // There is an explanation which is caracterised by the correct rules
          if (correctRules.size() > 1) {
            lines.emplace_back("We have found " + std::to_string(correctRules.size()) + " global rules explaining the model prediction :\n"); // There is no explanation, we choose the model decision
          } else {
            lines.emplace_back("We have found 1 global rule explaining the model prediction :\n"); // There is no explanation, we choose the model decision
          }
          for (int c = 0; c < correctRules.size(); c++) {
            lines.emplace_back("R" + std::to_string(c + 1) + ": " + stringRules[correctRules[c]]);
          }
        }
      }
      if (!notShowUncorrectRules) {
        if (!notcorrectRules.empty()) {
          lines.emplace_back("\nActivated rules without correct decision class :");
          for (int n = 0; n < notcorrectRules.size(); n++) {
            lines.emplace_back("F" + std::to_string(n + 1) + ": " + stringRules[notcorrectRules[n]]);
          }
        } else {
          lines.emplace_back("\nThere is no uncorrect rules.");
        }
      }

      lines.emplace_back("\n--------------------------------------------------------------------\n");
    }

    // Show result on consol
    for (string l : lines) {
      cout << l << endl;
    }

    // Output global explanation result
    if (explanationFileInit) {
      ofstream outputFile(explanationFile);
      if (outputFile.is_open()) {
        for (const auto &line : lines) {
          outputFile << line << "\n";
        }
        outputFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open explanation extraction file " + std::string(explanationFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec\n";

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const char *msg) {
    cerr << msg << endl;
    return -1;
  }

  return 0;
}

// Exemple pour lancer le code : ./fidexGlo -S testSampleData -R globalRulesDatanorm.txt -O explanation.txt -F fidexGlo/datafiles
