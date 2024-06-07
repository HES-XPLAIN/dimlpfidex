#ifndef BAG
#include <stdio.h>
#include <string.h>
#endif

#include "bagDimlp.h"

#include <fstream>

///////////////////////////////////////////////////////////////////

/**
 * @brief Creates datasets for each Dimlp network in the ensemble using bootstrap sampling.
 * @param masterTrain The master training dataset.
 * @param masterClass The master class dataset.
 * @param nbPat Number of patterns in the dataset.
 */
void BagDimlp::MakeDataSets(
    DataSet &masterTrain,
    DataSet &masterClass,
    int nbPat)

{
  int p;
  int k;
  int count;
  int val;
  std::vector<int> indPat(nbPat);
  IntRandomFunction ri(0, nbPat - 1);

  // indPat = new int[nbPat];

  for (int n = 0; n < NbDimlpNets; n++) {
    std::vector<int> busy(nbPat, 0);

    for (p = 0; p < nbPat; p++)
      busy[p] = 0;

    for (p = 0; p < nbPat; p++) {
      val = ri.RandomInteger();
      indPat[p] = val;
      busy[val] = 1;
    }

    count = static_cast<int>(std::count(busy.begin(), busy.end(), 0));
    std::cout << "Network " << n + 1 << " Number of Validation Examples = ";
    std::cout << count << "" << std::endl;

    // indVal = new int[count];
    std::vector<int> indVal(count);

    for (p = 0, k = 0; p < nbPat; p++)
      if (busy[p] == 0) {
        indVal[k] = p;
        k++;
      }
    VectData[n] = std::make_shared<DataSet>(masterTrain, indPat.data(), nbPat);
    VectDataClass[n] = std::make_shared<DataSet>(masterClass, indPat.data(), nbPat);

    ValData[n] = std::make_shared<DataSet>(masterTrain, indVal.data(), count);
    ValDataClass[n] = std::make_shared<DataSet>(masterClass, indVal.data(), count);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Trains all Dimlp networks in the ensemble.
 * @param test The test dataset.
 * @param testTar The test target dataset.
 * @param genericWeightsFile Path to the file for saving weights.
 * @param accuracyFile Path to the file for saving accuracy.
 * @param seed Seed for random number generation.
 */
void BagDimlp::TrainAll(
    DataSet &test,
    DataSet &testTar,
    const std::string &weightsFile,
    const std::string &accuracyFile,
    int seed)

{
  std::string str1;

  // Remove content from weightsFile
  std::ofstream(weightsFile, std::ios::out).close();

  for (int n = 0; n < NbDimlpNets; n++) {
    std::cout << "\n\n-------------------------------------------------------";
    std::cout << "---------------------------";

    std::cout << "\n\nTraining network " << n + 1 << "" << std::endl;

    // Output accuracy on file
    if (!accuracyFile.empty()) {
      std::ofstream accFile(accuracyFile, std::ios::app);
      if (accFile.is_open()) {
        accFile << "Network " << n + 1 << " : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + accuracyFile);
      }
    }

    VectDimlp[n] = std::make_shared<Dimlp>(Eta, Mu, Flat, ErrParam, AccuracyParam,
                                           DeltaErrParam, DiscrLevels, ShowErrParam,
                                           NbEpochsParam, NbLayers, NbNeurons, WeightFile, seed);
    bool fromBT = true;
    VectDimlp[n]->Dimlp::Train(*(VectData[n]), *(VectDataClass[n]),
                               test, testTar,
                               *(ValData[n]), *(ValDataClass[n]), accuracyFile, fromBT);

    VectDimlp[n]->Dimlp::SaveWeights(weightsFile, n + 1);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Initializes networks with pre-trained weights.
 * @param prefix Prefix for the weight files.
 */
void BagDimlp::DefNetsWithWeights(const std::string &prefix)

{
  std::string str1;

  for (int n = 0; n < NbDimlpNets; n++) {
    std::cout << "\n\n-------------------------------------------------------";
    std::cout << "---------------------------";

    std::cout << "\n\nBuilding network " << n + 1 << "" << std::endl;

    VectDimlp[n] = std::make_shared<Dimlp>(prefix, NbLayers, NbNeurons,
                                           DiscrLevels, n + 1);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Creates a global virtual hyperplane for all networks in the ensemble.
 * @param nbBins Number of bins.
 * @param nbIn Number of input neurons.
 * @param multiple Multiplication factor.
 * @return Shared pointer to the global virtual hyperplane.
 */
std::shared_ptr<VirtualHyp> BagDimlp::MakeGlobalVirt(int nbBins, int nbIn, int multiple)

{
  float *bias;
  float *weights;
  std::vector<std::shared_ptr<VirtualHyp>> virt;
  std::shared_ptr<VirtualHyp> globalVirt;

  virt.resize(NbDimlpNets);

  for (int n = 0; n < NbDimlpNets; n++) {
    bias = (VectDimlp[n]->GetLayer(0))->GetBias();
    weights = (VectDimlp[n]->GetLayer(0))->GetWeights();
    virt[n] = std::make_shared<VirtualHyp>(nbBins, nbIn, multiple, bias, weights);
  }
  globalVirt = std::make_shared<VirtualHyp>(nbBins, nbIn, multiple, NbDimlpNets, virt);

  return globalVirt;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward pass using the bagged ensemble of Dimlp networks for a single example.
 *
 * This method iterates over all the Dimlp networks in the ensemble (`VectDimlp`),
 * performs a forward pass on each network, and averages their outputs to produce
 * a final global output.
 *
 * @param data The dataset.
 * @param index Index of the example.
 */
void BagDimlp::ForwardOneExample1(DataSet &data, int index)

{
  int k;
  const float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  // Perform forward pass for each Dimlp network in the ensemble
  for (int n = 0; n < NbDimlpNets; n++) {
    VectDimlp[n]->ForwardOneExample1(data, index);

    ptrOut = (VectDimlp[n]->GetLayer(NbLayers - 2))->GetUp();

    // Accumulate the outputs of each network
    for (k = 0; k < NbOut; k++)
      GlobalOut[k] += ptrOut[k];
  }

  // Average the accumulated outputs to get the final global output
  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = GlobalOut[k] / (float)NbDimlpNets;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward pass using the bagged ensemble of Dimlp networks for a single example.
 *
 * This method iterates over all the Dimlp networks in the ensemble (`VectDimlp`),
 * performs a forward pass on each network, and averages their outputs to produce
 * a final global output.
 */
void BagDimlp::ForwardOneExample1()

{
  int k;
  const float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  // Perform forward pass for each Dimlp network in the ensemble
  for (int n = 0; n < NbDimlpNets; n++) {
    VectDimlp[n]->ForwardOneExample1();

    ptrOut = (VectDimlp[n]->GetLayer(NbLayers - 2))->GetUp();

    // Accumulate the outputs of each network
    for (k = 0; k < NbOut; k++)
      GlobalOut[k] += ptrOut[k];
  }

  // Average the accumulated outputs to get the final global output
  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = GlobalOut[k] / (float)NbDimlpNets;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the accuracy of the ensemble on the given dataset.
 * @param data The dataset.
 * @param target The target dataset.
 * @param accuracy Pointer to store the computed accuracy.
 * @param toWrite Flag to indicate whether to write the predictions to a file.
 * @param predFile Path to the file for saving predictions.
 */
void BagDimlp::ComputeAcc(
    DataSet &data,
    DataSet &target,
    float *accuracy,
    int toWrite,
    const std::string &predFile)

{
  int p;
  int ansNet;
  int ansTar;
  int good;
  int bad;
  std::vector<float> ptrOut;
  std::vector<float> ptrTar;
  std::ofstream buf;

  const int nbPat = data.GetNbEx();
  const int nbOut = target.GetNbAttr();

  if (toWrite) {
    std::cout << "\n\n"
              << predFile << ": "
              << "Writing ..." << std::endl;
  }
  buf.open(predFile);
  if (!buf)
    std::cout << "Cannot open file for writing";

  for (p = 0, good = 0, bad = 0; p < nbPat; p++) {
    BagDimlp::ForwardOneExample1(data, p);

    ptrOut = GlobalOut;
    ptrTar.assign(target.GetExample(p), target.GetExample(p) + nbOut);

    ansNet = Max(ptrOut);
    ansTar = Max(ptrTar);

    if (ansNet == ansTar)
      good++;
    else
      bad++;

    if (toWrite) {
      for (int o = 0; o < nbOut; o++) {
        buf << ptrOut[o] << " ";
      }

      buf << "" << std::endl;
    }
  }
  if (toWrite) {
    std::cout << predFile << ": "
              << "Written.\n"
              << std::endl;
  }

  *accuracy = static_cast<float>(good) + static_cast<float>(bad);
  *accuracy = (float)good / *accuracy;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a BagDimlp object.
 * @param eta Learning rate.
 * @param mu Momentum.
 * @param flat Flat spot elimination parameter.
 * @param errParam Error threshold parameter.
 * @param accuracyParam Accuracy threshold parameter.
 * @param deltaErrParam Delta error threshold parameter.
 * @param discrLevels Number of discretization levels.
 * @param showErrParam Frequency of error display.
 * @param nbEpochsParam Number of training epochs.
 * @param nbLayers Number of layers.
 * @param nbNeurons Vector containing the number of neurons in each layer.
 * @param nbDimlpNets Number of Dimlp networks in the ensemble.
 * @param weightFile Path to the file for saving weights.
 * @param seed Seed for random number generation.
 */
BagDimlp::BagDimlp(
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
    int seed) :

                Dimlp(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
                      discrLevels, showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile, seed),
                NbDimlpNets(nbDimlpNets), Eta(eta), Mu(mu), Flat(flat), ErrParam(errParam), AccuracyParam(accuracyParam),
                DeltaErrParam(deltaErrParam), DiscrLevels(discrLevels), ShowErrParam(showErrParam),
                NbEpochsParam(nbEpochsParam), NbLayers(nbLayers), WeightFile(weightFile)

{

  NbNeurons.assign(nbLayers, 0);

  for (int n = 0; n < nbLayers; n++)
    NbNeurons[n] = nbNeurons[n];

  std::cout << "Number of networks = " << nbDimlpNets << "\n"
            << std::endl;

  VectData.resize(nbDimlpNets);
  VectDataClass.resize(nbDimlpNets);
  ValData.resize(nbDimlpNets);
  ValDataClass.resize(nbDimlpNets);

  VectDimlp.resize(nbDimlpNets);

  NbOut = nbNeurons[NbLayers - 1];
  GlobalOut.resize(NbOut);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructs a BagDimlp object with specified discretization levels, layers, and neurons.
 * @param discrLevels Number of discretization levels.
 * @param nbLayers Number of layers.
 * @param nbNeurons Vector containing the number of neurons in each layer.
 * @param nbDimlpNets Number of Dimlp networks in the ensemble.
 * @param weightFile Path to the file for saving weights.
 * @param seed Seed for random number generation.
 */
BagDimlp::BagDimlp(
    int discrLevels,
    int nbLayers,
    std::vector<int> nbNeurons,
    int nbDimlpNets,
    const std::string &weightFile,
    int seed) :

                Dimlp(0, 0, 0, 0, 0, 0, discrLevels, 0, 0, nbLayers, nbNeurons, weightFile, seed),
                NbDimlpNets(nbDimlpNets), DiscrLevels(discrLevels), NbLayers(nbLayers), WeightFile(weightFile)

{
  NbNeurons.assign(nbLayers, 0);

  for (int n = 0; n < nbLayers; n++)
    NbNeurons[n] = nbNeurons[n];

  std::cout << "Number of networks = " << nbDimlpNets << "\n"
            << std::endl;

  VectDimlp.resize(nbDimlpNets);

  NbOut = nbNeurons[NbLayers - 1];
  // GlobalOut = new float[NbOut];
  GlobalOut.resize(NbOut);
}

///////////////////////////////////////////////////////////////////
