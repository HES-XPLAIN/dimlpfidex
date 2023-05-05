#include "dataSet.h"
#include "getRulesFun.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <tuple>
#include <vector>

using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidexGloRules -T <test data file> -P <test prediction file> -C <test true class file> ";
  std::cout << "-R <rules input file> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-O <stats output file>";

  std::cout << "\n-------------------------------------------------\n\n";
}

int main(int nbParam, char **param) {

  try {

    float temps;
    clock_t t1, t2;

    t1 = clock();

    // Import parameters
    if (nbParam == 1) {
      showParams();
      return -1;
    }

    // Parameters declaration

    char *testDataFile = 0; // Parameter after -T
    bool testDataFileInit = false;
    char *testDataFilePred = 0;
    bool testDataFilePredInit = false;
    char *testDataFileTrueClass = 0;
    bool testDataFileTrueClassInit = false;

    char *rulesFile = 0;
    bool rulesFileInit = false;
    char *statsFile = 0;
    bool statsFileInit = false;

    DataSetFid *testDatas;

    // Import parameters

    for (int p = 1; p < nbParam; p++) { // We skip "fidexGlo"
      if (*param[p] == '-') {
        p++;

        if (p >= nbParam) {
          throw std::runtime_error("Missing something at the end of the command.");
        }

        switch (*(param[p - 1] + 1)) { // Get letter after the -

        case 'T':
          testDataFile = param[p]; // Parameter after -T
          testDataFileInit = true;
          break;

        case 'P':
          testDataFilePred = param[p];
          testDataFilePredInit = true;
          break;

        case 'C':
          testDataFileTrueClass = param[p];
          testDataFileTrueClassInit = true;
          break;

        case 'R':
          rulesFile = param[p];
          rulesFileInit = true;
          break;

        case 'O':
          statsFile = param[p];
          statsFileInit = true;
          break;

        default: // If we put another -X option
          throw std::runtime_error("Illegal option : " + string(param[p - 1]));
        }
      }
    }

    if (!testDataFileInit) {
      throw std::runtime_error("The test data file has to be given with option -T");
    }
    if (!testDataFilePredInit) {
      throw std::runtime_error("The test predictions data file has to be given with option -P");
    }
    if (!testDataFileTrueClassInit) {
      throw std::runtime_error("The test true class data file has to be given with option -T");
    }
    if (!rulesFileInit) {
      throw std::runtime_error("The rules file has to be given with option -R");
    }

    std::cout << "Import data..." << endl
              << endl;

    // Get test data

    testDatas = new DataSetFid(testDataFile, testDataFilePred, testDataFileTrueClass);

    vector<vector<double>> *testData = testDatas->getDatas();
    vector<int> *testPreds = testDatas->getPredictions();
    vector<int> *testTrueClasses = testDatas->getTrueClasses();

    const int nbTestData = (*testData).size();
    if ((*testPreds).size() != nbTestData || (*testTrueClasses).size() != nbTestData) {
      throw std::runtime_error("All the test files need to have the same amount of datas");
    }
    int nbTestAttributs = (*testData)[0].size();

    // Get rules

    vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> rules; // A rule is on the form : <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
    vector<string> lines;                                                            // Lines for the output stats
    vector<string> stringRules;
    getRules(rules, lines, stringRules, rulesFile, nbTestAttributs);

    std::cout << "Data imported..." << endl
              << endl;

    std::cout << "Compute statistics..." << endl
              << endl;
    double fidelity = 0;       // Global rules fidelity rate (wrong only if there is activated rules but no correct one(with respect to prediction))
    double accuracy = 0;       // Global rules accuracy (true if there is at least one fidel activated rule and the model is right, or if there is no activated rules and the model is right or if all the activated rules decide the same class and this class is the true one)
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
      if (activatedRules.size() == 0) { // If there is no activated rule
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
        if (correctRules.size() == 0) { // If there is no correct rule
          meanNbWrongActivatedRules += activatedRules.size();

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

          fidelity++;                      // It is true to the model because we found a correct activated rule
          if (testPred == testTrueClass) { // If the model is right, it's true for the accuracy because we found a fidel rule
            accuracy++;
          }
          explainabilityTotal++;
          nbActivatedRulesAndModelAgree++;
          nbFidelRules++;
          if (testPred == testTrueClass) {
            accuracyWhenActivatedRulesAndModelAgree++; // It is true for the accuracy because prediction is correct
            accuracyWhenRulesAndModelAgree++;          // It is true for the accuracy because prediction is correct
          }
          meanNbCorrectActivatedRules += correctRules.size();
          meanNbWrongActivatedRules += (activatedRules.size() - correctRules.size());
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
    lines.push_back("The global rules accuracy is : " + std::to_string(accuracy));
    lines.push_back("The explainability rate (when we can find a rule) is : " + std::to_string(explainabilityRate));
    lines.push_back("The default rule rate (when we can't find a rule) is : " + std::to_string(defaultRuleRate));
    lines.push_back("The mean number of correct(fidel) activation rules per sample is : " + std::to_string(meanNbCorrectActivatedRules));
    lines.push_back("The mean number of wrong(not fidel) activation rules per sample is : " + std::to_string(meanNbWrongActivatedRules));
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
        for (int l = 0; l < lines.size(); l++) {
          outputFile << lines[l] + "\n";
        }
        outputFile.close();
      } else {
        throw std::runtime_error("Error : Couldn't open explanation extraction file " + std::string(statsFile) + ".");
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::printf("\nFull execution time = %f sec\n", temps);
  } catch (const char *msg) {
    std::printf(msg);
    cerr << msg << endl;
  }
}

// Exemple : fidexGloStats -T datafiles/datanorm -P datafiles/dimlp.out -C datafiles/dataclass2 -R datafiles/globalRules.txt -O datafiles/stats.txt
