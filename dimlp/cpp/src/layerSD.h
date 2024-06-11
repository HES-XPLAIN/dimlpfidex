#ifndef LAYERSD_H
#define LAYERSD_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized SD (Quantized Support Vector Machine Dot product (linear kernel)) layer with identity activation function, derived from the Layer class.
 *
 * This class implements a layer in the neural network that uses the identity activation function.
 */
class LayerSD : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the identity activation function.
   * @param x The input value.
   * @return The input value itself.
   */
  float Activation1(float x) override { return Identity(x); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerSD class.
   *
   * Initializes the layer with the identity activation function with the specified parameters.
   *
   * @param nbDown Number of neurons in the previous layer.
   * @param nbUp Number of neurons in the current layer.
   * @param nbWeights Total number of weights.
   * @param nbWeightsForInit Number of weights for initialization.
   */
  LayerSD(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif // LAYERSD_H
