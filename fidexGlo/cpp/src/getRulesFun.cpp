#include "getRulesFun.h"

struct RuleInfo {
  vector<tuple<int, bool, double>> antecedents;
  int ruleClass;
  int covSize;
  double ruleFidelity;
  double ruleAccuracy;
};

std::vector<std::string> splitString(const std::string &str, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = 0;
  while ((end = str.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
  }
  tokens.push_back(str.substr(start));
  return tokens;
}

RuleInfo parseRuleLine(const std::string &line, bool attributsInFile, vector<string> attributeNames, bool hasClassNames, vector<string> classNames) {
  RuleInfo ruleInfo;

  std::vector<std::string> tokens = splitString(line, " ");

  // Parsing antecedents
  size_t antecedentStartIndex = 0;
  size_t antecedentEndIndex = 0;
  size_t antecedentsEndIndex = 0;

  // Find the antecedant start index
  for (antecedentStartIndex = 0; antecedentStartIndex < tokens.size(); ++antecedentStartIndex) {
    if (tokens[antecedentStartIndex] == ":") {
      antecedentStartIndex = 4;
      break;
    }
    if (tokens[antecedentStartIndex] == "->") {
      antecedentStartIndex = 2;
      break;
    }
  }

  // Find the end index of antecedents part
  for (antecedentsEndIndex = antecedentStartIndex; antecedentsEndIndex < tokens.size(); ++antecedentsEndIndex) {
    if (tokens[antecedentsEndIndex] == "->") {
      break;
    }
  }

  while (antecedentStartIndex < tokens.size() && antecedentStartIndex < antecedentsEndIndex) {
    bool ineq;
    int attr;
    double val;
    std::string attributeName;
    int attributeIdx;
    // Find the end index of the current antecedent
    for (antecedentEndIndex = antecedentStartIndex; antecedentEndIndex < tokens.size(); ++antecedentEndIndex) {
      if (tokens[antecedentEndIndex].find("<") != std::string::npos || tokens[antecedentEndIndex].find(">=") != std::string::npos) {
        break;
      }
    }
    // Extract the attribute name, inequality and value
    for (size_t i = antecedentStartIndex; i < antecedentEndIndex + 1; ++i) {
      if (i > antecedentStartIndex) {
        attributeName += " ";
      }
      if (i == antecedentEndIndex) {
        if (tokens[i].find("<") != std::string::npos) {
          ineq = 0;
          std::vector<std::string> antToken = splitString(tokens[i], "<");
          attributeName += antToken[0];
          val = std::stod(antToken[1]);
        } else {
          ineq = 1;
          std::vector<std::string> antToken = splitString(tokens[i], ">=");
          attributeName += antToken[0];
          val = std::stod(antToken[1]);
        }
      } else {
        attributeName += tokens[i];
      }
    }

    if (attributsInFile) {
      bool foundName = false;
      for (size_t idAtr = 0; idAtr < attributeNames.size(); idAtr++) {
        if (attributeNames[idAtr] == attributeName) {
          attributeIdx = idAtr; // Match found, return the index
          foundName = true;
          break;
        }
      }
      if (!foundName) {
        throw std::runtime_error("Error : attribute name found in Rules file is not contained into attribute file.");
      }
      attr = attributeIdx;
    } else {
      attr = stoi(attributeName.substr(1));
    }
    ruleInfo.antecedents.push_back(make_tuple(attr, ineq, val));

    // Move to the next antecedent
    antecedentStartIndex = antecedentEndIndex + 1;
  }
  // Parsing rule class
  size_t classStartIndex = antecedentsEndIndex + 1;
  size_t classEndIndex = 0;

  // Find the end index of the class
  for (classEndIndex = classStartIndex; classEndIndex < tokens.size(); ++classEndIndex) {
    if (tokens[classEndIndex] == "Covering") {
      break;
    }
  }
  string className;
  int classIdx;
  for (size_t i = classStartIndex; i < classEndIndex; ++i) {
    if (i > classStartIndex) {
      className += " ";
    }
    className += tokens[i];
  }

  if (hasClassNames) {
    bool foundClassName = false;
    for (size_t idAtr = 0; idAtr < classNames.size(); idAtr++) {
      if (classNames[idAtr] == className) {
        classIdx = idAtr; // Match found, return the index
        foundClassName = true;
        break;
      }
    }
    if (!foundClassName) {
      throw std::runtime_error("Error : class name found in Rules file is not contained into attribute file.");
    }
    ruleInfo.ruleClass = classIdx;
  } else {
    ruleInfo.ruleClass = stoi(splitString(className, " ")[1]);
  }

  // Parsing covering size
  std::string covSizeToken = tokens[classEndIndex + 3];
  ruleInfo.covSize = std::stoi(covSizeToken);
  // Parsing rule fidelity
  std::string fidelityToken = tokens[classEndIndex + 6];
  ruleInfo.ruleFidelity = std::stod(fidelityToken);

  // Parsing rule accuracy
  std::string accuracyToken = tokens[classEndIndex + 9];
  ruleInfo.ruleAccuracy = std::stod(accuracyToken);

  return ruleInfo;
}

////////////////////////////////////////////////////////

void getRules(vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> &rules, vector<string> &lines, vector<string> &stringRules, const char *rulesFile, size_t nbTestAttributs, bool hasAttributeNames, vector<string> attributeNames, bool hasClassNames, vector<string> classNames) {

  tuple<vector<tuple<int, bool, double>>, int, int, double, double> rule; // <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>

  fstream rulesData;
  rulesData.open(rulesFile, ios::in); // Read data file
  if (rulesData.fail()) {
    throw std::runtime_error("Error : file " + std::string(rulesFile) + " not found");
  }

  lines.push_back("Global statistics of the rule set : ");

  string line;
  getline(rulesData, line);     // Skip first line;
  lines.push_back(line + "\n"); // Add global statistics to output file
  getline(rulesData, line);     // Skip second line
  bool attributsInFile = true;
  bool classesInFile = false;
  if (line.find("not") != std::string::npos) {
    attributsInFile = false;
  } else if (line.find("and") != std::string::npos) {
    classesInFile = true;
  }
  if (attributsInFile && !hasAttributeNames) {
    throw std::runtime_error("The attribute names file has to be given with option -A");
  }
  if (classesInFile && !hasClassNames) {
    throw std::runtime_error("The class names have to be given in attribut file");
  }

  while (getline(rulesData, line)) {
    if (!checkStringEmpty(line)) {
      stringRules.push_back(line);
      RuleInfo ruleInfo = parseRuleLine(line, attributsInFile, attributeNames, classesInFile, classNames);
      rule = make_tuple(ruleInfo.antecedents, ruleInfo.covSize, ruleInfo.ruleClass, ruleInfo.ruleFidelity, ruleInfo.ruleAccuracy);
      rules.push_back(rule);
    }
  }

  rulesData.close(); // close data file
}

////////////////////////////////////////////////////////

void getActivatedRules(vector<int> &activatedRules, vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> *rules, vector<double> *testValues) {
  int attr;
  bool ineq;
  double val;
  for (int r = 0; r < (*rules).size(); r++) { // For each rule
    bool notActivated = false;
    for (int a = 0; a < get<0>((*rules)[r]).size(); a++) { // For each antecedent
      attr = get<0>(get<0>((*rules)[r])[a]);
      ineq = get<1>(get<0>((*rules)[r])[a]);
      val = get<2>(get<0>((*rules)[r])[a]);
      if (ineq == 0 && (*testValues)[attr] >= val) { // If the inequality is not verified
        notActivated = true;
      }
      if (ineq == 1 && (*testValues)[attr] < val) {
        notActivated = true;
      }
    }
    if (!notActivated) {
      activatedRules.push_back(r);
    }
  }
}
