#include "layerFdp.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for the LayerFdimlp class.
 *
 * Initializes the layer with the specified parameters.
 *
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flat spot elimination parameter.
 * @param nbDown Number of neurons in the previous layer.
 * @param nbUp Number of neurons in the current layer.
 * @param nbWeights Total number of weights.
 * @param nbWeightsForInit Number of weights for initialization.
 */
LayerFdimlp::LayerFdimlp(
    float eta,
    float mu,
    float flat,
    int nbDown,
    int nbUp,
    int nbWeights,
    int nbWeightsForInit) :

                            Layer(eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit)

{
}

///////////////////////////////////////////////////////////////////
