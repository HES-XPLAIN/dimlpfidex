#include "layerDmp.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for the LayerDimlp class.
 *
 * Initializes the layer with the specified parameters and sets up the staircase activation function.
 *
 * @param eta Learning rate.
 * @param mu Momentum factor.
 * @param flat Flatness factor.
 * @param nbDown Number of neurons in the previous layer.
 * @param nbUp Number of neurons in this layer.
 * @param nbWeights Number of weights in this layer.
 * @param nbWeightsForInit Number of weights for initialization.
 * @param nbKnots Number of knots for the StairObj.
 */
LayerDimlp::LayerDimlp(
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
