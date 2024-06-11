#ifndef BAGDIMLP_H
#define BAGDIMLP_H

#include "dimlp.h"
#include "virtHyp.h"

/**
 * @brief Implements a bagging ensemble of Dimlp neural networks.
 */
class BagDimlp : public Dimlp {
  int NbDimlpNets; ///< Number of Dimlp networks in the ensemble.
  int NbOut;       ///< Number of output neurons.

  float Eta;                     ///< Learning rate.
  float Mu;                      ///< Momentum.
  float Flat;                    ///< Flat spot elimination parameter.
  float ErrParam;                ///< Error threshold parameter.
  float AccuracyParam;           ///< Accuracy threshold parameter.
  float DeltaErrParam;           ///< Delta error threshold parameter.
  int DiscrLevels;               ///< Number of discretization levels.
  int ShowErrParam;              ///< Frequency of error display.
  int NbEpochsParam;             ///< Number of training epochs.
  int NbLayers;                  ///< Number of layers in the network.
  const std::string &WeightFile; ///< Path to the file for saving weights.

  std::vector<int> NbNeurons; ///< Number of neurons in each layer.

  std::vector<float> GlobalOut; ///< Output of the ensemble network.

  std::vector<std::shared_ptr<Dimlp>> VectDimlp;       ///< Vector of Dimlp networks.
  std::vector<std::shared_ptr<DataSet>> VectData;      ///< Vector of training datasets for each network.
  std::vector<std::shared_ptr<DataSet>> VectDataClass; ///< Vector of class datasets for each network.
  std::vector<std::shared_ptr<DataSet>> ValData;       ///< Vector of validation datasets for each network.
  std::vector<std::shared_ptr<DataSet>> ValDataClass;  ///< Vector of validation class datasets for each network.

  //---------------------------------------------------------------------

  //---------------------------------------------------------------------

public:
  /**
   * @brief Gets the global output of the ensemble.
   * @return Pointer to the global output array.
   */
  float *GetGlobalOut() { return GlobalOut.data(); }

  /**
   * @brief Creates datasets for each Dimlp network in the ensemble using bootstrap sampling.
   */
  void MakeDataSets(DataSet &masterTrain, DataSet &masterClass, int nbPat);

  /**
   * @brief Trains all Dimlp networks in the ensemble.
   */
  void TrainAll(
      DataSet &test,
      DataSet &testTar,
      const std::string &genericWeightsFile,
      const std::string &accuracyFile,
      int seed = 0);

  /**
   * @brief Initializes networks with pre-trained weights.
   * @param prefix Prefix for the weight files.
   */
  void DefNetsWithWeights(const std::string &prefix);

  /**
   * @brief Creates a global virtual hyperplane for all networks in the ensemble.
   */
  std::shared_ptr<VirtualHyp> MakeGlobalVirt(int nbBins, int nbIn, int multiple);

  using BpNN::ForwardOneExample1;

  /**
   * @brief Performs forward pass using the bagged ensemble of Dimlp networks for a single example.
   */
  void ForwardOneExample1(DataSet &data, int index) override;

  /**
   * @brief Performs forward pass using the bagged ensemble of Dimlp networks for a single example.
   */
  void ForwardOneExample1() override;

  /**
   * @brief Computes the accuracy of the ensemble on the given dataset.
   */
  void ComputeAcc(DataSet &data, DataSet &target, float *accuracy, int tW, const std::string &predFile);

  /**
   * @brief Constructs a BagDimlp object.
   */
  BagDimlp(
      float eta,
      float mu,
      float flat,
      float errParam,
      float accuracyParam,
      float deltaErrParam,
      int discrLevels,
      int showErrParam,
      int nbEpochsParam,
      int nbLayers,
      std::vector<int> nbNeurons,
      int nbDimlpNets,
      const std::string &weightFile,
      int seed = 0);

  /**
   * @brief Constructs a BagDimlp object with specified discretization levels, layers, and neurons.
   */
  BagDimlp(
      int discrLevels,
      int nbLayers,
      std::vector<int> nbNeurons,
      int nbDimlpNets,
      const std::string &weightFile,
      int seed = 0);
};

#endif // BAGDIMLP_H
