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

    result << formattingDoubleToString(a.getValue()) << " ";
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
vector<Rule> Rule::fromJsonFile(string filename) {
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

  cout << "About to deserialize Rules" << endl;
  result = jsonData["rules"];
  cout << "Rules deserialized !" << endl;

  return result;
}

// TODO test this
void Rule::toJsonFile(string filename, vector<Rule> rules) {
  ofstream ofs(filename);

  if (!ofs.is_open() || ofs.fail()) {
    throw FileNotFoundError("JSON file to be written named '" + filename + "' couldn't be opened, cannot proceed.");
  }

  Json jsonData{{"rules", rules}};
  ofs << setw(4) << jsonData << endl;
}

bool Rule::isEqual(const Rule other) const {
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
