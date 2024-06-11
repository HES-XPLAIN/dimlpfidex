#ifndef LAYERSP5_H
#define LAYERSP5_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized layer with a quintic (fifth power) activation function, derived from the Layer class.
 *
 * This class implements a layer in the neural network that uses a quintic activation function.
 */
class LayerSP5 : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the quintic (fifth power) activation function.
   * @param x The input value.
   * @return The fifth power of the input value.
   */
  float Activation1(float x) override { return Power5(x); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerSP5 class.
   *
   * Initializes the layer with the quintic activation function with the specified parameters.
   *
   * @param nbDown Number of neurons in the previous layer.
   * @param nbUp Number of neurons in the current layer.
   * @param nbWeights Total number of weights.
   * @param nbWeightsForInit Number of weights for initialization.
   */
  LayerSP5(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif // LAYERSP5_H
