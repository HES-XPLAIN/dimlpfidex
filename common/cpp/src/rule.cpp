#include "rule.h"

/**
 * @brief Constructs a Rule object.
 *
 * @param antecedants vector of antecedants to insert inside a rule.
 * @param coveredSamples vector of integers containing the covered samples IDs.
 * @param outClass integer indicating which class is targetted by the rule.
 * @param fidelity double indicating the fidelity of the rule.
 * @param accuracy double indicating the accuracy of the rule.
 * @param confidence double indicating the confidence of the rule.
 */
Rule::Rule(const std::vector<Antecedant> &antecedants, const std::vector<int> &coveredSamples, int outClass, double fidelity, double accuracy, double confidence) {
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
 * @param attributes Optional vector of strings containing all attributes names, useful to print attribute names instead of integers.
 * @param classes Optional vector of strings containing all class names, useful to print class names instead of an integer.
 * @return std::string A string representation of the rule.
 */
std::string Rule::toString(const std::vector<std::string> &attributes, const std::vector<std::string> &classes) const {
  std::stringstream result;
  int _outputClass = getOutputClass();
  auto nbCoveredSamples = getCoveringSize();
  double _fidelity = getFidelity();
  double _accuracy = getAccuracy();
  double _confidence = getConfidence();

  for (Antecedant a : getAntecedants()) {
    if (!attributes.empty()) {
      result << attributes[a.getAttribute()];
    } else {
      result << "X" + std::to_string(a.getAttribute());
    }

    if (a.getInequality()) {
      result << ">=";
    } else {
      result << "<";
    }

    result << formattingDoubleToString(a.getValue()) << " ";
  }

  if (!classes.empty()) {
    result << "-> " << classes[_outputClass] << std::endl;
  } else {
    result << "-> class " << getOutputClass() << std::endl;
  }

  result << "   Train Covering size : " << std::to_string(nbCoveredSamples)
         << std::endl
         << "   Train Fidelity : " << formattingDoubleToString(_fidelity)
         << std::endl
         << "   Train Accuracy : " << formattingDoubleToString(_accuracy)
         << std::endl
         << "   Train Confidence : " << formattingDoubleToString(_confidence)
         << std::endl;

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
 * @param filename Path of the JSON file to be parsed.
 * @param decisionThreshold Reference to a float where the decision threshold will be stored.
 * @param positiveClassIndex Reference to an int where the positive class index will be stored.
 * @return std::vector<Rule> A vector of parsed rules.
 */
std::vector<Rule> Rule::fromJsonFile(const std::string &filename, float &decisionThreshold, int &positiveClassIndex) {
  std::vector<Rule> result;
  std::ifstream ifs(filename);

  if (!ifs.is_open() || ifs.fail()) {
    throw FileNotFoundError("JSON file to parse named '" + filename + "' was not found or is corrupted, cannot proceed.");
  }

  // this throws an exception if input is not valid JSON
  Json jsonData = Json::parse(ifs);

  if (!jsonData.contains("rules") || !jsonData.contains("positive index class") || !jsonData.contains("threshold")) {
    std::cout << "Parsing error: cannot find 'rules' list in json file." << std::endl;
    return result;
  }

  positiveClassIndex = jsonData["positive index class"];
  decisionThreshold = jsonData["threshold"];
  result = jsonData["rules"];

  return result;
}

/**
 * @brief Writes a vector of rules into a JSON file.
 *
 * @param filename Name of the file to be written.
 * @param rules Vector of rules to be written.
 * @param threshold Decision threshold.
 * @param positiveIndex Positive class index.
 */
void Rule::toJsonFile(const std::string &filename, const std::vector<Rule> &rules, float threshold, int positiveIndex) {
  std::ofstream ofs(filename);

  if (!ofs.is_open() || ofs.fail()) {
    throw FileNotFoundError("JSON file to be written named '" + filename + "' couldn't be opened, cannot proceed.");
  }

  Json jsonData;

  jsonData["threshold"] = threshold;
  jsonData["positive index class"] = positiveIndex;
  jsonData["rules"] = rules;

  ofs << std::setw(4) << jsonData << std::endl;
}

/**
 * @brief Compares a rule with another to determine whether they're identical or not.
 *
 * @param other Other rule for comparison.
 * @return bool Returns true if they're identical, false otherwise.
 */
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
 * @brief Generates a regular expression pattern for matching positive integers smaller than the given maximum number.
 *
 * @param maxNumber The maximum number (exclusive and positive) for which the pattern will match smaller integers.
 * @return std::string The generated regular expression pattern as a string.
 */
std::string generateRegexSmallerPositive(int maxNumber) {
  std::ostringstream regexStream;
  std::string maxStr = std::to_string(maxNumber);
  auto nbDigits = static_cast<int>(maxStr.length());

  // Accept numbers with fewer digits
  if (nbDigits > 1) {
    regexStream << "\\d{1," << (nbDigits - 1) << "}|";
  }

  // Accept numbers with the same number of digits but starting with lower digits
  for (int i = 0; i < nbDigits; ++i) {
    if (maxStr[i] != '0') {
      regexStream << maxStr.substr(0, i) << "[0-" << (maxStr[i] - '1') << "]\\d{" << (nbDigits - i - 1) << "}|";
    }
  }

  // Remove the last '|'
  std::string regex = regexStream.str();
  if (!regex.empty() && regex.back() == '|') {
    regex.pop_back();
  }

  return regex;
}

/**
 * @brief Generates a regular expression pattern for matching an antecedent of a rule using the ids of the attributes.
 *
 * @param nbAttributes The number of attributes that can appear in the rule.
 * @return std::string The compiled regular expression object that can be used to match an antecedant with attribute ids.
 */
std::string getAntStrPatternWithAttrIds(int nbAttributes) {
  std::string pattern = generateRegexSmallerPositive(nbAttributes);
  std::string idPattern("X(" + pattern + ")([<>]=?)(-?[\\d.]+([eE][+-])?\\d+)?");
  return idPattern;
}

/**
 * @brief Generates a regular expression pattern for matching an antecedant using the names of the attributes.
 *
 * @param attributeNames Vector of the names of the attributes that can appear in the rule.
 * @return std::string The compiled regular expression object that can be used to match an antecedant with attribute names.
 */
std::string getAntStrPatternWithAttrNames(const std::vector<std::string> &attributeNames) {
  std::string attrPattern;
  for (const auto &attr : attributeNames) {
    if (!attrPattern.empty()) {
      attrPattern += "|";
    }
    attrPattern += attr;
  }
  std::string attributesPattern("(" + attrPattern + ")([<>]=?)(-?[\\d.]+([eE][+-])?\\d+)?");
  return attributesPattern;
}

/**
 * @brief Generates a regular expression pattern for matching a class of rule using the ids of the classes.
 *
 * @param nbClasses The number of classes that can appear in the rule.
 * @return std::string The compiled regular expression object that can be used to match a rule class id.
 */
std::string getStrPatternWithClassIds(int nbClasses) {
  std::string pattern = generateRegexSmallerPositive(nbClasses);
  std::string idPattern("-> class (" + pattern + ")");
  return idPattern;
}

/**
 * @brief Generates a regular expression pattern for matching a class of rule using the names of the classes.
 *
 * @param classNames Vector of the names of the classes that can appear in the rule.
 * @return std::string The compiled regular expression object that can be used to match a rule class name.
 */
std::string getStrPatternWithClassNames(const std::vector<std::string> &classNames) {
  std::string clPattern;
  for (const auto &cl : classNames) {
    if (!clPattern.empty()) {
      clPattern += "|";
    }
    clPattern += cl;
  }
  std::string classesPattern("-> (" + clPattern + ")");
  return classesPattern;
}

/**
 * @brief Checks if a rules file contains rules with attribute names or attribute ids and with class names or class ids. A rule needs to start with "Rule".
 *
 * @param rulesFile The rules file to check.
 * @param dataset The dataset containing information about the attributes and classes.
 * @param withClasses Whether to check for class patterns as well.
 * @return std::vector<bool> Both booleans tell if there are attribute names or ids and class names or ids.
 * @throws FileNotFoundError If the rules file cannot be opened.
 * @throws FileContentError If the rules in the file are not properly formatted.
 */
std::vector<bool> getRulesPatternsFromRuleFile(const std::string &rulesFile, DataSetFid &dataset, bool withClasses) {
  bool hasAttrIds = true;
  bool hasAttrNames = true;
  bool hasClassIds = true;
  bool hasClassNames = true;

  bool foundARule = false;

  std::ifstream fileDta(rulesFile);
  if (!fileDta) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found");
  }

  std::string line;
  Rule rule;

  std::regex antecedantsPatternIds(": (" + getAntStrPatternWithAttrIds(dataset.getNbAttributes()) + " )*" + "->");
  std::regex antecedantsPatternNames;
  if (dataset.getHasAttributeNames()) {
    antecedantsPatternNames = ": (" + getAntStrPatternWithAttrNames(dataset.getAttributeNames()) + " )*" + "->";
  }
  auto patternWithClassIds = std::regex(getStrPatternWithClassIds(dataset.getNbClasses()));
  std::regex patternWithClassNames;
  if (dataset.getHasClassNames()) {
    patternWithClassNames = std::regex(getStrPatternWithClassNames(dataset.getClassNames()));
  }

  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // If line begins with "Rule"
      foundARule = true;

      bool matched = false;
      if (regex_search(line, antecedantsPatternIds)) {
        matched = true;
        hasAttrIds &= true; // Stays true only if it was already true, can't become true if it's false
      } else {
        hasAttrIds = false;
      }
      if (dataset.getHasAttributeNames() && regex_search(line, antecedantsPatternNames)) {
        matched = true;
        hasAttrNames &= true;
      } else {
        hasAttrNames = false;
      }
      if (!matched) {
        throw FileContentError("Error : in file " + rulesFile + ", the rule " + line + " is not in a good format. Maybe an attribute name or id is wrong or you forgot to add the attribute file");
      }
      matched = false;
      if (regex_search(line, patternWithClassIds)) {
        matched = true;
        hasClassIds &= true;
      } else {
        hasClassIds = false;
      }
      if (dataset.getHasClassNames() && regex_search(line, patternWithClassNames)) {
        matched = true;
        hasClassNames &= true;
      } else {
        hasClassNames = false;
      }
      if (!matched) {
        throw FileContentError("Error : in file " + rulesFile + ", the rule " + line + " is not in a good format. Maybe a class name or id is wrong or you forgot to add the attribute file");
      }

      // If no pattern matches each rule
      if (!hasAttrIds && !hasAttrNames) {
        throw FileContentError("Error : in file " + rulesFile + ", the rules are not always using the same convention for attributes, or a given attribute name or attribute id is wrong.");
      }
      if (!hasClassIds && !hasClassNames && withClasses) {
        throw FileContentError("Error : in file " + rulesFile + ", the rules are not always using the same convention for classes, or a given class name or class id is wrong.");
      }
    }
  }

  // If there is no rule in the file
  if (!foundARule) {
    throw FileContentError("Error : there is no rule in the file " + rulesFile + ". Note: a rule needs to start with 'Rule' keyword");
  }

  return std::vector<bool>{hasAttrNames, hasClassNames};
}

/**
 * @brief Converts a well-formatted rule string to a Rule object.
 *
 * @param rule The generated rule.
 * @param str The string rule to convert.
 * @param attributePattern The regex pattern to match attribute names or ids.
 * @param classPattern The regex pattern to match class names or ids.
 * @param withAttributeNames Whether the rule contains attribute names.
 * @param withClassNames Whether the rule contains class names.
 * @param dataset The dataset containing information about the attributes and classes.
 * @return bool Returns true if the rule is created, false otherwise.

 */
bool stringToRule(Rule &rule, const std::string &str, const std::regex &attributePattern, const std::regex &classPattern, bool withAttributeNames, bool withClassNames, DataSetFid &dataset) {

  std::vector<Antecedant> antecedants;
  bool isRule = false;

  std::istringstream iss(str);
  std::string token;

  while (iss >> token) {
    std::smatch match;
    if (regex_match(token, match, attributePattern)) {
      isRule = true;
      Antecedant antecedant;
      if (!withAttributeNames) {
        antecedant.setAttribute(stoi(match[1]));
      } else {
        auto it = find(dataset.getAttributeNames().begin(), dataset.getAttributeNames().end(), match[1]);
        antecedant.setAttribute(static_cast<int>(distance(dataset.getAttributeNames().begin(), it)));
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
          auto it = find(dataset.getClassNames().begin(), dataset.getClassNames().end(), match[1]);
          rule.setOutputClass(static_cast<int>(distance(dataset.getClassNames().begin(), it)));
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
 * @brief Get the rules from a rules file.
 *
 * @param rules Vector to store the rules obtained from the file.
 * @param rulesFile Path to the rules file.
 * @param dataset The dataset containing information about the attributes and classes.
 * @param decisionThreshold Reference to a float where the decision threshold will be stored.
 * @param positiveClassIndex Reference to an int where the positive class index will be stored.
 * @throws FileNotFoundError If the rules file cannot be opened.
 * @throws FileContentError If the rules in the file are not properly formatted.
 */
void getRules(std::vector<Rule> &rules, const std::string &rulesFile, DataSetFid &dataset, float &decisionThreshold, int &positiveClassIndex) {
  // if file is JSON read it properly
  if (rulesFile.substr(rulesFile.find_last_of(".") + 1) == "json") {
    rules = Rule::fromJsonFile(rulesFile, decisionThreshold, positiveClassIndex);
  } else {
    // Open rules file
    std::fstream rulesData;
    rulesData.open(rulesFile, std::ios::in); // Read data file
    if (rulesData.fail()) {
      throw FileNotFoundError("Error : file " + rulesFile + " not found.");
    }

    // Check if the file has attribute names or ids
    std::vector<bool> checkPatterns = getRulesPatternsFromRuleFile(rulesFile, dataset);
    bool attributesInFile = checkPatterns[0];
    bool classesInFile = checkPatterns[1];

    std::regex attributePattern;
    std::regex classPattern;
    if (attributesInFile) {
      attributePattern = getAntStrPatternWithAttrNames(dataset.getAttributeNames());
    } else {
      attributePattern = getAntStrPatternWithAttrIds(dataset.getNbAttributes());
    }

    if (classesInFile) {
      classPattern = getStrPatternWithClassNames(dataset.getClassNames());
    } else {
      classPattern = getStrPatternWithClassIds(dataset.getNbClasses());
    }

    std::string line;
    while (getline(rulesData, line)) {
      Rule rule;
      bool isRule = stringToRule(rule, line, attributePattern, classPattern, attributesInFile, classesInFile, dataset);

      if (isRule) {
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
}

/**
 * @brief Writes a list of rules into a given file. Returns a tuple of two doubles representing the mean covering size and the mean number of antecedants.
 *
 * @param filename Name of the file to be written/overwritten.
 * @param rules List of Rule objects to be written in "filename".
 * @param attributeNames List of attribute names, used to write Rule's antecedants with attribute names instead of a "X" variable.
 * @param classNames List of class names, used to write Rule's class with class names instead of numerical representation.
 * @param threshold Decision threshold.
 * @param positiveIndex Index defining the positive class in the dataset.
 * @return std::tuple<double, double> A tuple containing the mean covering size and the mean number of antecedants.
 */
std::tuple<double, double> writeRulesFile(const std::string &filename, const std::vector<Rule> &rules, const std::vector<std::string> &attributeNames,
                                          const std::vector<std::string> &classNames, float threshold, int positiveIndex) {
  if (rules.empty()) {
    std::cout << "Warning: cannot write to file \"" << filename << "\", generated rules list is empty.";
    return std::make_tuple(0, 0);
  }

  double meanCovSize = 0;
  double meanNbAntecedents = 0;

  if (filename.substr(filename.find_last_of(".") + 1) == "json") {
    for (Rule r : rules) { // each rule
      meanCovSize += static_cast<double>(r.getCoveredSamples().size());
      meanNbAntecedents += static_cast<double>(r.getAntecedants().size());
    }

    Rule::toJsonFile(filename, rules, threshold, positiveIndex);

  } else {
    int counter = 0;
    std::stringstream stream;
    std::ofstream file(filename);
    auto nbRules = static_cast<int>(rules.size());

    for (Rule r : rules) { // each rule
      meanCovSize += static_cast<double>(r.getCoveredSamples().size());
      meanNbAntecedents += static_cast<double>(r.getAntecedants().size());
      counter++;
      stream << "Rule " << counter << ": " << r.toString(attributeNames, classNames);
      stream << std::endl;
    }

    meanCovSize /= nbRules;
    meanNbAntecedents /= nbRules;

    if (file.is_open()) {
      file << "Number of rules : " << nbRules
           << ", mean sample covering number per rule : " << formattingDoubleToString(meanCovSize)
           << ", mean number of antecedents per rule : " << formattingDoubleToString(meanNbAntecedents)
           << std::endl;
      if (threshold != -1) {
        file << "Using a decision threshold of " << threshold << " for class " << positiveIndex
             << std::endl;
      } else {
        file << "No decision threshold is used.";
      }
      file << std::endl
           << std::endl
           << stream.str();

      file.close();

    } else {
      throw CannotOpenFileError("Error : Couldn't open rules extraction file \"" + filename + "\".");
    }
  }
  return std::make_tuple(meanCovSize, meanNbAntecedents);
}

/**
 * @brief Get the indices of the rules activated by a test sample.
 *
 * @param activatedRules Vector to store indices of activated rules.
 * @param rules Vector of rules to check.
 * @param testValues Values of the test sample for which we search activated rules.
 */
void getActivatedRules(std::vector<int> &activatedRules, std::vector<Rule> &rules, std::vector<double> &testValues) {
  int attr;
  bool ineq;
  double val;
  for (int r = 0; r < rules.size(); r++) { // For each rule
    bool notActivated = false;
    for (const auto &antecedant : rules[r].getAntecedants()) { // For each antecedant
      attr = antecedant.getAttribute();
      ineq = antecedant.getInequality();
      val = antecedant.getValue();
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

/**
 * @brief Extract the decision threshold and the index of the positive class from a rules file.
 *
 * @param filePath The path to the file containing the rules.
 * @param decisionThreshold Reference to a float variable where the decision threshold will be stored.
 *                          Set to -1.0 if not found in the file.
 * @param positiveClassIndex Reference to an int variable where the index of the positive class will be stored.
 *                           Set to -1 if not found in the file.
 * @throws FileNotFoundError If the file specified by filePath cannot be opened.
 */
void getThresholdFromRulesFile(const std::string &filePath, float &decisionThreshold, int &positiveClassIndex) {
  decisionThreshold = -1.0;
  positiveClassIndex = -1;

  std::ifstream file(filePath);
  std::string line;

  if (filePath.substr(filePath.find_last_of('.') + 1) == "json") {
    std::vector<Rule> rules;
    rules = Rule::fromJsonFile(filePath, decisionThreshold, positiveClassIndex);
  } else {

    if (!file) {
      throw FileNotFoundError("Error : file " + filePath + " not found");
    }

    while (std::getline(file, line)) {
      std::string tokenThresh;
      std::string tokenClass;

      if (line.find("Using a decision threshold of") != std::string::npos) {
        std::istringstream iss(line);
        iss >> tokenThresh >> tokenThresh >> tokenThresh >> tokenThresh >> tokenThresh >> tokenThresh >> tokenClass >> tokenClass >> tokenClass; // Get decision threshold
        decisionThreshold = std::stof(tokenThresh);
        positiveClassIndex = std::stoi(tokenClass);
      }
    }

    file.close();
  }
}
