#include "getRulesFun.h"

using namespace std;

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
      attributeName += antToken[0];
      val = std::stod(antToken[1]);
    } else {
      ineq = true;
      std::vector<std::string> antToken = splitString(tokens[id], ">=");
      attributeName += antToken[0];
      val = std::stod(antToken[1]);
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
        throw FileContentError("Error : attribute name found in Rules file is not contained into attribute file.");
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
      throw FileContentError("Error : class name found in Rules file is not contained into attribute file.");
    }

  } else {
    ruleClass = stoi(tokens[id + 2]);
  }
}

////////////////////////////////////////////////////////

void getRules(vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> &rules, vector<string> &statsLines, vector<string> &stringRules, const char *rulesFile, bool hasAttributeNames, const vector<string> &attributeNames, bool hasClassNames, const vector<string> &classNames) {
  tuple<vector<tuple<int, bool, double>>, int, int, double, double> rule; // <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)>
  fstream rulesData;
  rulesData.open(rulesFile, ios::in); // Read data file
  if (rulesData.fail()) {
    throw FileNotFoundError("Error : file " + std::string(rulesFile) + " not found.");
  }
  string line;
  getline(rulesData, line);          // Skip first line;
  statsLines.push_back(line + "\n"); // Add global statistics to output file
  getline(rulesData, line);          // Skip second line
  bool attributsInFile = true;
  bool classesInFile = false;

  if (line.find("not") != std::string::npos) {
    attributsInFile = false;
  } else if (line.find("and") != std::string::npos) {
    classesInFile = true;
  }
  if (attributsInFile && !hasAttributeNames) {
    throw CommandArgumentException("The attribute names file has to be given with option -A because there are attributes in the rule file.");
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

void getActivatedRules(vector<int> &activatedRules, vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> *rules, vector<double> *testValues) {
  int attr;
  bool ineq;
  double val;
  for (int r = 0; r < (*rules).size(); r++) { // For each rule
    bool notActivated = false;
    for (const auto &antecedent : get<0>((*rules)[r])) { // For each antecedant
      attr = get<0>(antecedent);
      ineq = get<1>(antecedent);
      val = get<2>(antecedent);
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
