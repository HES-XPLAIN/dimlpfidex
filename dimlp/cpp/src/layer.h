#ifndef LAYER_H
#define LAYER_H

#include <iostream>

#include "errFunct.h"
#include "randFun.h"
#include "standAct.h"
#include <math.h>

///////////////////////////////////////////////////////////////////

/**
 * @brief The Layer class represents a neural network layer with functionality for forward and backward propagation, weight updates, and activation functions.
 */
class Layer {
  float Eta;  ///< Learning rate for weight updates.
  float Mu;   ///< Momentum factor for weight updates.
  float Flat; ///< Flatness factor for the activation function.

  float EtaCentre; ///< Learning rate for the center of radial basis functions.
  float EtaSpread; ///< Learning rate for the spread of radial basis functions.

  int NbDown;           ///< Number of neurons in the previous layer.
  int NbUp;             ///< Number of neurons in this layer.
  int NbWeights;        ///< Number of weights in this layer.
  int NbWeightsForInit; ///< Number of weights for initialization.

  float *Down;                ///< Input values to the layer.
  float *DeltaDown;           ///< Delta values for the input neurons.
  std::vector<float> Up;      ///< Output values from the layer.
  std::vector<float> DeltaUp; ///< Delta values for the output neurons.

  std::vector<float> Weights;          ///< Weights of the connections.
  std::vector<float> OldWeights;       ///< Previous weights (for momentum).
  std::vector<float> ValidWeights;     ///< Validated weights.
  std::vector<float> BiasWeights;      ///< Bias weights.
  std::vector<float> OldBiasWeights;   ///< Previous bias weights (for momentum).
  std::vector<float> ValidBiasWeights; ///< Validated bias weights.

  //----------------------------------------------------------------

  /**
   * @brief Assigns parameters for the layer.
   */
  void AssignParam(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);

  /**
   * @brief Creates the data structures for the layer.
   */
  void CreateStruct();

  /**
   * @brief Applies the first activation function to the layer's outputs.
   */
  void ApplyTransf1();

  /**
   * @brief Applies the second activation function to the layer's outputs.
   */
  void ApplyTransf2();

  /**
   * @brief Performs forward propagation for a fully connected layer.
   */
  void ForwFully();

  /**
   * @brief Computes the delta values for the input neurons using the standard method.
   */
  void ComputeDeltaDownStand();

  /**
   * @brief Updates the bias weights using the standard method.
   */
  void AdaptBiasStand();

  /**
   * @brief Updates the weights for a fully connected layer.
   */
  void AdaptWeightsFully();

  //----------------------------------------------------------------

public:
  /**
   * @brief Initializes the weights of the layer.
   */
  void InitWeights();

  /**
   * @brief Gets the number of input neurons.
   * @return Number of input neurons.
   */
  int GetNbDown() const { return NbDown; }

  /**
   * @brief Gets the number of output neurons.
   * @return Number of output neurons.
   */
  int GetNbUp() const { return NbUp; }

  /**
   * @brief Gets the input values to the layer.
   * @return Pointer to the input values.
   */
  float *GetDown() { return Down; }

  /**
   * @brief Gets the output values from the layer.
   * @return Pointer to the output values.
   */
  float *GetUp() { return Up.data(); }

  /**
   * @brief Gets the delta values for the output neurons.
   * @return Pointer to the delta values.
   */
  float *GetDeltaUp() { return DeltaUp.data(); }

  /**
   * @brief Gets the weights of the connections.
   * @return Pointer to the weights.
   */
  float *GetWeights() { return Weights.data(); }

  /**
   * @brief Gets the bias weights.
   * @return Pointer to the bias weights.
   */
  float *GetBias() { return BiasWeights.data(); }

  /**
   * @brief Sets the input values to the layer.
   * @param pat Pointer to the input values.
   */
  void SetDown(float pat[]) { Down = pat; }

  /**
   * @brief Sets the delta values for the input neurons.
   * @param pat Pointer to the delta values.
   */
  void SetDeltaDown(float pat[]) { DeltaDown = pat; }

  /**
   * @brief The first activation function, default is sigmoid.
   * @param x Input value.
   * @return Activated value.
   */
  virtual float Activation1(float x) { return Sigmoid(x); }

  /**
   * @brief The second activation function, default is sigmoid.
   * @param x Input value.
   * @return Activated value.
   */
  virtual float Activation2(float x) { return Sigmoid(x); }

  /**
   * @brief Computes the half error function, default is mean squared error.
   * @param nbTar Number of target values.
   * @param netOut Network output values.
   * @param target Target values.
   * @return Error value.
   */
  virtual float HalfErrFunct(int nbTar, const std::vector<float> &netOut, const std::vector<float> &target) { return Lmse(nbTar, netOut, target); }

  /**
   * @brief Reads the weights from a file.
   */
  void ReadWeights(std::istream &inFile);

  /**
   * @brief Writes the weights to a file.
   */
  void WriteWeights(std::ostream &outFile);

  /**
   * @brief Pushes the current weights to the validated weights.
   */
  void PushWeights();

  /**
   * @brief Pops the validated weights to the current weights.
   */
  void PopWeights();

  /**
   * @brief Performs forward propagation using a specific method.
   */
  void ForwSpec();

  /**
   * @brief Performs forward propagation using a second specific method.
   */
  void ForwSpec2();

  /**
   * @brief Performs forward propagation for radial basis functions.
   */
  void ForwRadial();

  /**
   * @brief Performs forward propagation for the layer.
   */
  virtual void ForwLayer() {
    ForwFully();
  }

  /**
   * @brief Performs forward propagation and applies the first activation function.
   */
  void ForwAndTransf1() {
    ForwLayer();
    ApplyTransf1();
  }

  /**
   * @brief Performs forward propagation and applies the second activation function.
   */
  void ForwAndTransf2() {
    ForwLayer();
    ApplyTransf2();
  }

  /**
   * @brief Computes the delta values for the output neurons.
   */
  void ComputeDeltaOut(const float target[]);

  /**
   * @brief Computes the delta values for the input neurons using a specific method.
   */
  void ComputeDeltaDownSpec2();

  /**
   * @brief Computes the delta values for the input neurons.
   */
  virtual void ComputeDeltaDown() { ComputeDeltaDownStand(); }

  /**
   * @brief Updates the bias weights using a specific method.
   */
  void AdaptBiasSpec2();

  /**
   * @brief Updates the bias weights.
   */
  virtual void AdaptBias() { AdaptBiasStand(); }

  /**
   * @brief Updates the weights using a specific method.
   */
  void AdaptWeightsSpec();

  /**
   * @brief Updates the weights using a second specific method.
   */
  void AdaptWeightsSpec2();

  /**
   * @brief Updates the weights.
   */
  virtual void AdaptWeights() { AdaptWeightsFully(); }

  /**
   * @brief Performs backward propagation for the layer.
   */
  void BackLayer() {
    AdaptWeights();
    AdaptBias();
    ComputeDeltaDown();
  }

  /**
   * @brief Performs backward propagation for the layer without computing delta values for the input neurons.
   */
  void BackLayerWithout() {
    AdaptWeights();
    AdaptBias();
  }

  /**
   * @brief Sets the learning rates for radial basis functions.
   * @param etaCentre Learning rate for the center.
   * @param etaSpread Learning rate for the spread.
   */
  void SetEtas(float etaCentre, float etaSpread) {
    EtaCentre = etaCentre;
    EtaSpread = etaSpread;
  }

  //----------------------------------------------------------------

  /**
   * @brief Default destructor for the Layer class.
   */
  virtual ~Layer() = default;

  /**
   * @brief Constructor for the Layer class.
   */
  Layer(
      float eta,
      float mu,
      float flat,
      int nbDown,
      int nbUp,
      int nbWeights,
      int nbWeightsForInit);
};

///////////////////////////////////////////////////////////////////

#endif // LAYER_H
