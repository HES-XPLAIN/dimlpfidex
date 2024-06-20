#ifndef LAYERD4_H
#define LAYERD4_H

#include "layer.h"
#include "stairObj.h"

#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a specialized neural network Dimlp layer using a staircase activation function, derived from the Layer class, with no weight or bias adaptation.
 *
 * This class implements a layer in the neural network that uses a staircase activation function for its neurons.
 * It overrides specific functions from the base Layer class to provide customized behavior for forward propagation without adapting weights or biases.
 */
class LayerDimlp4 : public Layer {

  std::unique_ptr<StairObj> Stair; ///< Unique pointer to a StairObj for the staircase activation function.

  //----------------------------------------------------------------

  /**
   * @brief Applies the staircase activation function.
   * @param x The input value.
   * @return The output after applying the staircase function.
   */
  float Activation1(float x) override { return Stair->Funct(x); }

  /**
   * @brief Executes the forward propagation using the specialized method.
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
   * @brief Overrides the method to prevent delta computation.
   */
  void ComputeDeltaDown() override {
    // This method intentionally left empty to prevent delta computation
  }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructor for the LayerDimlp4 class.
   */
  LayerDimlp4(
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

#endif // LAYERD4_H
