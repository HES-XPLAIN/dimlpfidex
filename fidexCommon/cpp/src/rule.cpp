#include "rule.h"

/**
 * @brief Construct a new Rule:: Rule object
 *
 * @param antecedants vector of antecedants to insert inside a rule.
 * @param coveredSamples vector of integers containing the covered samples IDs.
 * @param out_class integer indicating which class is targetted by the rule.
 * @param accuracy double indicating the accuracy of the rule.
 * @param confidence double indicating the confidence of the rule.
 */
Rule::Rule(vector<Antecedant> antecedants, vector<int> coveredSamples, int out_class, double fidelity, double accuracy, double confidence) {
  setAntecedants(antecedants);
  setCoveredSamples(coveredSamples);
  setOutputClass(out_class);
  setFidelity(fidelity);
  setAccuracy(accuracy);
  setConfidence(confidence);
}

/**
 * @brief Builds a string presenting every element and value contained by a given rule.
 *
 * @param hasConfidence whether or not to display rule's confidence value.
 * @param attributes optional vector of string containing all attributes names, useful to print attribute names instead of integers.
 * @param classes optional vector of string containing all class names, useful to print class name instead of an integer.
 * @return string
 */
string Rule::toString(const vector<string> *attributes, const vector<string> *classes) {
  stringstream result;
  int outputClass = getOutputClass();
  int nbCoveredSamples = getCoveredSamples().size();
  double fidelity = getFidelity();
  double accuracy = getAccuracy();
  double confidence = getConfidence();

  for (Antecedant a : getAntecedants()) {
    if (attributes && !attributes->empty()) {
      result << (*attributes)[a.getAttribute()];
    } else {
      result << "X" + to_string(a.getAttribute());
    }

    if (a.getInequality()) {
      result << ">=";
    } else {
      result << "<";
    }

    result << to_string(a.gethyperlocus()) << " ";
  }

  if (classes && !classes->empty()) {
    result << "-> " << (*classes)[outputClass] << endl;
  } else {
    result << "-> class " << outputClass << endl;
  }

  result << "   Train Covering size : " << nbCoveredSamples
         << endl
         << "   Train Fidelity : " << formattingDoubleToString(fidelity)
         << endl
         << "   Train Accuracy : " << formattingDoubleToString(accuracy)
         << endl
         << "   Train Confidence : " << formattingDoubleToString(confidence)
         << endl;

  return result.str();
}
