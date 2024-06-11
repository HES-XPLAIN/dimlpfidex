#ifndef LAYERSP4_H
#define LAYERSP4_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized layer with a quartic (fourth power) activation function, derived from the Layer class.
 *
 * This class implements a layer in the neural network that uses a quartic activation function.
 */
class LayerSP4 : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the quartic (fourth power) activation function.
   * @param x The input value.
   * @return The fourth power of the input value.
   */
  float Activation1(float x) override { return Power4(x); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerSP4 class.
   *
   * Initializes the layer with the quartic activation function with the specified parameters.
   *
   * @param nbDown Number of neurons in the previous layer.
   * @param nbUp Number of neurons in the current layer.
   * @param nbWeights Total number of weights.
   * @param nbWeightsForInit Number of weights for initialization.
   */
  LayerSP4(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif // LAYERSP4_H
