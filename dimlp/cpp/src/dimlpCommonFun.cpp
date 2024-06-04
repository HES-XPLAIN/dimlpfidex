#include "dimlpCommonFun.h"

/**
 * @brief Saves the outputs of a neural network to a file.
 *
 * This function performs a forward pass on each data example and writes the network's outputs to a specified file.
 *
 * @param data Reference to the dataset containing the examples.
 * @param net Shared pointer to the Dimlp neural network.
 * @param nbOut Number of outputs of the network.
 * @param nbWeightLayers Number of weight layers in the network.
 * @param outfile Name of the file where the outputs will be written.
 * @throws CannotOpenFileError If the output file cannot be opened.
 */
void SaveOutputs(
    DataSet &data,
    std::shared_ptr<Dimlp> net,
    int nbOut,
    int nbWeightLayers,
    const std::string &outfile)

{
  std::filebuf buf;

  if (buf.open(outfile, std::ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  std::cout << "\n\n"
            << outfile << ": "
            << "Writing ..." << std::endl;

  std::ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "" << std::endl;
  }

  std::cout << outfile << ": "
            << "Written.\n"
            << std::endl;
}
