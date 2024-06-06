#include "layerD3.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a LayerDimlp3 object with the given parameters.
 *
 * Initializes the layer with the specified parameters and sets up the staircase activation function.
 *
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flat spot elimination parameter.
 * @param nbDown Number of neurons in the downstream layer.
 * @param nbUp Number of neurons in the upstream layer.
 * @param nbWeights Number of weights.
 * @param nbWeightsForInit Number of weights for initialization.
 * @param nbKnots Number of knots for the staircase function.
 */
LayerDimlp3::LayerDimlp3(
    float eta,
    float mu,
    float flat,
    int nbDown,
    int nbUp,
    int nbWeights,
    int nbWeightsForInit,
    int nbKnots) :

                   Layer(eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit),
                   Stair(std::unique_ptr<StairObj>(new StairObj(nbKnots)))

{
}

///////////////////////////////////////////////////////////////////
