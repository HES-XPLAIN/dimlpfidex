#include "hyperLocus.h"

/**
 * @brief Calculates the hyperlocus matrix containing all possible hyperplanes in the feature space that discriminate between different classes of samples, based on the weights training file.
 *
 * This function calculates the positions of the hyperplanes using the number of quantization levels,
 * the size of the interval, the bias and the weights, and then stores these positions in the hyperlocus matrix.
 *
 * @param dataFileWeights Path to the file containing the weights.
 * @param nbQuantLevels Number of quantization levels.
 * @param hiKnot Upper bound of the interval.
 * @return Matrix representing the hyperlocus.
 */
std::vector<std::vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot) {

  double lowKnot = -hiKnot;

  std::cout << "\nParameters of hyperLocus :\n"
            << std::endl;
  std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
  std::cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << std::endl
            << std::endl;

  std::cout << "Import weight file..." << std::endl;

  DataSetFid weightDatas("weight datas", dataFileWeights);

  int nbNets = weightDatas.getNbNets();

  std::vector<std::vector<double>> matHypLocus;

  for (int n = 0; n < nbNets; n++) {
    std::vector<double> biais = weightDatas.getInBiais(n);
    std::vector<double> weights = weightDatas.getInWeights(n);

    if (nbNets == 1) {
      std::cout << "Weight file imported" << std::endl
                << std::endl;

      std::cout << "computation of hyperLocus" << std::endl;
    }

    size_t nbIn = biais.size();      // Number of neurons in the first hidden layer (May be the number of input variables)
    int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

    double dist = hiKnot - lowKnot;         // Size of the interval
    double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

    std::vector<std::vector<double>> matHypLocusTemp(nbIn, std::vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
    std::vector<double> knots(nbKnots);                                                   // vector of location of the separations for one dimension (hyperplans will be placed close)

    for (int k = 0; k < nbKnots; k++) {
      knots[k] = lowKnot + (binWidth * k); // location of each separation within a dimension (hyperplans will be placed close)
    }

    for (int i = 0; i < nbIn; i++) { // Loop on dimension
      for (int j = 0; j < nbKnots; j++) {
        matHypLocusTemp[i][j] = (knots[j] - biais[i]) / weights[i]; // Placement of the hyperplan
      }
    }
    matHypLocus.insert(matHypLocus.end(), matHypLocusTemp.begin(), matHypLocusTemp.end());
  }

  std::cout << "HyperLocus computed" << std::endl
            << std::endl;

  return matHypLocus;
}

/**
 * @brief Calculates the hyperlocus matrix containing all possible hyperplanes in the feature space that discriminate between different classes of samples, based on the rules training file.
 *
 * This function reads a rules file to extract thresholds for each attribute from the
 * antecedents of the rules. These thresholds are then used to build the hyperlocus matrix,
 * which contains the positions of the hyperplanes for each attribute in the dataset.
 *
 * @param rulesFile Path to the file containing the rules.
 * @param dataset Reference to the dataset object.
 * @return Matrix representing the hyperlocus.
 */
std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, DataSetFid &dataset) {
  std::string line;

  std::vector<std::vector<double>> matHypLocus(dataset.getNbAttributes());
  std::vector<std::set<double>> thresholds(dataset.getNbAttributes()); // Thresholds for each attribute

  // Get pattern for attributes
  std::regex attributePattern;
  bool attributeIdsInFile = getRulesPatternsFromRuleFile(rulesFile, dataset, false)[0];
  if (attributeIdsInFile) { // If we have attribute names
    attributePattern = getAntStrPatternWithAttrIds(dataset.getNbAttributes());
  } else {
    attributePattern = getAntStrPatternWithAttrNames();
  }
  std::regex classPattern(getStrPatternWithClassIds(dataset.getNbClasses()));

  std::ifstream fileDta(rulesFile);

  if (!fileDta) {
    throw FileNotFoundError("Error : file " + rulesFile + " not found");
  }
  // Get thresholds values from rules file
  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // If line begins with "Rule"
      Rule rule;
      if (stringToRule(rule, line, attributePattern, classPattern, !attributeIdsInFile, false, dataset)) {
        for (Antecedent ant : rule.getAntecedents()) {
          thresholds[ant.getAttribute()].insert(ant.getValue());
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
