#ifndef HYPERLOCUSFCT_H
#define HYPERLOCUSFCT_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/dataSet.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/rule.h"
#include <fstream>
#include <iomanip>
#include <regex>
#include <set>
#include <sstream>
#include <string.h>

/**
 * @brief Calculates the hyperlocus matrix containing all possible hyperplanes in the feature
 * space that discriminate between different classes of samples, based on the weights training file.
 */
std::vector<std::vector<double>> calcHypLocus(const std::string &dataFileWeights, int nbQuantLevels, double hiKnot);

/**
 * @brief Calculates the hyperlocus matrix containing all possible hyperplanes in the feature
 * space that discriminate between different classes of samples, based on the rules training file.
 */
std::vector<std::vector<double>> calcHypLocus(const std::string &rulesFile, DataSetFid &dataset);

#endif // HYPERLOCUSFCT_H
