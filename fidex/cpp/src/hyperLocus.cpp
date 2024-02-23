#include "hyperLocus.h"

// Potentially optimized version of hyperlocus computation
// vector<vector<double>> compute() {
//   int nbInputs = biases.size();
//   int nbBins = nbQuantLevels;
//   int nbKnots = nbBins + 1;

//   int distance = hiKnot - lowKnot;
//   double binWidth = distance / nbBins;

//   vector<vector<double>> hyperlocus(nbInputs, vector(nbKnots));
//   vector<double> knots(nbKnots, 0.0);

//   bool areKnotsComputed = false;

//   for (int i = 0; i < nbInputs; i++) {
//     for (int j = 0; j < nbKnots; j++) {
//       if (!areKnotsComputed) {
//         knots[j] = lowKnot + binWidth * j;
//       }

//       hyperlocus[i, j] = (knots[j] - biases[i]) / weights[i];
//     }
//     areKnotsComputed = true;
//   }

//   return hyperlocus;
// }

vector<vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot, bool verbose) {

  double lowKnot = -hiKnot;
  if (verbose) {
    cout << "\nParameters of hyperLocus :\n"
         << endl;
    cout << "- Number of stairs " << nbQuantLevels << endl;
    cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << endl
         << endl;

    cout << "Import weight file..." << endl;
  }

  DataSetFid weightDatas("weight datas", dataFileWeights);

  vector<vector<double>> allWeights = weightDatas.getWeights();
  vector<double> biais = weightDatas.getInBiais();
  vector<double> weights = weightDatas.getInWeights();

  if (verbose) {
    cout << "Weight file imported" << endl
         << endl;

    cout << "computation of hyperLocus" << endl;
  }

  size_t nbIn = biais.size();      // Number of neurons in the first hidden layer (May be the number of input variables)
  int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

  double dist = hiKnot - lowKnot;         // Size of the interval
  double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

  vector<vector<double>> matHypLocus(nbIn, vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
  vector<double> knots(nbKnots);                                     // vector of location of the separations for one dimension (hyperplans will be placed close)

  for (int k = 0; k < nbKnots; k++) {
    knots[k] = lowKnot + (binWidth * k); // location of each separation within a dimension (hyperplans will be placed close)
  }

  for (int i = 0; i < nbIn; i++) { // Loop on dimension
    for (int j = 0; j < nbKnots; j++) {
      matHypLocus[i][j] = (knots[j] - biais[i]) / weights[i]; // Placement of the hyperplan
    }
  }
  if (verbose) {
    cout << "HyperLocus computed" << endl
         << endl;
  }
  return matHypLocus;
}

std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, const int nbAttributes, const std::vector<std::string> &attributeNames) {

  std::string line;
  std::regex Xpattern("X(\\d+)([<>]=?)(-?[\\d.]+)");

  std::regex attributesPattern;
  std::regex pattern;

  // Check if attribute names are provided
  if (!attributeNames.empty()) {
    // Create attribute names pattern
    string attrPattern;
    for (const auto &attr : attributeNames) {
      if (!attrPattern.empty()) {
        attrPattern += "|";
      }
      attrPattern += attr;
    }
    attributesPattern = std::regex("(" + attrPattern + ")([<>]=?)(-?[\\d.]+)");
  }

  vector<vector<double>> matHypLocus(nbAttributes);
  vector<set<double>> thresholds(nbAttributes); // Thresholds for each attribute
  ifstream fileDta(rulesFile);

  if (!fileDta) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found");
  }

  // Check if the file follows the X[nb_attr] pattern or the attribute names pattern
  bool isXPatternFound = false;
  bool isAttributeNamesPatternFound = false;
  while (getline(fileDta, line)) {
    if (regex_search(line, Xpattern)) {
      isXPatternFound = true;
      break;
    } else if (regex_search(line, attributesPattern)) {
      isAttributeNamesPatternFound = true;
      break;
    }
  }
  if (!isXPatternFound && !isAttributeNamesPatternFound) {
    throw FileContentError("Format error in file " + rulesFile + ". The antecedent of a rule must follow one of the formats: 'Xi<=value' or 'attribute_name>value', where 'i' is the attribute ID (in range [0, nbAttributes-1]) and 'value' is a real number. The comparison symbols allowed are '<=' and '>'.");
  } else if (isXPatternFound) {
    pattern = Xpattern;
  } else {
    pattern = attributesPattern;
  }

  // Return to the beginning of the file
  fileDta.clear();
  fileDta.seekg(0, ios::beg);

  // Get thresholds values from rules file
  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // If line begins with "Rule"
      istringstream iss(line);
      string token;
      iss >> token; // "Rule"
      iss >> token; // "1:"
      while (iss >> token) {
        smatch match;
        if (regex_match(token, match, pattern)) {
          int index;
          if (isXPatternFound) {
            index = stoi(match[1]);
          } else {
            auto it = find(attributeNames.begin(), attributeNames.end(), match[1]);
            if (it != attributeNames.end()) {
              index = static_cast<int>(distance(attributeNames.begin(), it));
            } else {
              throw FileContentError("Error: in file " + rulesFile + ", attribute not found in attributeNames vector, got " + token);
            }
          }
          double value = stod(match[3]);
          thresholds[index].insert(value);

        } else if (token == "->") { // end of rule
          break;
        } else {
          throw FileContentError("Error : in file " + rulesFile + ", antecedant of rule not in good format, got " + token);
        }
      }
    }
  }

  for (size_t i = 0; i < thresholds.size(); ++i) {
    matHypLocus[i].assign(thresholds[i].begin(), thresholds[i].end());
  }

  fileDta.close(); // close data file
  return matHypLocus;
}
