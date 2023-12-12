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
Rule::Rule(vector<Antecedant> antecedants, vector<int> coveredSamples, int out_class, double accuracy, double confidence) {
  setAntecedants(antecedants);
  setCoveredSamples(coveredSamples);
  setOutputClass(out_class);
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
  double accuracy = getAccuracy();
  double confidence = getConfidence();

  for (Antecedant a : getAntecedants()) {
    if (attributes) {
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

  if (classes) {
    result << "-> " << (*classes)[outputClass] << endl;
  } else {
    result << "-> class " << outputClass << endl;
  }
  // TODO pretty print double values with formattingDoubleToString()
  result << "   Train Covering size : " << nbCoveredSamples
         << endl
         << "   Train Fidelity : 1" // TODO: compute this value instead of hardcoding it.
         << endl
         << "   Train Accuracy : " << accuracy
         << endl
         << "   Train Confidence : " << confidence << endl;

  return result.str();
}