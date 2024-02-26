#include "rule.h"

/**
 * @brief Construct a new Rule:: Rule object
 *
 * @param antecedants vector of antecedants to insert inside a rule.
 * @param coveredSamples vector of integers containing the covered samples IDs.
 * @param outClass integer indicating which class is targetted by the rule.
 * @param accuracy double indicating the accuracy of the rule.
 * @param confidence double indicating the confidence of the rule.
 */
Rule::Rule(const vector<Antecedant> &antecedants, const vector<int> &coveredSamples, int outClass, double fidelity, double accuracy, double confidence) {
  setAntecedants(antecedants);
  setCoveredSamples(coveredSamples);
  setCoveringSize(static_cast<int>(coveredSamples.size()));
  setOutputClass(outClass);
  setFidelity(fidelity);
  setAccuracy(accuracy);
  setConfidence(confidence);
}

/**
 * @brief Builds a string presenting every element and value contained by a given rule.
 *
 * @param attributes optional vector of string containing all attributes names, useful to print attribute names instead of integers.
 * @param classes optional vector of string containing all class names, useful to print class name instead of an integer.
 * @return string
 */
string Rule::toString(const vector<string> &attributes, const vector<string> &classes) const {
  stringstream result;
  int _outputClass = getOutputClass();
  auto nbCoveredSamples = getCoveringSize();
  double _fidelity = getFidelity();
  double _accuracy = getAccuracy();
  double _confidence = getConfidence();

  for (Antecedant a : getAntecedants()) {
    if (!attributes.empty()) {
      result << attributes[a.getAttribute()];
    } else {
      result << "X" + to_string(a.getAttribute());
    }

    if (a.getInequality()) {
      result << ">=";
    } else {
      result << "<";
    }

    result << formattingDoubleToString(a.getValue()) << " ";
  }

  if (!classes.empty()) {
    result << "-> " << classes[_outputClass] << endl;
  } else {
    result << "-> class " << getOutputClass() << endl;
  }

  result << "   Train Covering size : " << nbCoveredSamples
         << endl
         << "   Train Fidelity : " << formattingDoubleToString(_fidelity)
         << endl
         << "   Train Accuracy : " << formattingDoubleToString(_accuracy)
         << endl
         << "   Train Confidence : " << formattingDoubleToString(_confidence)
         << endl;

  return result.str();
}

/**
 * @brief Parses a JSON file and returns a vector of the parsed rules. JSON rule format must be like this example:
 * {
 *   "rules": [
 *       {
 *           "antecedants": [
 *               {
 *                   "attribute": 0,
 *                   "inequality": true,
 *                   "value": 0.7231
 *               },
 *               {
 *                   "attribute": 3,
 *                   "inequality": false,
 *                   "value": 0.0031
 *               },
 *               ...
 *           ],
 *           "coveredSamples": [1,2,3, ...],
 *           "outputClass": 0,
 *           "fidelity": 1,
 *           "accuracy": 0.63,
 *           "confidence": 0.8
 *       },
 *       {...}
 *     ]
 *   }
 *
 * @param filename   path of the JSON file to be parsed
 * @return vector<Rule>
 */
// TODO test this
vector<Rule> Rule::fromJsonFile(const string &filename) {
  vector<Rule> result;
  ifstream ifs(filename);

  if (!ifs.is_open() || ifs.fail()) {
    throw FileNotFoundError("JSON file to parse named '" + filename + "' was not found or is corrupted, cannot proceed.");
  }

  // this throws an exception if input is not valid JSON
  Json jsonData = Json::parse(ifs);

  if (!jsonData.contains("rules")) {
    cout << "Parsing error: cannot find 'rules' list in json file." << endl;
    return result;
  }

  result = jsonData["rules"];

  return result;
}

/**
 * @brief writes automatically a vector of rules into a JSON file
 *
 * @param filename name of the file to be written
 * @param rules vector of rules to be written
 */
// TODO test this
void Rule::toJsonFile(const string &filename, const vector<Rule> &rules) {
  ofstream ofs(filename);

  if (!ofs.is_open() || ofs.fail()) {
    throw FileNotFoundError("JSON file to be written named '" + filename + "' couldn't be opened, cannot proceed.");
  }

  Json jsonData{{"rules", rules}};
  ofs << setw(4) << jsonData << endl;
}

bool Rule::isEqual(const Rule &other) const {
  double epsilon = 10e-6;

  if (getAntecedants() != other.getAntecedants())
    return false;

  if (getCoveredSamples() != other.getCoveredSamples())
    return false;

  if (getOutputClass() != other.getOutputClass())
    return false;

  if (fabs(getFidelity() - other.getFidelity()) > epsilon)
    return false;

  if (fabs(getAccuracy() != other.getAccuracy()) > epsilon)
    return false;

  if (fabs(getConfidence() != other.getConfidence()) > epsilon)
    return false;

  return true;
}

/**
 * @brief Generates a regular expression pattern for matching an antecedent of rule using the ids of the attributes.
 *
 * @param nbAttributes The number of attributes that can appear in the rule.
 * @return std::regex The compiled regular expression object that can be used to match an antecedant with attribute's ids.
 */
std::string getAntStrPatternWithAttrIds(int nbAttributes) {
  string pattern;
  for (int i = 0; i < nbAttributes; i++) {
    if (!pattern.empty()) {
      pattern += "|";
    }
    pattern += std::to_string(i);
  }
  std::string idPattern("X(" + pattern + ")([<>]=?)(-?[\\d.]+)");
  return idPattern;
}

/**
 * @brief Generates a regular expression pattern for matching an antecedent using the names of the attributes.
 *
 * @param attributeNames Vector of the names of the attributes that can appear in the rule.
 * @return std::regex The compiled regular expression object that can be used to match an antecedant with attribute's names.
 */
std::string getAntStrPatternWithAttrNames(const std::vector<std::string> &attributeNames) {
  string attrPattern;
  for (const auto &attr : attributeNames) {
    if (!attrPattern.empty()) {
      attrPattern += "|";
    }
    attrPattern += attr;
  }
  std::string attributesPattern("(" + attrPattern + ")([<>]=?)(-?[\\d.]+)");
  return attributesPattern;
}

/**
 * @brief Generates a regular expression pattern for matching a class of rule using the ids of the classes.
 *
 * @param nbClasses The number of classes that can appear in the rule.
 * @return std::regex The compiled regular expression object that can be used to match a rule class id.
 */
std::string getStrPatternWithClassIds(int nbClasses) {
  string pattern;
  for (int i = 0; i < nbClasses; i++) {
    if (!pattern.empty()) {
      pattern += "|";
    }
    pattern += std::to_string(i);
  }
  std::string idPattern("-> class (" + pattern + ")");
  return idPattern;
}

/**
 * @brief Generates a regular expression pattern for matching a class of rule using the names of the classes.
 *
 * @param classNames Vector of the names of the attributes that can appear in the rule.
 * @return std::regex The compiled regular expression object that can be used to match a rule class name.
 */
std::string getStrPatternWithClassNames(const std::vector<std::string> &classNames) {
  string classPattern;
  for (const auto &cl : classNames) {
    if (!classPattern.empty()) {
      classPattern += "|";
    }
    classPattern += cl;
  }
  std::string classesPattern("-> (" + classPattern + ")");
  return classesPattern;
}

/**
 * @brief Checks if a string rule verify pattern with attribute names, attribute ids, class names and class ids. It verifies the entire rule, not just one antecedant.
 *
 * @param str The string rule to check.
 * @param nbAttributes The number of attributes that can appear in the rule.
 * @param attributeNames Vector of the names of the attributes that can appear in the rule.
 * @param nbClasses The number of classes that can appear in the rule.
 * @param classNames Vector of the names of the attributes that can appear in the rule.
 * @return vector<bool> Each boolean tells if a pattern is verified in the rule.
 */
std::vector<bool> getRulePatternsFromString(const std::string &str, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames) {
  bool withAttrIdsPattern = false;
  bool withAttrNamesPattern = false;
  bool withClassIdsPattern = false;
  bool withClassNamesPattern = false;
  std::regex pattern;

  pattern = ": (" + getAntStrPatternWithAttrIds(nbAttributes) + " )*" + "->";
  if (regex_search(str, pattern)) {
    withAttrIdsPattern = true;
  }

  if (!attributeNames.empty()) {
    pattern = ": (" + getAntStrPatternWithAttrNames(attributeNames) + " )*" + "->";
    if (regex_search(str, pattern)) {
      withAttrNamesPattern = true;
    }
  }

  if (regex_search(str, std::regex(getStrPatternWithClassIds(nbClasses)))) {
    withClassIdsPattern = true;
  }
  if (!classNames.empty() && regex_search(str, std::regex(getStrPatternWithClassNames(classNames)))) {
    withClassNamesPattern = true;
  }
  return std::vector<bool>{withAttrIdsPattern, withAttrNamesPattern, withClassIdsPattern, withClassNamesPattern};
}

/**
 * @brief Checks if a rules file verify contains rules with attribute names or attribute ids and with class names or class ids. A rule needs to start with "Rule"
 *
 * @param rulesFile The rules file to check.
 * @param nbAttributes The number of attributes that can appear in a rule.
 * @param attributeNames Vector of the names of the attributes that can appear in a rule.
 * @param nbClasses The number of classes that can appear in a rule.
 * @param classNames Vector of the names of the attributes that can appear in a rule.
 * @return vector<bool> Both boolean tells if there is attribute names or ids and class names or ids.
 */
std::vector<bool> getRulesPatternsFromRuleFile(const std::string &rulesFile, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames) {
  bool withAttrIdsPattern = true;
  bool withAttrNamesPattern = true;
  bool withClassIdsPattern = true;
  bool withClassNamesPattern = true;

  bool foundARule = false;

  ifstream fileDta(rulesFile);

  if (!fileDta) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found");
  }

  std::string line;
  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // If line begins with "Rule"

      foundARule = true;
      std::vector<bool> checkPatterns = getRulePatternsFromString(line, nbAttributes, attributeNames, nbClasses, classNames);

      bool withAttrIdsPatternTemp = checkPatterns[0];
      bool withAttrNamesPatternTemp = checkPatterns[1];
      bool withClassIdsPatternTemp = checkPatterns[2];
      bool withClassNamesPatternTemp = checkPatterns[3];

      // If no pattern matches this rule
      if (!withAttrIdsPatternTemp && !withAttrNamesPatternTemp) {
        throw FileContentError("Error : in file " + rulesFile + ", the rule " + line + " is not on a good format. Maybe an attribute name or id is wrong or you forgot to add the attribute file");
      }
      if (!withClassIdsPatternTemp && !withClassNamesPatternTemp) {
        throw FileContentError("Error : in file " + rulesFile + ", the rule " + line + " is not on a good format. Maybe a class name or id is wrong or you forgot to add the attribute file");
      }

      if (!withAttrIdsPatternTemp) {
        withAttrIdsPattern = false;
      }
      if (!withAttrNamesPatternTemp) {
        withAttrNamesPattern = false;
      }
      if (!withClassIdsPatternTemp) {
        withClassIdsPattern = false;
      }
      if (!withClassNamesPatternTemp) {
        withClassNamesPattern = false;
      }

      // If no pattern matches each rule
      if (!withAttrIdsPattern && !withAttrNamesPattern) {
        throw FileContentError("Error : in file " + rulesFile + ", the rules are not always using the same convention for attributes, or a given attribute name or attribute id is wrong.");
      }
      if (!withClassIdsPattern && !withClassNamesPattern) {
        throw FileContentError("Error : in file " + rulesFile + ", the rules are not always using the same convention for classes, or a given class name or class id is wrong.");
      }
    }
  }

  // If there is no rule in the file
  if (!foundARule) {
    throw FileContentError("Error : there is no rule in the file " + rulesFile + ".");
  }

  return std::vector<bool>{withAttrNamesPattern, withClassNamesPattern};
}

/**
 * @brief Converts a well-formatted rule string to a Rule object.
 *
 * @param rule The generated rule.
 * @param str The string rule to convert.
 * @param withAttributeNames Whether rule contains attribute names
 * @param withClassNames Whether rule contains class names
 * @param nbAttributes The number of attributes that can appear in a rule.
 * @param attributeNames Vector of the names of the attributes that can appear in the rule.
 * @param nbClasses The number of classes that can appear in a rule.
 * @param classNames Vector of the names of the classes that can appear in the rule.
 * @return true If the rule is created.
 * @return false If the rule couldn't be created.
 */
bool stringToRule(Rule &rule, const std::string &str, bool withAttributeNames, bool withClassNames, int nbAttributes, const std::vector<std::string> &attributeNames, int nbClasses, const std::vector<std::string> &classNames) {
  std::regex attributePattern;
  std::regex classPattern;
  if (withAttributeNames) {
    attributePattern = getAntStrPatternWithAttrNames(attributeNames);
  } else {
    attributePattern = getAntStrPatternWithAttrIds(nbAttributes);
  }

  if (withClassNames) {
    classPattern = getStrPatternWithClassNames(classNames);
  } else {
    classPattern = getStrPatternWithClassIds(nbClasses);
  }

  std::vector<Antecedant> antecedants;
  bool isRule = false;

  istringstream iss(str);
  string token;
  while (iss >> token) {
    smatch match;
    if (regex_match(token, match, attributePattern)) {
      isRule = true;
      Antecedant antecedant;
      if (!withAttributeNames) {
        antecedant.setAttribute(stoi(match[1]));
      } else {
        auto it = find(attributeNames.begin(), attributeNames.end(), match[1]);
        antecedant.setAttribute(static_cast<int>(distance(attributeNames.begin(), it)));
      }
      if (match[2] == ">=") {
        antecedant.setInequality(true);
      } else {
        antecedant.setInequality(false);
      }
      antecedant.setValue(stod(match[3]));
      antecedants.push_back(antecedant);
    } else if (token == "->") {
      std::string classString = token;
      iss >> token;
      classString += " " + token;
      if (!withClassNames) {
        iss >> token;
        classString += " " + token;
      }
      if (regex_match(classString, match, classPattern)) {
        isRule = true;
        if (!withClassNames) {
          rule.setOutputClass(stoi(match[1]));
        } else {
          auto it = find(classNames.begin(), classNames.end(), match[1]);
          rule.setOutputClass(static_cast<int>(distance(classNames.begin(), it)));
        }
      }
    }
  }
  if (isRule) {
    rule.setAntecedants(antecedants);
    return true;
  }

  return false;
}

/**
 * @brief Get the Rules from a rules file
 *
 * @param rules Rules obtained from the file.
 * @param rulesFile Rules file.
 * @param attributeNames Vector of the names of the attributes that can appear in the rule.
 * @param classNames Vector of the names of the classes that can appear in the rule.
 * @param nbAttributes The number of attributes that can appear in a rule.
 * @param nbClasses The number of classes that can appear in a rule.
 */
void getRulesPlus(std::vector<Rule> &rules, const std::string &rulesFile, const vector<string> &attributeNames, const vector<string> &classNames, int nbAttributes, int nbClasses) {

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

  std::string line;
  while (getline(rulesData, line)) {
    Rule rule;
    if (stringToRule(rule, line, attributesInFile, classesInFile, nbAttributes, attributeNames, nbClasses, classNames)) {
      getline(rulesData, line); // Cov size
      rule.setCoveringSize(stoi(splitString(line, " ")[4]));
      getline(rulesData, line); // Fidelity
      rule.setFidelity(stoi(splitString(line, " ")[3]));
      getline(rulesData, line); // Accuracy
      rule.setAccuracy(stod(splitString(line, " ")[3]));
      getline(rulesData, line); // Confidence
      rule.setConfidence(stod(splitString(line, " ")[3]));
      rules.push_back(rule);
    }
  }
}
