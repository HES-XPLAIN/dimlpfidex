#include "dimlp.h"

////////////////////////////////////////////////////////////////////////

/**
 * @brief Network type for Dimlp.
 *
 * This variable defines the type of network as "Discretized Interpretable Multi-Layer Perceptron".
 */
const std::string netType = "Discretized Interpretable Multi-Layer Perceptron";

/**
 * @brief Constructs a Dimlp network with specified parameters.
 *
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flatness factor.
 * @param errParam Error threshold.
 * @param accuracyParam Accuracy threshold.
 * @param deltaErrParam Delta error threshold.
 * @param discrLevels Number of discretized levels.
 * @param showErrParam Show error parameter.
 * @param nbEpochsParam Number of epochs.
 * @param nbLayers Number of layers in the network.
 * @param nbNeurons Number of neurons in each layer.
 * @param weightFile File to save weights.
 * @param seed Seed for random number generation (default: 0).
 */
Dimlp::Dimlp(
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int discrLevels,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const std::string &weightFile,
    int seed) :

                BpNN(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
                     showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
                     netType, seed)

{
  std::cout << "Number of quantized levels = " << discrLevels << "\n"
            << std::endl;

  DefineDimlp(discrLevels);
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a Dimlp network by reading weights from a file.
 *
 * @param readFile File to read weights from.
 * @param nbLayers Number of layers in the network.
 * @param nbNeurons Number of neurons in each layer.
 * @param discrLevels Number of discretized levels.
 * @param netId Network ID (default: 1).
 */
Dimlp::Dimlp(
    const std::string &readFile,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    int discrLevels,
    int netId) :

                 BpNN(readFile, nbLayers, nbNeurons,
                      netType, netId)

{
  std::cout << "Number of quantized levels = " << discrLevels << "\n"
            << std::endl;

  DefineDimlp(discrLevels);
  ReadWeights();
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a Dimlp network by reading weights from a file and sets training parameters.
 *
 * @param readFile File to read weights from.
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flatness factor.
 * @param errParam Error threshold.
 * @param accuracyParam Accuracy threshold.
 * @param deltaErrParam Delta error threshold.
 * @param discrLevels Number of discretized levels.
 * @param showErrParam Show error parameter.
 * @param nbEpochsParam Number of epochs.
 * @param nbLayers Number of layers in the network.
 * @param nbNeurons Number of neurons in each layer.
 * @param weightFile File to save weights.
 * @param seed Seed for random number generation (default: 0).
 */
Dimlp::Dimlp(
    const std::string &readFile,
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int discrLevels,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const std::string &weightFile,
    int seed) :

                BpNN(readFile, eta, mu, flat, errParam, accuracyParam, deltaErrParam,
                     showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile,
                     netType, seed) {
  std::cout << "Number of quantized levels = " << discrLevels << "\n"
            << std::endl;

  DefineDimlp(discrLevels);
  ReadWeights();
}

////////////////////////////////////////////////////////////////////////
