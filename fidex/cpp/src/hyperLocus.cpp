#include "hyperLocus.h"

// Potentially optimized version of hyperlocus computation
/* vector<vector<double>> compute() {
   int nbInputs = biases.size();
   int nbBins = nbQuantLevels;
   int nbKnots = nbBins + 1;

   int distance = hiKnot - lowKnot;
   double binWidth = distance / nbBins;

   vector<vector<double>> hyperlocus(nbInputs, vector(nbKnots));
   vector<double> knots(nbKnots, 0.0);

   bool areKnotsComputed = false;

   for (int i = 0; i < nbInputs; i++) {
     for (int j = 0; j < nbKnots; j++) {
       if (!areKnotsComputed) {
         knots[j] = lowKnot + binWidth * j;
       }

       hyperlocus[i, j] = (knots[j] - biases[i]) / weights[i];
     }
     areKnotsComputed = true;
   }

   return hyperlocus;
 }*/

vector<vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot, bool verbose) {

  double lowKnot = -hiKnot;

  std::cout << "\nParameters of hyperLocus :\n"
            << endl;
  std::cout << "- Number of stairs " << nbQuantLevels << endl;
  std::cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << endl
            << endl;

  std::cout << "Import weight file..." << endl;

  DataSetFid weightDatas("weight datas", dataFileWeights);

  int nbNets = weightDatas.getNbNets();

  std::vector<std::vector<double>> matHypLocus;

  for (int n = 0; n < nbNets; n++) {
    vector<double> biais = weightDatas.getInBiais(n);
    vector<double> weights = weightDatas.getInWeights(n);

    if (nbNets == 1) {
      std::cout << "Weight file imported" << endl
                << endl;

      std::cout << "computation of hyperLocus" << endl;
    }

    size_t nbIn = biais.size();      // Number of neurons in the first hidden layer (May be the number of input variables)
    int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

    double dist = hiKnot - lowKnot;         // Size of the interval
    double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

    vector<vector<double>> matHypLocusTemp(nbIn, vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
    vector<double> knots(nbKnots);                                         // vector of location of the separations for one dimension (hyperplans will be placed close)

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

std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, DataSetFid &dataset) {
  std::string line;

  vector<vector<double>> matHypLocus(dataset.getNbAttributes());
  vector<set<double>> thresholds(dataset.getNbAttributes()); // Thresholds for each attribute

  // if rule file is in json format
  if (rulesFile.substr(rulesFile.find_last_of(".") + 1) == "json") {
    vector<Rule> rules = Rule::fromJsonFile(rulesFile);

    for (Rule rule : rules) {
      for (Antecedant ant : rule.getAntecedants()) {
        thresholds[ant.getAttribute()].insert(ant.getValue());
      }
    }

  // rules file is in txt format 
  } else {
    // Get pattern for attributes
    std::regex pattern;
    bool attributesInFile = getRulesPatternsFromRuleFile(rulesFile, dataset, false)[0];
    if (attributesInFile) { // If we have attribute names
      pattern = getAntStrPatternWithAttrNames(dataset.getAttributeNames());
    } else {
      pattern = getAntStrPatternWithAttrIds(dataset.getNbAttributes());
    }

    ifstream fileDta(rulesFile);

    if (!fileDta) {
      throw FileNotFoundError("Error : file " + rulesFile + " not found");
    }
    // Get thresholds values from rules file
    while (getline(fileDta, line)) {
      if (line.find("Rule") == 0) { // If line begins with "Rule"
        Rule rule;
        if (stringToRule(rule, line, attributesInFile, false, dataset)) {
          for (Antecedant ant : rule.getAntecedants()) {
            thresholds[ant.getAttribute()].insert(ant.getValue());
          }
        }
      }
    }

    fileDta.close(); // close data file
  }

  for (size_t i = 0; i < thresholds.size(); ++i) {
    matHypLocus[i].assign(thresholds[i].begin(), thresholds[i].end());
  }

  return matHypLocus;
}
