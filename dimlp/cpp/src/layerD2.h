#ifndef LAYERD2_H
#define LAYERD2_H

#include "layer.h"
#include "stairObj.h"

#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief The LayerDimlp2 class represents a Dimlp layer in a neural network using a staircase activation function.
 *
 * This class inherits from the Layer class and overrides several functions to implement
 * specific behaviors for the forward pass and delta computation.
 * This layer does not adapt weights or biases during training.
 */
class LayerDimlp2 : public Layer {

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
   * @brief Overrides the method to prevent weight adaptation.
   */
  void AdaptWeights() override {
    // This method intentionally left empty to prevent weight adaptation
  }

  /**
   * @brief Overrides the method to prevent bias adaptation.
   */
  void AdaptBias() override {
    // This method intentionally left empty to prevent bias adaptation
  }

  /**
   * @brief Computes the delta values for the input neurons using a specific method.
   */
  void ComputeDeltaDown() override { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerDimlp2 class.
   */
  LayerDimlp2(
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

#endif // LAYERD2_H
