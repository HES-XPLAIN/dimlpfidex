#include "layerD4.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for the LayerDimlp4 class.
 *
 * Initializes the layer with the specified parameters and sets up the staircase activation function.
 *
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flat spot elimination parameter.
 * @param nbDown Number of neurons in the previous layer.
 * @param nbUp Number of neurons in the current layer.
 * @param nbWeights Total number of weights.
 * @param nbWeightsForInit Number of weights for initialization.
 * @param nbKnots Number of knots for the staircase activation function.
 */
LayerDimlp4::LayerDimlp4(
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
