#ifndef BAG
#include "writeErr.h"
#include <stdio.h>
#include <string.h>
#endif

#include "bagDimlp.h"

#include <fstream>
using namespace std;

///////////////////////////////////////////////////////////////////

void BagDimlp::MakeDataSets(DataSet &masterTrain, DataSet &masterClass,
                            int nbPat)

{
  int n, p, k, count, val;
  int *indPat;
  int *indVal;
  int *busy;
  IntRandomFunction ri(0, nbPat - 1);

  indPat = new int[nbPat];

  for (n = 0; n < NbDimlpNets; n++) {
    busy = new int[nbPat];

    for (p = 0; p < nbPat; p++)
      busy[p] = 0;

    for (p = 0; p < nbPat; p++) {
      val = ri.RandomInteger();
      indPat[p] = val;
      busy[val] = 1;
    }

    for (p = 0, count = 0; p < nbPat; p++)
      if (busy[p] == 0)
        count++;

    cout << "Network " << n + 1 << " Number of Validation Examples = ";
    cout << count << "\n";

    indVal = new int[count];

    for (p = 0, k = 0; p < nbPat; p++)
      if (busy[p] == 0) {
        indVal[k] = p;
        k++;
      }

    VectData[n] = new DataSet(masterTrain, indPat, nbPat);
    VectDataClass[n] = new DataSet(masterClass, indPat, nbPat);

    ValData[n] = new DataSet(masterTrain, indVal, count);
    ValDataClass[n] = new DataSet(masterClass, indVal, count);

    delete busy;
    delete indVal;
  }

  delete indPat;
}

///////////////////////////////////////////////////////////////////

void BagDimlp::TrainAll(DataSet &masterTrain, DataSet &masterClass,
                        DataSet &test, DataSet &testTar,
                        char genericWeightsFile[], char *accuracyFile)

{
  int n;
  char str1[80];
  char str2[5];

  for (n = 0; n < NbDimlpNets; n++) {
    cout << "\n\n-------------------------------------------------------";
    cout << "---------------------------";

    cout << "\n\nTraining network " << n + 1 << "\n";

    // Output accuracy on file
    if (accuracyFile != 0) {
      ofstream accFile(accuracyFile, ios::app);
      if (accFile.is_open()) {
        accFile << "Network " << n + 1 << " : \n\n";
        accFile.close();
      } else {
        char errorMsg[] = "Cannot open file for writing";
        WriteError(errorMsg, accuracyFile);
      }
    }

    VectDimlp[n] = new Dimlp(Eta, Mu, Flat, ErrParam, AccuracyParam,
                             DeltaErrParam, DiscrLevels, ShowErrParam,
                             NbEpochsParam, NbLayers, NbNeurons, WeightFile);
    bool fromBT = true;
    VectDimlp[n]->Dimlp::Train(*(VectData[n]), *(VectDataClass[n]), test,
                               testTar, *(ValData[n]), *(ValDataClass[n]),
                               accuracyFile, fromBT);

    strcpy(str1, genericWeightsFile);
    sprintf(str2, "%d", n + 1);
    strcat(str1, str2);
    strcat(str1, ".wts");
    VectDimlp[n]->Dimlp::SaveWeights(str1);
  }
}

///////////////////////////////////////////////////////////////////

void BagDimlp::DefNetsWithWeights(char *prefix)

{
  int n;
  char str1[80];
  char str2[5];

  for (n = 0; n < NbDimlpNets; n++) {
    cout << "\n\n-------------------------------------------------------";
    cout << "---------------------------";

    cout << "\n\nBuilding network " << n + 1 << "\n";

    strcpy(str1, prefix);
    sprintf(str2, "%d", n + 1);
    strcat(str1, str2);
    strcat(str1, ".wts");
    VectDimlp[n] = new Dimlp(str1, NbLayers, NbNeurons, DiscrLevels);
  }
}

///////////////////////////////////////////////////////////////////

VirtualHyp *BagDimlp::MakeGlobalVirt(int nbBins, int nbIn, int multiple)

{
  int n;
  float *bias;
  float *weights;
  VirtualHyp **virt;
  VirtualHyp *globalVirt;

  virt = new VirtualHyp *[NbDimlpNets];

  for (n = 0; n < NbDimlpNets; n++) {
    bias = (VectDimlp[n]->GetLayer(0))->GetBias();
    weights = (VectDimlp[n]->GetLayer(0))->GetWeights();
    virt[n] = new VirtualHyp(nbBins, nbIn, multiple, bias, weights);
  }

  globalVirt = new VirtualHyp(nbBins, nbIn, multiple, NbDimlpNets, virt);

  for (n = 0; n < NbDimlpNets; n++)
    virt[n]->Del();

  delete virt;

  return globalVirt;
}

///////////////////////////////////////////////////////////////////

void BagDimlp::ForwardOneExample1(DataSet &data, int index)

{
  int n, k;
  float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  for (n = 0; n < NbDimlpNets; n++) {
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
  int n, k, l;
  float *ptrOut;

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = 0;

  for (n = 0; n < NbDimlpNets; n++) {
    VectDimlp[n]->ForwardOneExample1();

    ptrOut = (VectDimlp[n]->GetLayer(NbLayers - 2))->GetUp();

    for (k = 0; k < NbOut; k++)
      GlobalOut[k] += ptrOut[k];
  }

  for (k = 0; k < NbOut; k++)
    GlobalOut[k] = GlobalOut[k] / (float)NbDimlpNets;
}

///////////////////////////////////////////////////////////////////

void BagDimlp::ComputeAcc(DataSet &data, DataSet &target, float *accuracy,
                          int toWrite, char predFile[])

{
  int p, o, ansNet, ansTar;
  int good, bad;
  float *ptrOut;
  float *ptrTar;
  // filebuf  buf;
  ofstream buf;

  const int nbPat = data.GetNbEx();
  const int nbOut = target.GetNbAttr();

  // if (buf.open("dimlpBT.out", output) == 0)
  //  cout << "Cannot open file for writing.\n\n";

  if (toWrite) {
    cout << "\n\n"
         << predFile << ": "
         << "Writing ...\n";
  }
  buf.open(predFile);
  if (!buf)
    cout << "Cannot open file for writing";

  // ostream outFile(&buf);

  for (p = 0, good = 0, bad = 0; p < nbPat; p++) {
    BagDimlp::ForwardOneExample1(data, p);

    ptrOut = GlobalOut;
    ptrTar = target.GetExample(p);

    ansNet = Max(ptrOut, nbOut);
    ansTar = Max(ptrTar, nbOut);

    if (ansNet == ansTar)
      good++;
    else
      bad++;

    if (toWrite) {
      for (o = 0, ptrOut = GlobalOut; o < nbOut; o++, ptrOut++)
        // outFile << *ptrOut << " ";
        buf << *ptrOut << " ";

      // outFile << "\n";
      buf << "\n";
    }
  }
  if (toWrite) {
    cout << predFile << ": "
         << "Written.\n\n";
  }

  *accuracy = good + bad;
  *accuracy = (float)good / *accuracy;
}

///////////////////////////////////////////////////////////////////

void BagDimlp::Del()

{
  int n;

  for (n = 0; n < NbDimlpNets; n++) {
    VectData[n]->LightDel();
    VectDataClass[n]->LightDel();
    ValData[n]->LightDel();
    ValDataClass[n]->LightDel();

    VectDimlp[n]->Del();
  }

  delete VectData;
  delete VectDataClass;
  delete ValData;
  delete ValDataClass;
  delete VectDimlp;
  delete NbNeurons;
  delete GlobalOut;
}

///////////////////////////////////////////////////////////////////

BagDimlp::BagDimlp(float eta, float mu, float flat, float errParam,
                   float accuracyParam, float deltaErrParam, int discrLevels,
                   int showErrParam, int nbEpochsParam, int nbLayers,
                   int *nbNeurons, int nbDimlpNets, char weightFile[])
    :

      Dimlp(eta, mu, flat, errParam, accuracyParam, deltaErrParam, discrLevels,
            showErrParam, nbEpochsParam, nbLayers, nbNeurons, weightFile)

{
  int n;

  Eta = eta;
  Mu = mu;
  Flat = flat;
  ErrParam = errParam;
  AccuracyParam = accuracyParam;
  DeltaErrParam = deltaErrParam;
  DiscrLevels = discrLevels;
  ShowErrParam = showErrParam;
  NbEpochsParam = nbEpochsParam;
  NbLayers = nbLayers;
  NbNeurons = new int[nbLayers];
  NbDimlpNets = nbDimlpNets;
  WeightFile = weightFile;

  for (n = 0; n < nbLayers; n++)
    NbNeurons[n] = nbNeurons[n];

  cout << "Number of networks = " << nbDimlpNets << "\n\n";

  VectData = new DataSet *[nbDimlpNets];
  VectDataClass = new DataSet *[nbDimlpNets];
  ValData = new DataSet *[nbDimlpNets];
  ValDataClass = new DataSet *[nbDimlpNets];

  VectDimlp = new Dimlp *[nbDimlpNets];

  NbOut = nbNeurons[NbLayers - 1];
  GlobalOut = new float[NbOut];
}

///////////////////////////////////////////////////////////////////

BagDimlp::BagDimlp(int discrLevels, int nbLayers, int *nbNeurons,
                   int nbDimlpNets, char weightFile[])
    :

      Dimlp(0, 0, 0, 0, 0, 0, discrLevels, 0, 0, nbLayers, nbNeurons,
            weightFile)

{
  int n;

  DiscrLevels = discrLevels;
  NbLayers = nbLayers;
  NbNeurons = new int[nbLayers];
  NbDimlpNets = nbDimlpNets;
  WeightFile = weightFile;

  for (n = 0; n < nbLayers; n++)
    NbNeurons[n] = nbNeurons[n];

  cout << "Number of networks = " << nbDimlpNets << "\n\n";

  VectDimlp = new Dimlp *[nbDimlpNets];

  NbOut = nbNeurons[NbLayers - 1];
  GlobalOut = new float[NbOut];
}

///////////////////////////////////////////////////////////////////
