#ifndef RULE_H
#define RULE_H
// Find better way to include this
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedent.h"
#include "checkFun.h"
#include "dataSet.h"
#include "parameters.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Json = nlohmann::json;

/**
 * @brief Represents a rule used in a rule-based system.
 *
 * This class encapsulates the attributes and methods needed to handle rules
 * which consist of antecedents and the conditions associated with them.
 */
class Rule {
private:
  std::vector<Antecedent> antecedents; ///< Vector of antecedents in the rule.
  std::vector<int> coveredSamples;     ///< Vector of sample IDs covered by the rule.
  int outputClass = -1;                ///< Class targeted by the rule.
  int coveringSize = -1;               ///< Number of samples covered by the rule.
  double fidelity = -1;                ///< Fidelity of the rule.
  double accuracy = -1;                ///< Accuracy of the rule.
  double confidence = -1;              ///< Confidence of the rule.

  // define to ease JSON lib use
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rule, antecedents, coveredSamples, outputClass, coveringSize, fidelity, accuracy, confidence)

public:
  Rule() = default;

  /**
   * @brief Constructs a Rule object.
   */
  Rule(const std::vector<Antecedent> &antecedents,
       const std::vector<int> &coveredSamples,
       int out_class,
       double fidelity,
       double accuracy,
       double confidence);

  // SETTERS

  /**
   * @brief Sets the output class of the rule.
   *
   * @param value The new output class.
   */
  void setOutputClass(int value) { outputClass = value; }

  /**
   * @brief Sets the covering size of the rule.
   *
   * @param value The new covering size.
   */
  void setCoveringSize(int value) { coveringSize = value; }

  /**
   * @brief Sets the fidelity of the rule.
   *
   * @param value The new fidelity value.
   */
  void setFidelity(double value) { fidelity = value; }

  /**
   * @brief Sets the accuracy of the rule.
   *
   * @param value The new accuracy value.
   */
  void setAccuracy(double value) { accuracy = value; }

  /**
   * @brief Sets the confidence of the rule.
   *
   * @param value The new confidence value.
   */
  void setConfidence(double value) { confidence = value; }

  /**
   * @brief Sets the antecedents of the rule.
   *
   * @param values The new antecedents.
   */
  void setAntecedents(const std::vector<Antecedent> &values) { antecedents = values; }

  /**
   * @brief Sets the covered samples of the rule and updates the covering size.
   *
   * @param values The new covered samples.
   */
  void setCoveredSamples(const std::vector<int> &values) {
    coveredSamples = values;
    coveringSize = static_cast<int>(values.size());
  }

  // GETTERS

  /**
   * @brief Gets the antecedents of the rule.
   *
   * @return The antecedents of the rule.
   */
  std::vector<Antecedent> getAntecedents() const { return antecedents; }

  /**
   * @brief Gets the number of antecedents in the rule.
   *
   * @return The number of antecedents.
   */
  int getNbAntecedents() const { return static_cast<int>(antecedents.size()); }

  /**
   * @brief Gets the covered samples of the rule.
   *
   * @return The covered samples.
   */
  std::vector<int> getCoveredSamples() const { return coveredSamples; }

  /**
   * @brief Gets the covering size of the rule.
   *
   * @return The covering size.
   */
  int getCoveringSize() const { return coveringSize; }

  /**
   * @brief Gets the output class of the rule.
   *
   * @return The output class.
   */
  int getOutputClass() const { return outputClass; }

  /**
   * @brief Gets the fidelity of the rule.
   *
   * @return The fidelity.
   */
  double getFidelity() const { return fidelity; }

  /**
   * @brief Gets the accuracy of the rule.
   *
   * @return The accuracy.
   */
  double getAccuracy() const { return accuracy; }

  /**
   * @brief Gets the confidence of the rule.
   *
   * @return The confidence.
   */
  double getConfidence() const { return confidence; }

  // UTILITIES

  /**
   * @brief Adds an antecedent to the rule.
   *
   * @param value The antecedent to add.
   */
  void addAntecedent(Antecedent value) { antecedents.push_back(value); };

  /**
   * @brief Adds a covered sample to the rule and updates the covering size.
   *
   * @param value The sample to add.
   */
  void addCoveredSample(int value) {
    coveredSamples.push_back(value);
    coveringSize += 1;
  };

  /**
   * @brief Builds a string presenting every element and value contained by a given rule.
   */
  std::string toString(const std::vector<std::string> &attributes, const std::vector<std::string> &classes) const;

  /**
   * @brief Compares a rule with another to determine whether they're identical.
   */
  bool isEqual(const Rule &other) const;

  /**
   * @brief Parses a JSON file and returns a vector of the parsed rules.
   */
  static std::vector<Rule> fromJsonFile(const std::string &filename, float &decisionThreshold, int &positiveClassIndex);

  /**
   * @brief Writes a vector of rules into a JSON file.
   */
  static void toJsonFile(const std::string &filename, const std::vector<Rule> &rules, float threshold, int positiveIndex);
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES

/**
 * @brief Overloads the stream insertion operator for easy printing of Rule objects.
 *
 * @param stream The output stream.
 * @param rule The Rule object to print.
 * @return std::ostream& The output stream with the Rule information.
 */
inline std::ostream &operator<<(std::ostream &stream, const Rule &rule) {
  for (Antecedent a : rule.getAntecedents())
    stream << a;
  stream << std::endl;
  stream << "   #Antecedents: " << std::to_string(rule.getAntecedents().size()) << std::endl
         << "   #Covered samples: " << std::to_string(rule.getCoveredSamples().size()) << std::endl
         << "   Confidence: " << std::to_string(rule.getConfidence()) << std::endl
         << "   Accuracy:   " << std::to_string(rule.getAccuracy()) << std::endl
         << "   Covering:   " << std::to_string(rule.getCoveredSamples().size()) << std::endl;

  return stream;
}

/**
 * @brief Overloads the equality operator to compare two Rule objects.
 *
 * @param r1 The first Rule object.
 * @param r2 The second Rule object.
 * @return Returns true if the rules are identical, false otherwise.
 */
inline bool operator==(const Rule &r1, const Rule &r2) {
  return r1.isEqual(r2);
}

/**
 * @brief Overloads the inequality operator to compare two Rule objects.
 *
 * @param r1 The first Rule object.
 * @param r2 The second Rule object.
 * @return Returns true if the rules are not identical, false otherwise.
 */
inline bool operator!=(const Rule &r1, const Rule &r2) {
  return !r1.isEqual(r2);
}

// FUNCTIONS TO HANDLE RULES PATTERNS AND FILES

/**
 * @brief Generates a regular expression pattern for matching positive integers smaller than the given maximum number.
 */
std::string generateRegexSmallerPositive(int maxNumber);

/**
 * @brief Generates a regular expression pattern for matching an antecedent of a rule using the IDs of the attributes.
 */
std::string getAntStrPatternWithAttrIds(int nbAttributes);

/**
 * @brief Generates a regular expression pattern for matching an antecedent using the names of the attributes.
 */
std::string getAntStrPatternWithAttrNames();

/**
 * @brief Generates a regular expression pattern for matching a class of a rule using the IDs of the classes.
 */
std::string getStrPatternWithClassIds(int nbClasses);

/**
 * @brief Generates a regular expression pattern for matching a class of a rule using the names of the classes.
 */
std::string getStrPatternWithClassNames();

/**
 * @brief Checks if a rules file contains rules with attribute names or attribute IDs and with class names or class IDs. A rule needs to start with "Rule".
 */
std::vector<bool> getRulesPatternsFromRuleFile(const std::string &rulesFile, const DataSetFid &dataset, bool withClasses = true);

/**
 * @brief Converts a well-formatted rule string to a Rule object.
 */
bool stringToRule(Rule &rule, const std::string &str, const std::regex &attributePattern, const std::regex &classPattern, bool withAttributeNames, bool withClassNames, DataSetFid &dataset);

/**
 * @brief Get the rules from a rules file.
 */
void getRules(std::vector<Rule> &rules, const std::string &rulesFile, DataSetFid &dataset, float &decisionThreshold, int &positiveClassIndex);

/**
 * @brief Writes a list of rules into a given file. Returns a tuple of two doubles representing the mean covering size and the mean number of antecedents.
 */
std::tuple<double, double> writeRulesFile(const std::string &filename, const std::vector<Rule> &rules, const std::vector<std::string> &attributeNames,
                                          const std::vector<std::string> &classNames, float threshold, int positiveIndex);

/**
 * @brief Get the indices of the rules activated by a test sample.
 */
void getActivatedRules(std::vector<int> &activatedRules, std::vector<Rule> &rules, std::vector<double> &testValues);

/**
 * @brief Extract the decision threshold and the index of the positive class from a rules file.
 */
void getThresholdFromRulesFile(const std::string &filePath, float &decisionThreshold, int &positiveClassIndex);

#endif // RULE_H
