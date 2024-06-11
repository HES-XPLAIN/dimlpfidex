#ifndef LAYERFDP_H
#define LAYERFDP_H

#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized fuzzy neural network layer using Gaussian activation functions, derived from the Layer class.
 *
 * This class implements a fuzzy layer in the neural network that uses Gaussian activation functions for its neurons.
 * It overrides specific functions from the base Layer class to provide customized behavior for forward propagation and weight/bias adaptation.
 */
class LayerFdimlp : public Layer {

  //----------------------------------------------------------------

  /**
   * @brief Applies the Gaussian activation function.
   * @param x The input value.
   * @return The output after applying the Gaussian function.
   */
  float Activation1(float x) override { return Gaussian(x); }

  /**
   * @brief Applies the Gaussian activation function.
   * @param x The input value.
   * @return The output after applying the Gaussian function.
   */
  float Activation2(float x) override { return Gaussian(x); }

  /**
   * @brief Executes the forward propagation using the specialized method.
   */
  void ForwLayer() override { ForwSpec2(); }

  /**
   * @brief Overrides the method to adapt weights using a specialized approach.
   */
  void AdaptWeights() override { AdaptWeightsSpec2(); }

  /**
   * @brief Overrides the method to adapt biases using a specialized approach.
   */
  void AdaptBias() override { AdaptBiasSpec2(); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerFdimlp class.
   */
  LayerFdimlp(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);
};

///////////////////////////////////////////////////////////////////

#endif // LAYERFDP_H
