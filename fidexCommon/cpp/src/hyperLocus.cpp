#include "hyperLocus.h"
#include <regex>

std::vector<std::vector<double>> calcHypLocus(const char *dataFileWeights, int nbQuantLevels, double hiKnot, bool display) {

  double lowKnot = -hiKnot;
  if (display) {
    std::cout << "\nParameters of hyperLocus :\n"
              << std::endl;
    std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
    std::cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << std::endl
              << std::endl;

    std::cout << "Import weight file..." << std::endl;
  }

  DataSetFid weightDatas("weight datas", dataFileWeights);

  std::vector<std::vector<double>> allWeights = weightDatas.getWeights();
  std::vector<double> biais = weightDatas.getInBiais();
  std::vector<double> weights = weightDatas.getInWeights();

  if (display) {
    std::cout << "Weight file imported" << std::endl
              << std::endl;

    std::cout << "computation of hyperLocus" << std::endl;
  }

  size_t nbIn = biais.size();      // Number of neurons in the first hidden layer (May be the number of input variables)
  int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

  double dist = hiKnot - lowKnot;         // Size of the interval
  double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

  std::vector<std::vector<double>> matHypLocus(nbIn, std::vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
  std::vector<double> knots(nbKnots);                                               // std::vector of location of the separations for one dimension (hyperplans will be placed close)

  for (int k = 0; k < nbKnots; k++) {
    knots[k] = lowKnot + (binWidth * k); // location of each separation within a dimension (hyperplans will be placed close)
  }

  for (int i = 0; i < nbIn; i++) { // Loop on dimension
    for (int j = 0; j < nbKnots; j++) {
      matHypLocus[i][j] = (knots[j] - biais[i]) / weights[i]; // Placement of the hyperplan
    }
  }
  if (display) {
    std::cout << "HyperLocus computed" << std::endl
              << std::endl;
  }
  return matHypLocus;
}

std::vector<std::vector<double>> calcHypLocus(const char *rulesFile, const int nbAttributes, const std::vector<std::string> &attributeNames) {

  std::string line;
  std::regex Xpattern("X(\\d+)([<>]=?)(-?[\\d.]+)");
  std::regex attributesPattern;
  std::regex pattern;

  // Check if attribute names are provided
  if (!attributeNames.empty()) {
    // Create attribute names pattern
    std::string attrPattern;
    for (const auto &attr : attributeNames) {
      if (!attrPattern.empty()) {
        attrPattern += "|";
      }
      attrPattern += attr;
    }
    attributesPattern = std::regex("(" + attrPattern + ")([<>]=?)(-?[\\d.]+)");
  }

  std::vector<std::vector<double>> matHypLocus(nbAttributes);
  std::vector<std::set<double>> thresholds(nbAttributes); // Thresholds for each attribute
  std::ifstream fileDta(rulesFile);

  if (!fileDta) {
    throw FileNotFoundError("Error : file " + std::string(rulesFile) + " not found");
  }

  // Check if the file follows the X[nb_attr] pattern or the attribute names pattern
  bool isXPatternFound = false;
  bool isAttributeNamesPatternFound = false;
  while (getline(fileDta, line)) {
    if (std::regex_search(line, Xpattern)) {
      isXPatternFound = true;
      break;
    } else if (std::regex_search(line, attributesPattern)) {
      isAttributeNamesPatternFound = true;
      break;
    }
  }
  if (!isXPatternFound && !isAttributeNamesPatternFound) {
    throw FileContentError("Error : in file " + std::string(rulesFile) + ", antecedant of rule not in good format, it should be in the form X3<=45.3 or in the form SMOKING>0");
  } else if (isXPatternFound) {
    pattern = Xpattern;
  } else {
    pattern = attributesPattern;
  }

  // Return to the beginning of the file
  fileDta.clear();
  fileDta.seekg(0, std::ios::beg);

  // Get thresholds values from rules file
  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // If line begins with "Rule"
      std::istringstream iss(line);
      std::string token;
      iss >> token; // "Rule"
      iss >> token; // "1:"
      while (iss >> token) {
        std::smatch match;
        if (std::regex_match(token, match, pattern)) {
          int index;
          if (isXPatternFound) {
            index = std::stoi(match[1]);
          } else {
            auto it = std::find(attributeNames.begin(), attributeNames.end(), match[1]);
            if (it != attributeNames.end()) {
              index = static_cast<int>(std::distance(attributeNames.begin(), it));
            } else {
              throw FileContentError("Error: in file " + std::string(rulesFile) + ", attribute not found in attributeNames vector, got " + token);
            }
          }
          double value = std::stod(match[3]);
          thresholds[index].insert(value);

        } else if (token == "->") { // end of rule
          break;
        } else {
          throw FileContentError("Error : in file " + std::string(rulesFile) + ", antecedant of rule not in good format, got " + token);
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
