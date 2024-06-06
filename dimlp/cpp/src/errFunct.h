#ifndef ERRFUNCT_H
#define ERRFUNCT_H

#include <vector>

/**
 * @brief Computes the least mean squared error (LMSE) between network output and target values.
 */
float Lmse(int nbTar, std::vector<float> netOut, std::vector<float> target);

#endif // ERRFUNCT_H
