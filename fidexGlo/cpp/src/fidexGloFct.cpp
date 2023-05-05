#include "fidexGloFct.h"

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidexGlo -S <test sample(s) data file> ";
  std::cout << "-R <rules input file> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-O <Rule output file>";

  std::cout << "\n-------------------------------------------------\n\n";
}

int fidexGlo(string command) {

  try {

    float temps;
    clock_t t1, t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    int nbParam = commandList.size();

    // Import parameters
    if (nbParam == 1) {
      showParams();
      return -1;
    }

    // Parameters declaration

    char *testSamplesDataFile = 0;
    bool testSamplesDataFileInit = false;
    char *rulesFile = 0;
    bool rulesFileInit = false;
    char *explanationFile = 0;
    bool explanationFileInit = false;

    // Import parameters

    for (int p = 1; p < nbParam; p++) { // We skip "fidexGlo"
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw std::runtime_error("Missing something at the end of the command.");
        }

        switch (commandList[p - 1][1]) { // Get letter after the -

        case 'S':
          testSamplesDataFile = &(commandList[p])[0];
          testSamplesDataFileInit = true;
          break;

        case 'R':
          rulesFile = &(commandList[p])[0];
          rulesFileInit = true;
          break;

        case 'O':
          explanationFile = &(commandList[p])[0];
          explanationFileInit = true;
          break;

        default: // If we put another -X option
          throw std::runtime_error("Illegal option : " + string(&(commandList[p - 1])[0]));
        }
      }
    }

    if (!testSamplesDataFileInit) {
      throw std::runtime_error("The test samples data file has to be given with option -S");
    }
    if (!rulesFileInit) {
      throw std::runtime_error("The rules file has to be given with option -R");
    }

    std::cout << "Import datas..." << endl
              << endl;

    // Get test data

    vector<vector<double>> testSamplesValues;
    vector<vector<double>> testSamplesOutputValuesPredictions;
    vector<int> testSamplesPreds;
    vector<double> testSampleValues;
    vector<double> testSampleOutputValuesPredictions;
    int testSamplePred;
    fstream testData;
    testData.open(testSamplesDataFile, ios::in); // Read data file
    if (testData.fail()) {
      throw std::runtime_error("Error : file " + std::string(testSamplesDataFile) + " not found");
    }
    string line;
    bool firstLine = true;
    while (!testData.eof()) {
      getline(testData, line);
      if (line.length() != 0) {
        std::stringstream myLine(line);
        double value;
        testSampleValues.clear();
        while (myLine >> value) {
          testSampleValues.push_back(value);
        }
        testSamplesValues.push_back(testSampleValues);
      } else if (firstLine) {
        throw std::runtime_error("Error : in file " + std::string(testSamplesDataFile) + ", first line is empty");
      } else {
        while (!testData.eof()) {
          getline(testData, line);
          if (line.length() != 0) {
            throw std::runtime_error("Error : file " + std::string(testSamplesDataFile) + " is not on good format, there is more than one empty line between 2 samples");
          }
        }
        break; // If there is just an empty line at the end of the file
      }
      if (!testData.eof()) {
        getline(testData, line);
        if (line.length() != 0) {
          std::stringstream myLine(line);
          double value;
          testSampleOutputValuesPredictions.clear();
          while (myLine >> value) {
            testSampleOutputValuesPredictions.push_back(value);
          }
          testSamplesOutputValuesPredictions.push_back(testSampleOutputValuesPredictions);
          testSamplePred = std::max_element(testSampleOutputValuesPredictions.begin(), testSampleOutputValuesPredictions.end()) - testSampleOutputValuesPredictions.begin();
          testSamplesPreds.push_back(testSamplePred);
        } else {
          throw std::runtime_error("Error : file " + std::string(testSamplesDataFile) + " has not enough test prediction data");
        }
      } else {
        throw std::runtime_error("Error : file " + std::string(testSamplesDataFile) + " has not enough test prediction data");
      }
      if (!testData.eof()) {
        getline(testData, line);
        if (line.length() != 0) {
          throw std::runtime_error("Error : in file " + std::string(testSamplesDataFile) + ", you need to have empty lines between samples");
        }
      }
      firstLine = false;
    }
    testData.close(); // close data file

    int nbSamples = testSamplesValues.size();
    int nbTestAttributs = testSamplesValues[0].size();
    const int nbClass = testSamplesOutputValuesPredictions[0].size();

    for (int s = 0; s < nbSamples; s++) {
      if (testSamplesValues[s].size() != nbTestAttributs) {
        throw std::runtime_error("Error : in file " + std::string(testSamplesDataFile) + ", all test datas need to have the same number of variables");
      }
    }

    for (int s = 0; s < nbSamples; s++) {
      if (testSamplesOutputValuesPredictions[s].size() != nbClass) {
        throw std::runtime_error("Error : in file " + std::string(testSamplesDataFile) + ", all test datas need to have the same number of prediction values");
      }
    }

    // Get rules

    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;                                                            // Lines for the output stats
    vector<string> stringRules;
    getRules(rules, lines, stringRules, rulesFile, nbTestAttributs);

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
      if (activatedRules.size() == 0) { // If there is no activated rule
        cout << "\nThere is no class activated" << endl;
        lines.push_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
        lines.push_back("We choose the model prediction.");
        lines.push_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
      } else { // There is some activated rules
        for (int v : activatedRules) {
          if (get<2>(rules[v]) == testSamplesPreds[currentSample]) { // Check if the class of the rule is the predicted one
            correctRules.push_back(v);
          } else {
            notcorrectRules.push_back(v);
          }
        }
        if (correctRules.size() == 0) { // If there is no correct rule
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
              lines.push_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found " + std::to_string(activatedRules.size()) + " rules with class " + std::to_string(ancientClass) + " :\n");
            } else {
              lines.push_back("We didn't found any rule with same prediction as the model (class " + std::to_string(testSamplesPreds[currentSample]) + "), but we found 1 rule with class " + std::to_string(ancientClass) + " :\n");
            }
            for (int v = 0; v < activatedRules.size(); v++) {
              lines.push_back("R" + std::to_string(v + 1) + ": " + stringRules[activatedRules[v]]);
            }
          } else {
            cout << "\nThere is no correct rule for this sample." << endl;
            lines.push_back("We couldn't find any global explanation for this sample."); // There is no explanation, we choose the model decision
            lines.push_back("We choose the model prediction.");
            lines.push_back("The predicted class is " + std::to_string(testSamplesPreds[currentSample]));
          }

        } else { // There is an explanation which is caracterised by the correct rules
          if (correctRules.size() > 1) {
            lines.push_back("We have found " + std::to_string(correctRules.size()) + " global rules explaining the model prediction :\n"); // There is no explanation, we choose the model decision
          } else {
            lines.push_back("We have found 1 global rule explaining the model prediction :\n"); // There is no explanation, we choose the model decision
          }
          for (int c = 0; c < correctRules.size(); c++) {
            lines.push_back("R" + std::to_string(c + 1) + ": " + stringRules[correctRules[c]]);
          }
        }
      }
      if (!notShowUncorrectRules) {
        if (notcorrectRules.size() != 0) {
          lines.push_back("\nActivated rules without correct decision class :");
          for (int n = 0; n < notcorrectRules.size(); n++) {
            lines.push_back("F" + std::to_string(n + 1) + ": " + stringRules[notcorrectRules[n]]);
          }
        } else {
          lines.push_back("\nThere is no uncorrect rules.");
        }
      }

      lines.push_back("\n--------------------------------------------------------------------\n");
    }

    // Show result on consol
    for (string l : lines) {
      cout << l << endl;
    }

    // Output global explanation result
    if (explanationFileInit) {
      ofstream outputFile(explanationFile);
      if (outputFile.is_open()) {
        for (int l = 0; l < lines.size(); l++) {
          outputFile << lines[l] + "\n";
        }
        outputFile.close();
      } else {
        throw std::runtime_error("Error : Couldn't open explanation extraction file " + std::string(explanationFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::printf("\nFull execution time = %f sec\n", temps);

  } catch (const char *msg) {
    std::printf(msg);
    cerr << msg << endl;
  }

  return 0;
}

// Exemple : .\fidexGlo.exe -S ../fidexGlo/datafiles/testSampleData -R ../fidexGlo/datafiles/globalRules.txt -O ../fidexGlo/datafiles/explanation.txt
