#ifndef BPNN_H
#define BPNN_H

#include "randFun.h"
#include <iostream>
#include <vector>

#include "dataSet.h"

#include "layerD2.h"
#include "layerDmp.h"

#include <math.h>

#include "layerD3.h"
#include "layerD4.h"
#include "layerFdp.h"

#include "../../../common/cpp/src/checkFun.h"
#include "layerRad.h"
#include "layerSD.h"
#include "layerSP3.h"
#include "layerSP4.h"
#include "layerSP5.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a Backpropagation Neural Network (BpNN).
 *
 * This class implements the structure and functionalities of a neural network using backpropagation for training.
 */
class BpNN {
  std::string ReadFile; /**< The file from which to read the network's weights. */
  std::string SaveFile; /**< The file to which the network's weights are saved. */

  float Eta;  /**< Learning rate for weight updates. */
  float Mu;   /**< Momentum rate for weight updates. */
  float Flat; /**< Flatness parameter used in some activation functions. */

  float ErrParam;      /**< Error threshold parameter. */
  float AccuracyParam; /**< Accuracy threshold parameter. */
  float DeltaErrParam; /**< Delta error threshold parameter. */
  int ShowErrParam;    /**< Number of epochs between error displays. */
  int NbEpochsParam;   /**< Number of epochs for training. */

  int NetId = 1;              /**< Network ID, used for identifying networks in a file. */
  int NbLayers;               /**< Total number of layers including input and output layers. */
  int NbWeightLayers;         /**< Number of weight layers. */
  std::vector<int> NbNeurons; /**< Number of neurons in each layer. */

  std::vector<std::shared_ptr<Layer>> VecLayer; /**< Vector of layers in the network. */

  static int initRandomGen; /**< Static variable to initialize random generator only once. */

  //------------------------------------------------------------------------

  /**
   * @brief Initializes the random generator.
   */
  void InitRandomGen(int seed = 0) const;

  /**
   * @brief Creates the network structure.
   */
  void CreateNetStruct(std::vector<int> nbNeurons);

  /**
   * @brief Writes the architecture parameters to the console.
   */
  void WriteArchParam() const;

  /**
   * @brief Assigns the parameters for the network.
   */
  void AssignParam(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::string &saveFile);

  /**
   * @brief Writes the parameters to the console.
   */
  void WriteParam() const;

  /**
   * @brief Saves the weights to a file.
   */
  void SaveWeights() const;

  /**
   * @brief Pushes the weights to a temporary storage.
   */
  void Push() const;

  /**
   * @brief Pops the weights from the temporary storage.
   */
  void Pop() const;

  //------------------------------------------------------------------------

  /**
   * @brief Forwards one example through the network.
   */
  void ForwardOneExample2(DataSet &data, int index) const;

  /**
   * @brief Computes the error with the same activation by performing the forward pass.
   */
  float ComputeErrorSameAct(
      DataSet &data,
      DataSet &target,
      float *accuracy) const;

  //------------------------------------------------------------------------

  /**
   * @brief Backpropagates one example through the network.
   */
  void BackOneExample(DataSet &target, int index) const;

  /**
   * @brief Trains the network for one epoch.
   */
  void TrainOneEpoch(DataSet &data, DataSet &target, const IntRandomFunction *r) const;

  //------------------------------------------------------------------------

public:
  /**
   * @brief Resets the random generator initialization flag.
   */
  static void resetInitRandomGen();

  /**
   * @brief Gets the number of layers.
   *
   * @return The number of layers.
   */
  int GetNbLayers() const { return NbLayers; }

  /**
   * @brief Gets the number of weight layers.
   *
   * @return The number of weight layers.
   */
  int GetNbWeightLayers() const { return NbWeightLayers; }

  /**
   * @brief Finds the index of the maximum value in a vector.
   */
  int Max(const std::vector<float> &vec) const;

  /**
   * @brief Gets the layer at the specified index.
   *
   * @param indLayer The index of the layer.
   * @return The shared pointer to the layer.
   */
  std::shared_ptr<Layer> GetLayer(int indLayer) { return VecLayer[indLayer]; }

  /**
   * @brief Saves the weights to a specified file.
   */
  void SaveWeights(const std::string &str, int netId) const;

  /**
   * @brief Forwards one example through the network.
   */
  virtual void ForwardOneExample1(DataSet &data, int index);

  /**
   * @brief Forwards one example through the network.
   */
  virtual void ForwardOneExample1(float *ex);

  /**
   * @brief Forwards one example through the network.
   */
  virtual void ForwardOneExample1();

  /**
   * @brief Reads the weights from a file.
   */
  void ReadWeights() const;

  /**
   * @brief Prints the specific error for the Dimlp network.
   */
  void PrintSpecErrDimlp(float specErr, float specAcc) const;

  /**
   * @brief Prints the specific error.
   *
   * @param err The error.
   * @param acc The accuracy.
   */
  virtual void PrintSpecErr(float err, float acc) { PrintSpecErrDimlp(err, acc); }

  /**
   * @brief Defines a Smlp (standard multilayer perceptron) network.
   */
  void DefineSmlp();

  /**
   * @brief Defines a Dimlp network with a Dimlp layer that uses a staircase activation function.
   */
  void DefineDimlp(int discrLevels);

  /**
   * @brief Defines a Qmlp (Quantized multilayer perceptron) network with a Dimlp layer that uses a staircase activation function.
   */
  void DefineQmlp(int discrLevels);

  /**
   * @brief Defines a Fdimlp network with a Dimlp layer and a fuzzy Dimlp layer that use a staircase activation function.
   */
  void DefineFdimlp(int discrLevels);

  /**
   * @brief Defines a Fdimlp2 network with a Dimlp layer and a fuzzy Dimlp layer that use a staircase activation function.
   */
  void DefineFdimlp2(int discrLevels);

  /**
   * @brief Defines a SD (Quantized Support Vector Machine Dot product (linear kernel)) network with a layer using an identity activation function and a Dimlp layer with a staircase activation function.
   */
  void DefineSD(int discrLevels);

  /**
   * @brief Defines a SP3 network with a layer using a cubic activation function and a Dimlp layer with a staircase activation function.
   */
  void DefineSP3(int discrLevels);

  /**
   * @brief Defines a SP4 network with a layer using a quartic (fourth power) activation function and a Dimlp layer with a staircase activation function.
   */
  void DefineSP4(int discrLevels);

  /**
   * @brief Defines a SP5 network with a layer using a quintic (fifth power) activation function and a Dimlp layer with a staircase activation function.
   */
  void DefineSP5(int discrLevels);

  /**
   * @brief Defines a SR (Quantized Support Vector Machine Radial basis function) network that uses a specialized radial basis function (RBF) layer with a Gaussian activation function and a Dimlp layer with a staircase activation function.
   */
  void DefineSR(int discrLevels);

  //------------------------------------------------------------------------

  /**
   * @brief Computes the error and accuracy by performing the forward pass.
   */
  float ComputeError(
      DataSet &data,
      DataSet &target,
      float *accuracy);

  /**
   * @brief Trains the network.
   */
  void TrainPhase(DataSet &train,
                  DataSet &trainTar,
                  DataSet &test,
                  DataSet &testTar,
                  DataSet &valid,
                  DataSet &validTar,
                  const std::string &accuracyFile,
                  bool fromBT = false);

  //------------------------------------------------------------------------

  /**
   * @brief Default copy constructor for BpNN.
   *
   * This constructor creates a new instance of BpNN as a copy of an existing instance.
   */
  BpNN(const BpNN &) = default; // Default copy constructor

  /**
   * @brief Default copy assignment operator for BpNN.
   *
   * This operator assigns the values from an existing instance of BpNN to another instance.
   *
   * @return A reference to the assigned BpNN instance.
   */
  BpNN &operator=(const BpNN &) = default; // Default copy assignment operator

  /**
   * @brief Virtual destructor for BpNN.
   *
   * This destructor ensures that the destructor of the derived class is called
   * when an object is deleted through a pointer to the base class.
   */
  virtual ~BpNN() = default; // virtual destructor

  /**
   * @brief Constructs a BpNN with the specified parameters.
   */
  BpNN(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &saveFile,
      const std::string &printNetType,
      int seed = 0);

  /**
   * @brief Constructs a BpNN from a file.
   */
  BpNN(
      const std::string &readFile,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &printNetType,
      int netId = 1);

  /**
   * @brief Constructs a BpNN with the specified parameters and reads weights from a file.
   */
  BpNN(
      const std::string &readFile,
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      const std::vector<int> &nbNeurons,
      const std::string &saveFile,
      const std::string &printNetType,
      int seed = 0);
};

//------------------------------------------------------------------------

#endif // BPNN_H
