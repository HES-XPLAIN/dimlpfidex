/*
File to check if cover size is right for each rule.
*/

#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>
using namespace std;

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidex -T <train dataset file> ";
  std::cout << "-R <rules data file> ";

  std::cout << "\n-------------------------------------------------\n\n";
}

int main(int nbParam, char **param) {

  // Import parameters
  if (nbParam <= 1) {
    showParams();
    return 0;
  }

  // Parameters declaration

  const char *trainDataFile = nullptr; // Train data
  bool trainDataFileInit = false;

  bool ruleFileInit = false;
  const char *ruleFile = nullptr;

  // Import parameters

  int p = 1; // We skip "fidex"
  while (p < nbParam) {
    if (*param[p] == '-') {
      p++;

      if (p >= nbParam) {
        throw CommandArgumentException("Missing something at the end of the command.");
      }
      char option = *(param[p - 1] + 1);
      const char *arg = param[p];
      switch (option) { // Get letter after the -

      case 'T':
        trainDataFile = arg; // Parameter after -T
        trainDataFileInit = true;
        break;
      case 'R':
        ruleFile = arg; // Parameter after -R
        ruleFileInit = true;
        break;

      default: // If we put another -X option
        throw CommandArgumentException("Illegal option : " + string(param[p - 1]));
      }
    }

    p++;
  }

  if (!trainDataFileInit) {
    throw CommandArgumentException("The train data file has to be given with option -T");
  }
  if (!ruleFileInit) {
    throw CommandArgumentException("The rule file has to be given with option -R");
  }

  // Get train data
  string line;
  vector<vector<double>> trainData;
  fstream fileDta;

  fileDta.open(trainDataFile, ios::in); // Read data file
  if (fileDta.fail()) {
    throw FileNotFoundError("Error : file " + std::string(trainDataFile) + " not found");
  }

  while (!fileDta.eof()) {
    getline(fileDta, line);
    if (line.length() != 0) {
      std::stringstream myLine(line);
      double value;
      vector<double> tempVect;
      while (myLine >> value) {
        tempVect.push_back(value);
      }
      trainData.push_back(tempVect);
    }
  }

  fileDta.close(); // close data file

  // Get rules and coverings
  vector<tuple<vector<tuple<int, bool, double>>, int>> rules; // rules are vector of antecedants and covgering size
  fstream rulesFile;

  rulesFile.open(ruleFile, ios::in); // Read data file
  if (rulesFile.fail()) {
    throw FileNotFoundError("Error : file " + std::string(ruleFile) + " not found");
  }

  while (!rulesFile.eof()) {
    getline(rulesFile, line);
    if (line.length() != 0 && (line.find("Covering size : ") != std::string::npos)) {
      tuple<vector<tuple<int, bool, double>>, int> currentRule;
      std::string delimiter = "Covering size : ";
      std::string beforeCovSize = line.substr(0, line.find(delimiter));
      line.erase(0, line.find(delimiter) + delimiter.length());
      std::string afterCovSize = line;
      int covSize = stoi(afterCovSize.substr(0, line.find(" Fidelity")));
      std::string firstPart = beforeCovSize.substr(0, beforeCovSize.find(" ->"));
      std::string anteced = firstPart.erase(0, firstPart.find(" : X") + 3);
      std::stringstream antString(anteced);
      string ant;
      vector<std::string> antVect;
      while (antString >> ant) {
        antVect.push_back(ant);
      }
      vector<tuple<int, bool, double>> antecedants;
      tuple<int, bool, double> currentAntecedant;
      bool inequality;
      int attribut;
      double value;
      for (string antecedant : antVect) {
        if (antecedant.find("<") != std::string::npos) {
          inequality = false;
          std::string attributStr = antecedant.substr(0, line.find("<"));
          attributStr.erase(0, 1); // Delete the X
          attribut = stoi(attributStr);
          antecedant.erase(0, antecedant.find("<") + 1);
          value = stod(antecedant);
        }
        if (antecedant.find(">=") != std::string::npos) {
          inequality = true;
          std::string attributStr = antecedant.substr(0, line.find(">="));
          attributStr.erase(0, 1); // Delete the X
          attribut = stoi(attributStr);
          antecedant.erase(0, antecedant.find(">=") + 2);
          value = stod(antecedant);
        }
        currentAntecedant = make_tuple(attribut, inequality, value);
        antecedants.push_back(currentAntecedant);
      }
      currentRule = make_tuple(antecedants, covSize);
      rules.push_back(currentRule);
    }
  }
  rulesFile.close(); // close data file

  // Check how many samples are covered for each rule
  int nbRule = 1;

  for (auto r : rules) {
    int nbCovered = 0;
    bool covered;
    for (const auto &data : trainData) {
      covered = true;
      for (auto ant : get<0>(r)) {
        if (get<1>(ant) == 0) {
          if (data[get<0>(ant)] >= get<2>(ant)) {

            covered = false;
            break;
          }
        } else {
          if (data[get<0>(ant)] < get<2>(ant)) {
            covered = false;
            break;
          }
        }
      }
      if (covered) {
        nbCovered += 1;
      }
    }

    if (get<1>(r) != nbCovered) {
      cout << "Rule " << std::to_string(nbRule) << " : ";

      for (auto ant : get<0>(r)) {
        if (get<1>(ant) == 0) {
          cout << get<0>(ant) << " < " << get<2>(ant) << " ";
        } else {
          cout << get<0>(ant) << " >= " << get<2>(ant) << " ";
        }
      }
      cout << endl;

      cout << "Predicted Cov Size : " << get<1>(r) << endl;
      cout << "True Cov Size : " << nbCovered << endl
           << endl;
    }
    nbRule += 1;
  }
}

// Ex : ./checkCovSize -T ../fidexGlo/datafiles/datanormTrain -R ../fidexGlo/datafiles/globalRulesDatanorm.txt
