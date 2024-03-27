#ifndef BAG
#include <stdio.h>
#include <string.h>
#endif

#include "bagDimlp.h"

#include <fstream>

///////////////////////////////////////////////////////////////////

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

void BagDimlp::ForwardOneExample1(DataSet &data, int index)

{
  int k;
  const float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  for (int n = 0; n < NbDimlpNets; n++) {
    VectDimlp[n]->ForwardOneExample1(data, index);

    ptrOut = (VectDimlp[n]->GetLayer(NbLayers - 2))->GetUp();

    for (k = 0; k < NbOut; k++)
      GlobalOut[k] += ptrOut[k];
  }

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = GlobalOut[k] / (float)NbDimlpNets;
}

///////////////////////////////////////////////////////////////////

void BagDimlp::ForwardOneExample1()

{
  int k;
  const float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  for (int n = 0; n < NbDimlpNets; n++) {
    VectDimlp[n]->ForwardOneExample1();

    ptrOut = (VectDimlp[n]->GetLayer(NbLayers - 2))->GetUp();

    for (k = 0; k < NbOut; k++)
      GlobalOut[k] += ptrOut[k];
  }

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = GlobalOut[k] / (float)NbDimlpNets;
}

///////////////////////////////////////////////////////////////////

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
