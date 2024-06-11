#ifndef LAYERD3_H
#define LAYERD3_H

#include "layer.h"
#include "stairObj.h"

#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief The LayerDimlp3 class represents a Dimlp layer in a neural network using a staircase activation function.
 *
 * This class inherits from the Layer class and overrides several functions to implement
 * specific behaviors for the forward pass, weight adaptation, and delta computation.
 */
class LayerDimlp3 : public Layer {

  std::unique_ptr<StairObj> Stair; ///< Unique pointer to a StairObj for the staircase activation function.

  //----------------------------------------------------------------

  /**
   * @brief Computes the activation using the staircase function.
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
   * @brief Computes the delta values for the downstream layer.
   */
  void ComputeDeltaDown() override { ComputeDeltaDownSpec2(); }

  //----------------------------------------------------------------

public:
  /**
   * @brief Constructs a LayerDimlp3 object with the given parameters.
   */
  LayerDimlp3(
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

#endif // LAYERD3_H
