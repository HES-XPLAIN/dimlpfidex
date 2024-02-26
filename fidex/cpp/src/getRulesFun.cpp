#include "getRulesFun.h"

using namespace std;

void getAntecedents(vector<tuple<int, bool, double>> &antecedents, int &ruleClass, const std::string &line, bool attributsInFile, vector<string> attributeNames, bool hasClassNames, vector<string> classNames) {
  std::vector<std::string> tokens = splitString(line, " ");
  int id = 2;
  while (tokens[id] != "->") {
    bool ineq;
    int attr;
    double val;
    std::string attributeName;
    int attributeIdx;
    if (tokens[id].find("<") != std::string::npos) {
      ineq = false;
      std::vector<std::string> antToken = splitString(tokens[id], "<");
      if (antToken.size() < 2) {
        throw FileContentError("Error : The rule '" + line + "' is not on the good format.");
      }
      attributeName += antToken[0];
      try {
        val = std::stod(antToken[1]);
      } catch (const std::invalid_argument &) {
        throw FileContentError("Error : The rule is not on the good format. The value " + antToken[1] + " is not a number.");
      }
    } else {
      ineq = true;
      std::vector<std::string> antToken = splitString(tokens[id], ">=");
      if (antToken.size() < 2) {
        throw FileContentError("Error : The rule '" + line + "' is not on the good format.");
      }
      attributeName += antToken[0];
      try {
        val = std::stod(antToken[1]);
      } catch (const std::invalid_argument &) {
        throw FileContentError("Error : The rule is not on the good format. The value " + antToken[1] + " is not a number.");
      }
    }
    if (attributsInFile) {
      bool foundName = false;
      for (size_t idAtr = 0; idAtr < attributeNames.size(); idAtr++) {
        if (attributeNames[idAtr] == attributeName) {
          attributeIdx = static_cast<int>(idAtr); // Match found, return the index
          foundName = true;
          break;
        }
      }
      if (!foundName) {
        throw FileContentError("Error : attribute " + attributeName + " found in Rules file is not contained into attribute file.");
      }
      attr = attributeIdx;
    } else {
      attr = stoi(attributeName.substr(1));
    }
    antecedents.push_back(make_tuple(attr, ineq, val));

    id++;
  }
  if (hasClassNames) {
    bool foundClassName = false;
    for (size_t idAtr = 0; idAtr < classNames.size(); idAtr++) {
      if (classNames[idAtr] == tokens[id + 1]) {
        ruleClass = static_cast<int>(idAtr); // Match found, return the index
        foundClassName = true;
        break;
      }
    }
    if (!foundClassName) {
      throw FileContentError("Error : class name " + tokens[id + 1] + " found in Rules file is not contained into attribute file.");
    }

  } else {
    ruleClass = stoi(tokens[id + 2]);
  }
}

void getRulesPlus(vector<Rule> &rules, vector<string> &statsLines, vector<string> &stringRules, const std::string &rulesFile, const vector<string> &attributeNames, const vector<string> &classNames, int nbAttributes, int nbClasses) {

  std::vector<Rule> rules;
  // Open rules file
  fstream rulesData;
  rulesData.open(rulesFile, ios::in); // Read data file
  if (rulesData.fail()) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found.");
  }

  // Check if the file has attribute names or ids
  vector<bool> checkPatterns = getRulesPatternsFromRuleFile(rulesFile, nbAttributes, attributeNames, nbClasses, classNames);
  bool attributesInFile = checkPatterns[0];
  bool classesInFile = checkPatterns[1];
  if (attributesInFile) {
    std::cout << "There is attribute names in file" << std::endl;
  } else {
    std::cout << "There is attribute ids in file" << std::endl;
  }

  if (classesInFile) {
    std::cout << "There is class names in file" << std::endl;
  } else {
    std::cout << "There is class ids in file" << std::endl;
  }

  std::string line;
  while (getline(rulesData, line)) {
    std::cout << line << std::endl;
    Rule rule;
    if (stringToRule(rule, line, attributesInFile, classesInFile, nbAttributes, attributeNames, nbClasses, classNames)) {
      std::cout << rule.toString(attributeNames, classNames) << std::endl;
      rules.push_back(rule);
    }
  }

  /*
  if (!isXPatternFound && !isAttributeNamesPatternFound) {
    throw FileContentError("Format error in file " + rulesFile + ". The antecedent of a rule must follow one of the formats: 'Xi<=value' or 'attribute_name>value', where 'i' is the attribute ID (in range [0, nbAttributes-1]) and 'value' is a real number. The comparison symbols allowed are '<=' and '>'.");
  } else if (isXPatternFound) {
    pattern = Xpattern;
  } else {
    pattern = attributesPattern;
  }*/

  // Return to the beginning of the file
  rulesData.clear();
  rulesData.seekg(0, ios::beg);
}

////////////////////////////////////////////////////////

void getRules(vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> &rules, vector<string> &statsLines, vector<string> &stringRules, const std::string &rulesFile, bool hasAttributeNames, const vector<string> &attributeNames, bool hasClassNames, const vector<string> &classNames) {
  tuple<vector<tuple<int, bool, double>>, int, int, double, double> rule; // <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
  fstream rulesData;
  rulesData.open(rulesFile, ios::in); // Read data file
  if (rulesData.fail()) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found.");
  }
  string line;
  getline(rulesData, line);          // Skip first line;
  statsLines.push_back(line + "\n"); // Add global statistics to output file
  getline(rulesData, line);          // Skip second line
  bool attributsInFile = true;
  bool classesInFile = true;
  if (line.find("Attribute names are not specified.") != std::string::npos) { // TODO : See if we can delete those lines in file and replace by a check to see if we have attribute names or not. cf. patterns in hyperLocus.cpp
    attributsInFile = false;
  }
  getline(rulesData, line);
  if (line.find("Class names are not specified.") != std::string::npos) {
    classesInFile = false;
  }
  if (attributsInFile && !hasAttributeNames) {
    throw CommandArgumentException("The attribute names file has to be given with option --attributes_file because there are attributes in the rule file.");
  }
  if (classesInFile && !hasClassNames) {
    throw CommandArgumentException("The class names have to be given in attribut file because there are class names in the rule file.");
  }
  string strRule;

  while (getline(rulesData, line)) { // New rule
    if (!checkStringEmpty(line)) {
      strRule = line;
      vector<tuple<int, bool, double>> antecedents;
      int ruleClass;
      getAntecedents(antecedents, ruleClass, line, attributsInFile, attributeNames, classesInFile, classNames);
      getline(rulesData, line); // Cov size
      strRule += "\n" + line;
      int covSize = stoi(splitString(line, " ")[4]);
      getline(rulesData, line); // Fidelity
      strRule += "\n" + line;
      int ruleFidelity = stoi(splitString(line, " ")[3]);
      getline(rulesData, line); // Accuracy
      strRule += "\n" + line;
      double ruleAccuracy = stod(splitString(line, " ")[3]);
      rule = make_tuple(antecedents, covSize, ruleClass, ruleFidelity, ruleAccuracy);
      rules.push_back(rule);
      getline(rulesData, line);
      strRule += "\n" + line;
      strRule += "\n";
      stringRules.emplace_back(strRule);
    }
  }
  rulesData.close(); // close data file
}

////////////////////////////////////////////////////////

void getActivatedRules(vector<int> &activatedRules, vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> &rules, vector<double> &testValues) {
  int attr;
  bool ineq;
  double val;
  for (int r = 0; r < rules.size(); r++) { // For each rule
    bool notActivated = false;
    for (const auto &antecedent : get<0>(rules[r])) { // For each antecedant
      attr = get<0>(antecedent);
      ineq = get<1>(antecedent);
      val = get<2>(antecedent);
      if (ineq == 0 && testValues[attr] >= val) { // If the inequality is not verified
        notActivated = true;
      }
      if (ineq == 1 && testValues[attr] < val) {
        notActivated = true;
      }
    }
    if (!notActivated) {
      activatedRules.push_back(r);
    }
  }
}
