using namespace std;
#include <math.h>

#include "writeErr.h"
#include "bpNN.h"

#define  LLL 1
#include "layerFdp.h"

#define   LD3   1
#include "layerD3.h"

#define   LD4   1
#include "layerD4.h"

#include "layerRad.h"
#include "layerSD.h"
#include "layerSP3.h"
#include "layerSP4.h"
#include "layerSP5.h"
#include <fstream>
#include<bits/stdc++.h>

///////////////////////////////////////////////////////////////////

int BpNN::Max(float vec[], int nbEl)

{  float max    = vec[0];
   int   indMax = 0;
   int   k;

   for (k=1; k<nbEl; k++)
   {
       if (vec[k] > max)
       {
          max    = vec[k];
          indMax = k;
       }
   }

   return indMax;
}

///////////////////////////////////////////////////////////////////

void BpNN::InitRandomGen()
{
   static int initRandomGen;

   if (initRandomGen == 0)
   {
       IntRandomFunction   irf;
       FloatRandomFunction frf; 

       initRandomGen++;

       cout << "\nRandom number generator initialized.\n\n";
   }
}

///////////////////////////////////////////////////////////////////

void BpNN::CreateNetStruct(int nbNeurons[])

{  int l;

   NbNeurons = new int[NbLayers];

   for (l=0; l<NbLayers; l++)
       NbNeurons[l] = nbNeurons[l];


   VecLayer = new Layer*[NbWeightLayers];

   for (l=1; l<NbWeightLayers; l++)
       VecLayer[l] = new Layer(Eta, Mu, Flat, 
                               NbNeurons[l], NbNeurons[l+1],
                               NbNeurons[l] * NbNeurons[l+1], 
                               NbNeurons[l] + 1);


   for (l=2; l<NbWeightLayers; l++)
   {
       VecLayer[l]->SetDown(VecLayer[l-1]->GetUp());
       VecLayer[l]->SetDeltaDown(VecLayer[l-1]->GetDeltaUp());
   }
}

///////////////////////////////////////////////////////////////////

void BpNN::WriteArchParam()

{  int l;

   cout << "\n\nArchitecture: ";

   for (l=0; l<NbLayers-1; l++)
       cout << NbNeurons[l] << "-";
   cout << NbNeurons[l] << "\n\n";
}

///////////////////////////////////////////////////////////////////

void BpNN::AssignParam
(
   float eta,
   float mu,
   float flat,
   float errParam,
   float accuracyParam,
   float deltaErrParam,
   int   showErrParam,
   int   nbEpochsParam,
   int   nbLayers,
   char  saveFile[])

{
   Eta            = eta;
   Mu             = mu;
   Flat           = flat;
   ErrParam       = errParam;
   AccuracyParam  = accuracyParam;
   DeltaErrParam  = deltaErrParam;
   ShowErrParam   = showErrParam;
   NbEpochsParam  = nbEpochsParam;
   NbLayers       = nbLayers;
   strcpy(SaveFile, saveFile);

   NbWeightLayers = nbLayers-1;
}

///////////////////////////////////////////////////////////////////

void BpNN::WriteParam()
{
   cout << "Parameters:\n\n";

   cout << "Eta                   = " << Eta << "\n";
   cout << "Mu                    = " << Mu << "\n";
   cout << "Flat                  = " << Flat << "\n";

   if (ErrParam >= 0)
      cout << "Error Threshold       = " << ErrParam << "\n";

   if (AccuracyParam <= 1)
      cout << "Accuracy Threshold    = " << AccuracyParam << "\n";

   if (DeltaErrParam != 0)
      cout << "Delta Error Threshold = " << DeltaErrParam << "\n";

   cout << "Show Error            = " << ShowErrParam << "\n";

   if (NbEpochsParam < 1000000000)
      cout << "Epochs                = " << NbEpochsParam << "\n";

   cout << "\n";
}

///////////////////////////////////////////////////////////////////

void BpNN::SaveWeights()

{  int     n;
   filebuf buf;

   if (buf.open(SaveFile, ios_base::out) == 0){
      char errorMsg[] = "Cannot open file for writing";
      WriteError(errorMsg, SaveFile);
   }

   ostream outFile(&buf);

   cout << "\n\n" << SaveFile << ": " << "Writing ...\n";


   for (n=0; n<NbWeightLayers; n++)
       VecLayer[n]->WriteWeights(outFile);

   cout << SaveFile << ": " << "Written.\n\n";

}

///////////////////////////////////////////////////////////////////

void BpNN::SaveWeights(char* strSave)

{  int     n;
   filebuf buf;

   if (buf.open(strSave, ios_base::out) == 0){
      char errorMsg[] = "Cannot open file for writing";
      WriteError(errorMsg, strSave);
   }
   ostream outFile(&buf);

   cout << "\n\n" << strSave << ": " << "Writing ...\n";


   for (n=0; n<NbWeightLayers; n++)
       VecLayer[n]->WriteWeights(outFile);

   cout << strSave << ": " << "Written.\n\n";

}

///////////////////////////////////////////////////////////////////

void BpNN::ReadWeights()

{  int     n;
   filebuf buf;

   if (buf.open(ReadFile, ios_base::in) == 0){
      char errorMsg[] = "Cannot open input file ";
      WriteError(errorMsg, ReadFile);
   }

   istream inFile(&buf);

   cout << "\n\n" << ReadFile << ": " << "Reading ...\n";


   for (n=0; n<NbLayers-1; n++)
       VecLayer[n]->ReadWeights(inFile);

   cout << ReadFile << ": " << "Read.\n\n";
}

///////////////////////////////////////////////////////////////////

void BpNN::Push()

{  int l;

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->PushWeights();
}

///////////////////////////////////////////////////////////////////

void BpNN::Pop()

{  int l;

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->PopWeights();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1(DataSet& data, int index)

{  int l;

   VecLayer[0]->SetDown(data.GetExample(index));

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->ForwAndTransf1();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1(float* ex)

{  int l;

   VecLayer[0]->SetDown(ex);

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->ForwAndTransf1();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1()

{  int l;

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->ForwAndTransf1();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample2(DataSet& data, int index)

{  int l;

   VecLayer[0]->SetDown(data.GetExample(index));

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->ForwAndTransf2();
}

///////////////////////////////////////////////////////////////////

float BpNN::ComputeErrorSameAct
(
   DataSet& data,
   DataSet& target,
   float*   accuracy)

{  int    p, ansNet, ansTar;
   int    good, bad;
   float  sum;
   float* ptrOut;
   float* ptrTar;
   const int nbPat = data.GetNbEx();
   const int nbOut = target.GetNbAttr();

   for (p=0, sum=0.0, good=0, bad=0; p<nbPat; p++)
   {
       ForwardOneExample2(data, p);

       ptrOut = VecLayer[NbWeightLayers-1]->GetUp();
       ptrTar = target.GetExample(p);

       ansNet = Max(ptrOut, nbOut);
       ansTar = Max(ptrTar, nbOut);

       sum += VecLayer[NbWeightLayers-1]->HalfErrFunct(nbOut, ptrOut, ptrTar);


       if (ansNet == ansTar)
          good++;
       else 
          bad++;
   }

   *accuracy = good + bad;
   *accuracy = (float) good / *accuracy;

   return sum;
}

///////////////////////////////////////////////////////////////////

void BpNN::BackOneExample(DataSet& target, int index)

{  int l;

   VecLayer[NbWeightLayers-1]->ComputeDeltaOut(target.GetExample(index));

   for (l=NbWeightLayers-1; l > 0; l--)
       VecLayer[l]->BackLayer();

   VecLayer[0]->BackLayerWithout();
}

///////////////////////////////////////////////////////////////////

void BpNN::TrainOneEpoch(DataSet& data, DataSet& target, IntRandomFunction* r)

{  int indPat, p;

   const int nbPat = data.GetNbEx();


   for (p=0; p<nbPat; p++)
   {
       indPat = r->RandomInteger();

       ForwardOneExample2(data, indPat);
       BackOneExample(target, indPat);
   }
}

///////////////////////////////////////////////////////////////////

void BpNN::PrintSpecErrDimlp(float specErr, float specAcc)
{
   char temp[80];
   sprintf(temp, "    (SSE = %12f ", specErr);
   cout << temp;
   sprintf(temp,"   ACC = %8f)", specAcc);
   cout << temp;
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSmlp()

{  const int nbWeights        = NbNeurons[0] * NbNeurons[1];
   const int nbWeightsForInit = NbNeurons[0] + 1;

   VecLayer[0] = new Layer(Eta, Mu, Flat, 
                           NbNeurons[0], NbNeurons[1], 
                           nbWeights, nbWeightsForInit);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineDimlp(int discrLevels)

{  const int nbWeights        = NbNeurons[1];
   const int nbWeightsForInit = 2;

   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                nbWeights, nbWeightsForInit, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineQmlp(int discrLevels)

{  const int nbWeights        = NbNeurons[1];
   const int nbWeightsForInit = 2;

   VecLayer[0] = new LayerDimlp4(Eta, Mu, Flat, 
                                 NbNeurons[0], NbNeurons[1], 
                                 nbWeights, nbWeightsForInit, 
                                 discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineFdimlp(int discrLevels)

{  
   VecLayer[1] = new LayerDimlp3(Eta, Mu, Flat, 
                                 NbNeurons[1], NbNeurons[2], 
                                 NbNeurons[2], 2, 
                                 discrLevels);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

   VecLayer[0] = new LayerFdimlp(Eta, Mu, Flat, 
                                 NbNeurons[0], NbNeurons[1], 
                                 NbNeurons[1], 2);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineFdimlp2(int discrLevels)

{  
   VecLayer[1] = new LayerDimlp2(Eta, Mu, Flat, 
                                 NbNeurons[1], NbNeurons[2], 
                                 NbNeurons[2], 2, 
                                 discrLevels);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

   VecLayer[0] = new LayerFdimlp(Eta, Mu, Flat, 
                                 NbNeurons[0], NbNeurons[1], 
                                 NbNeurons[1], 2);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSD(int discrLevels)

{  
   VecLayer[1] = new LayerSD(NbNeurons[1], NbNeurons[2], 
                             NbNeurons[1] * NbNeurons[2], 
                             NbNeurons[1] + 1);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());


   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                NbNeurons[1], 2, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP5(int discrLevels)

{  
   VecLayer[1] = new LayerSP5(NbNeurons[1], NbNeurons[2], 
                              NbNeurons[1] * NbNeurons[2], 
                              NbNeurons[1] + 1);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());


   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                NbNeurons[1], 2, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP3(int discrLevels)

{  
   VecLayer[1] = new LayerSP3(NbNeurons[1], NbNeurons[2], 
                              NbNeurons[1] * NbNeurons[2], 
                              NbNeurons[1] + 1);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());


   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                NbNeurons[1], 2, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSR(int discrLevels)

{  
   VecLayer[1] = new LayerRad(NbNeurons[1], NbNeurons[2], 
                              NbNeurons[1] * NbNeurons[2], 
                              NbNeurons[1] + 1);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());


   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                NbNeurons[1], 2, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP4(int discrLevels)

{  
   VecLayer[1] = new LayerSP4(NbNeurons[1], NbNeurons[2], 
                             NbNeurons[1] * NbNeurons[2], 
                             NbNeurons[1] + 1);

   VecLayer[2]->SetDown(VecLayer[1]->GetUp());
   VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());


   VecLayer[0] = new LayerDimlp(Eta, Mu, Flat, 
                                NbNeurons[0], NbNeurons[1], 
                                NbNeurons[1], 2, 
                                discrLevels);

   VecLayer[1]->SetDown(VecLayer[0]->GetUp());
   VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

float BpNN::ComputeError
(
   DataSet& data,
   DataSet& target,
   float*   accuracy)

{  int    p, ansNet, ansTar;
   int    good, bad;
   float  sum;
   float* ptrOut;
   float* ptrTar;
   const  int nbPat = data.GetNbEx();
   const  int nbOut = target.GetNbAttr();

   for (p=0, sum=0.0, good=0, bad=0; p<nbPat; p++)
   {
       ForwardOneExample1(data, p);

       ptrOut = VecLayer[NbWeightLayers-1]->GetUp();
       ptrTar = target.GetExample(p);

       ansNet = Max(ptrOut, nbOut);
       ansTar = Max(ptrTar, nbOut);

       sum += VecLayer[NbWeightLayers-1]->HalfErrFunct(nbOut, ptrOut, ptrTar);

       if (ansNet == ansTar)
          good++;
       else 
          bad++;
   }

   *accuracy = good + bad;
   *accuracy = (float) good / *accuracy;

   return sum;
}

///////////////////////////////////////////////////////////////////

void BpNN::TrainPhase
(
   DataSet& train, 
   DataSet& trainTar, 
   DataSet& test, 
   DataSet& testTar,
   DataSet& valid,
   DataSet& validTar,
   char*    accuracyFile,
   bool     fromBT
   )


{  float oldErr = 0;
   float acc, err, specAcc, specErr, prevValidErr, validErr, accValid, testErr, accTest;
   int   e;

   IntRandomFunction randInt(0, train.GetNbEx()-1);

   err = ComputeError(train, trainTar, &acc);

   char temp[80];
   cout << "     0: ";
   sprintf(temp, " SSE = %12f ", err);
   cout << temp;
   sprintf(temp, "   ACC = %8f", acc);
   cout << temp;
   
   specErr = ComputeErrorSameAct(train, trainTar, &specAcc);          
   PrintSpecErr(specErr, specAcc);

   cout << "\n";

   if (valid.GetNbEx() != 0)
   {
      prevValidErr = ComputeError(valid, validTar, &accValid);

      cout << "Validation set: ";
      sprintf(temp, " SSE = %12f ", prevValidErr);
      cout << temp;
      sprintf(temp, "   ACC = %8f\n\n", accValid);
      cout << temp;

      Push();
   }

   if (acc >= AccuracyParam)
   {
      cout << "\n\n*** REACHED ACCURACY THRESHOLD";
      cout << " (" << AccuracyParam << ")\n\n";
      return;
   }

   if (err < ErrParam)
   {
      cout << "\n\n*** REACHED ERROR THRESHOLD";
      cout << " (" << ErrParam << ")\n\n";
      return;
   }

   if (fabs(oldErr - specErr) < DeltaErrParam)
   {
      cout << "\n\n*** REACHED VARIATION CRITERION THRESHOLD";
      cout << " (" << DeltaErrParam << ")\n\n";
      return;
   }

   for (e=1; e<=NbEpochsParam; e++)
   {
       TrainOneEpoch(train, trainTar, &randInt);

       if (e % ShowErrParam == 0) 
       {   
          err = ComputeError(train, trainTar, &acc);

          sprintf(temp, "%6d: ", e);
          cout << temp;
          sprintf(temp, " SSE = %12f ", err);
          cout << temp;
          sprintf(temp, "   ACC = %8f", acc);
          cout << temp;


          specErr = ComputeErrorSameAct(train, trainTar, &specAcc);           
          PrintSpecErr(specErr, specAcc);

          cout << "\n";

          if (valid.GetNbEx() != 0)
          {
             validErr = ComputeError(valid, validTar, &accValid);

             cout << "Validation set: ";
             sprintf(temp, " SSE = %12f ", validErr);
             cout << temp;
             sprintf(temp, "   ACC = %8f", accValid);
             cout << temp;

             if (validErr < prevValidErr)
             {
                cout << " (Better validation error, saving weights)";
                Push();
                prevValidErr = validErr;
             }

             cout << "\n\n";
          }

          if (acc >= AccuracyParam)
          {
             cout << "\n\n*** REACHED ACCURACY THRESHOLD";
             cout << " (" << AccuracyParam << ")\n\n";
             break;
          }

          if (err < ErrParam)
          {
             cout << "\n\n*** REACHED ERROR THRESHOLD";
             cout << " (" << ErrParam << ")\n\n";
             break;
          }

          if (fabs(oldErr - specErr) < DeltaErrParam)
          {
             cout << "\n\n*** REACHED VARIATION CRITERION THRESHOLD";
             cout << " (" << DeltaErrParam << ")\n\n";
             break;
          }

          oldErr = specErr;
       }
   }




   err = ComputeError(train, trainTar, &acc);

   cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << err;
   cout << "\n\n*** ACCURACY ON TRAINING SET = " << acc << "\n\n";
   if (valid.GetNbEx() != 0)
   {
      Pop(); // Get the right weights from best validation error
      validErr = ComputeError(valid, validTar, &accValid);

      cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << validErr;
      cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "\n";
   }

   if (test.GetNbEx() != 0)
   {
      testErr = ComputeError(test, testTar, &accTest);

      cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << testErr;
      cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "\n";
   }
   // Output accuracy stats in file
   if(accuracyFile != 0){
      ofstream accFile (accuracyFile, ios::app);
      if(accFile.is_open()){
         accFile << "Sum squared error on training set = " << err << "\n";
         accFile << "Accuracy on training set = " << acc << "\n\n";
         if (valid.GetNbEx() > 0){
            accFile << "Sum squared error on validation set = " << validErr << "\n";
            accFile << "Accuracy on validation set = " << accValid << "\n\n";
         }
         if (test.GetNbEx() > 0){
            accFile << "Sum squared error on testing set = " << testErr << "\n";
            accFile << "Accuracy on testing set = " << accTest << "\n\n";
         }
         accFile.close();
      }
      else{
         char errorMsg[] = "Cannot open file for writing";
         WriteError(errorMsg, accuracyFile);
      }
   }

   if (! fromBT){
      SaveWeights();
   }
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN
(
   float eta,
   float mu,
   float flat,
   float errParam,
   float accuracyParam,
   float deltaErrParam,
   int   showErrParam,
   int   nbEpochsParam,
   int   nbLayers,
   int   nbNeurons[],
   char  saveFile[],
   char  printNetType[])

{  
   InitRandomGen();

   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
   cout << "Creating " << printNetType << " structures ...";

   AssignParam(eta, mu, flat, errParam, accuracyParam, deltaErrParam, 
               showErrParam, nbEpochsParam, nbLayers, saveFile);

   CreateNetStruct(nbNeurons);

   WriteArchParam();
   WriteParam();

   cout << printNetType << " network created.\n\n";
   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN
(
   char  readFile[80],
   int   nbLayers,
   int   nbNeurons[],
   char  printNetType[])

{  
   strcpy(ReadFile, readFile);

   NbLayers       = nbLayers;
   NbWeightLayers = nbLayers-1;

   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
   cout << "Creating " << printNetType << " structures ...";

   CreateNetStruct(nbNeurons);

   WriteArchParam();

   cout << printNetType << " network created.\n\n";
   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN
(
   char  readFile[80],
   float eta,
   float mu,
   float flat,
   float errParam,
   float accuracyParam,
   float deltaErrParam,
   int   showErrParam,
   int   nbEpochsParam,
   int   nbLayers,
   int   nbNeurons[],
   char  saveFile[],
   char  printNetType[])

{  
   InitRandomGen();

   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
   cout << "Creating " << printNetType << " structures ...";


   strcpy(ReadFile, readFile);

   AssignParam(eta, mu, flat, errParam, accuracyParam, deltaErrParam, 
               showErrParam, nbEpochsParam, nbLayers, saveFile);

   CreateNetStruct(nbNeurons);

   WriteArchParam();
   WriteParam();

   cout << printNetType << " network created.\n\n";
   cout << "\n\n-----------------------------------------";
   cout << "-------------------------------------\n\n";
}

///////////////////////////////////////////////////////////////////

void BpNN::Del()

{  int l;

   delete NbNeurons;

   for (l=0; l<NbWeightLayers; l++)
       VecLayer[l]->Del();

   delete VecLayer;
}

///////////////////////////////////////////////////////////////////
