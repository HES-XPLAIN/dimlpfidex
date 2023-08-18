#include "hyperLocus.h"

std::vector<std::vector<double>> calcHypLocus(const char *dataFileWeights, int nbQuantLevels, double hiKnot) {

  double lowKnot = -hiKnot;

  std::cout << "\nParameters of hyperLocus :\n\n";
  std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
  std::cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << std::endl
            << std::endl;

  std::cout << "Import weight file..." << std::endl;

  DataSetFid weightDatas(dataFileWeights);

  std::vector<std::vector<double>> allWeights = weightDatas.getWeights();
  std::vector<double> biais = weightDatas.getInBiais();
  std::vector<double> weights = weightDatas.getInWeights();

  std::cout << "Weight file imported" << std::endl
            << std::endl;

  std::cout << "computation of hyperLocus" << std::endl;

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

  std::cout << "HyperLocus computed" << std::endl
            << std::endl;

  return matHypLocus;
}

std::vector<std::vector<double>> calcHypLocus(const char *rulesFile, int nbAttributes) {

  std::vector<std::vector<double>> matHypLocus(nbAttributes);
  std::vector<std::set<double>> thresholds(nbAttributes); // Thresholds for each attribute
  std::ifstream fileDta(rulesFile);

  if (!fileDta) {
    throw FileNotFoundError("Error : file " + std::string(rulesFile) + " not found");
  }

  std::string line;
  while (getline(fileDta, line)) {
    if (line.find("Rule") == 0) { // Si la ligne commence par "Rule"
      std::istringstream iss(line);
      std::string token;
      while (iss >> token) {
        if (token.substr(0, 1) == "X") {
          size_t index = std::stoi(token.substr(1));
          iss >> token;
          iss >> token;
          double value = std::stod(token);
          std::cout << index << " " << value << std::endl;
        }
      }
    }
  }

  fileDta.close(); // close data file

  return matHypLocus;
}
