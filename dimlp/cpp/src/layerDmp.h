#ifndef LAYERDMP_H
#define LAYERDMP_H

#include "layer.h"
#include "stairObj.h"
#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief The LayerDimlp class represents a Dimlp layer in a neural network using a staircase activation function.
 *
 * This class inherits from the Layer class and overrides several functions to implement
 * specific behaviors for the forward pass, weight adaptation, and delta computation.
 * It uses the staircase activation function for neuron activations.
 */
class LayerDimlp : public Layer {

  std::unique_ptr<StairObj> Stair; ///< Unique pointer to a StairObj for the staircase activation function.

  //----------------------------------------------------------------

  /**
   * @brief Activation function using the staircase function.
   * @param x The input value.
   * @return The output after applying the staircase function.
   */
  float Activation1(float x) override { return Stair->Funct(x); }

  /**
   * @brief Performs the forward pass specific to this layer.
   */
  void ForwLayer() override { ForwSpec(); }

  /**
   * @brief Adapts the weights specific to this layer.
   */
  void AdaptWeights() override { AdaptWeightsSpec(); }

  /**
   * @brief Overrides the method to prevent delta computation.
   */
  void ComputeDeltaDown() override {
    // This method intentionally left empty to prevent delta computation
  }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerDimlp class.
   */
  LayerDimlp(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit,
      int nbKnots);
};

///////////////////////////////////////////////////////////////////

#endif // LAYERDMP_H
