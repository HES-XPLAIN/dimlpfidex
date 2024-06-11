#ifndef LAYERRAD_H
#define LAYERRAD_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized radial basis function (RBF) layer, derived from the Layer class.
 *
 * This class implements a layer in the neural network that uses Gaussian activation functions
 * and performs radial basis function computations during forward propagation.
 */
class LayerRad : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the Gaussian activation function.
   * @param x The input value.
   * @return The output after applying the Gaussian function.
   */
  float Activation1(float x) override { return Gaussian(x); }

  /**
   * @brief Executes the forward propagation using radial basis functions.
   */
  void ForwLayer() override { ForwRadial(); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerRad class.
   *
   * Initializes the radial basis function layer with the specified parameters.
   *
   * @param nbDown Number of neurons in the previous layer.
   * @param nbUp Number of neurons in the current layer.
   * @param nbWeights Total number of weights.
   * @param nbWeightsForInit Number of weights for initialization.
   */
  LayerRad(
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit) :

                              Layer(0, 0, 0, nbDown, nbUp, nbWeights, nbWeightsForInit)

  {
  }
};

///////////////////////////////////////////////////////////////////

#endif // LAYERRAD_H
