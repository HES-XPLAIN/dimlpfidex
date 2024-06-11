#ifndef LAYERSP3_H
#define LAYERSP3_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized layer with a cubic activation function, derived from the Layer class.
 *
 * This class implements a layer in the neural network that uses a cubic activation function.
 */
class LayerSP3 : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the cubic activation function.
   * @param x The input value.
   * @return The cube of the input value.
   */
  float Activation1(float x) override { return Power3(x); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerSP3 class.
   *
   * Initializes the layer with the cubic activation function with the specified parameters.
   *
   * @param nbDown Number of neurons in the previous layer.
   * @param nbUp Number of neurons in the current layer.
   * @param nbWeights Total number of weights.
   * @param nbWeightsForInit Number of weights for initialization.
   */
  LayerSP3(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif // LAYERSP3_H
