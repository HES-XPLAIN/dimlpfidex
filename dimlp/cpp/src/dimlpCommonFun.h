#ifndef DIMLPCOMMONFUN_H
#define DIMLPCOMMONFUN_H

#include "dimlp.h"

/**
 * @brief Saves the outputs of a neural network to a file.
 */
void SaveOutputs(DataSet &data, std::shared_ptr<Dimlp> net, int nbOut, int nbWeightLayers, const std::string &outfile);

#endif // DIMLPCOMMONFUN_H
