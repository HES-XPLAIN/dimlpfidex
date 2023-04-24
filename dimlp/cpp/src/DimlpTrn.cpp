using namespace std;
#include "dimlp.h"

#define BPNN 1

#include "realHyp2.h"
#include "checkFun.h"
#include "writeErr.h"

#include <stdlib.h>
#include <string.h>
#include <fstream>

#define  HidFile "dimlp.hid"


////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DimlpTrn -L <training set file> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";
   cout << " <Options>\n\n";
   
   cout << "Options are: \n\n";
   cout << "-A <file of attributes>\n";
   cout << "-V <validation set file>\n";
   cout << "-T <testing set file>\n";
   cout << "-W <file of pretrained weights>\n";
   cout << "-1 <file of train classes>\n";
   cout << "-2 <file of test classes>\n";
   cout << "-3 <file of validation classes>\n";
   cout << "-w <output weight file>\n"; // If we want to specify weight output file, not to be weights.wts
   cout << "-p <output train prediction file>\n"; // If we want to specify output train prediction file, not to be dimlp.out
   cout << "-t <output test prediction file>\n"; // If we want to specify output test prediction file, not to be dimlpTest.out
   cout << "-v <output validation prediction file>\n"; // If we want to specify output test prediction file, not to be dimlpValidation.out
   cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
   cout << "-o <output file with train, test and validation accuracy>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-R (RULE EXTRACTION)\n";
   cout << "-F <extraction ruleFile>\n"; // If we want to extract rules in a rulesFile instead of console
   cout << "-l <back-propagation learning parameter (Eta)>\n";
   cout << "-m <back-propagation momentum parameter (Mu)>\n";
   cout << "-f <back-propagation flat spot elimination parameter (Flat)>\n";
   cout << "-q <number of stairs in staircase activation function>\n";
   cout << "-e <error threshold>\n";
   cout << "-a <accuracy threshold>\n";
   cout << "-d <absolute difference error threshold>\n";
   cout << "-i <number of epochs>\n";
   cout << "-s <number of epochs to show error>\n";

   cout << "\n-------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////


void SaveOutputs
(
   DataSet& data, 
   Dimlp*   net,
   int      nbOut, 
   int      nbWeightLayers,
   char*    outfile
)

{  int     p, o;
   filebuf buf;

   if (buf.open(outfile, ios_base::out) == 0){
      char errorMsg[] = "Cannot open file for writing";
      WriteError(errorMsg, outfile);
   }

   Layer* layer = net->GetLayer(nbWeightLayers-1);
   float* out   = layer->GetUp();

   cout << "\n\n" << outfile << ": " << "Writing ...\n";

   ostream outFile(&buf);

   for (p=0; p<data.GetNbEx(); p++)
   {
       net->ForwardOneExample1(data, p);

       for (o=0; o<nbOut; o++)
       {
           outFile << out[o] << " ";
       }
 
       outFile << "\n";
   }

   cout << outfile << ": " << "Written.\n\n";
}

////////////////////////////////////////////////////////////

int main(int nbParam, char** param)

{  
   

   
   int k;

   DataSet  Train;
   DataSet  Test;
   DataSet  TrainClass;
   DataSet  TestClass;
   DataSet  Valid;
   DataSet  ValidClass;
   DataSet  All;

   AttrName Attr;

   Dimlp* net;

   float eta      = 0.1;
   float mu       = 0.6;
   float flat     = 0.01;
   float errThres = -1111111111.0;
   float accThres = 11111111111111.0;
   float deltaErr = 0;
   int   showErr  = 10;
   int   epochs   = 1500;
   int   quant    = 50;
   
   int ruleExtr   = 0;

   int nbIn  = 0;
   int nbOut = 0;

   char* learnFile  = 0;
   char* testFile   = 0;
   char* validFile  = 0;
   char* weightFile = 0;
   char* outputWeightFile = (char*) "dimlp.wts";
   char* predTrainFile = (char*) "dimlp.out";
   char* predTestFile = (char*) "dimlpTest.out";
   char* predValidationFile = (char*) "dimlpValidation.out";
   char* rulesFile = 0;
   char* consoleFile = 0;
   char* accuracyFile = 0;
   char* learnTar   = 0;
   char* testTar    = 0;
   char* validTar   = 0;
   char* attrFile   = 0;

   int   flagEp = 0;

   int       nbLayers;
   int       nbWeightLayers;
   int*      vecNbNeurons;

   StringInt arch;
   StringInt archInd;

   if (nbParam == 1)
   {
      GiveAllParam();
      return -1;
   }

   for (k=1; k<nbParam; k++)
   {
       if (*param[k] == '-')
       {
          k++;

          if (k >= nbParam)
          {
             if (*(param[k-1] + 1) != 'R')
             {
                cout << "Missing something at the end of the command.\n";
                return -1;
             } 
          }

          switch(*(param[k-1] + 1))
          {
              case 'l' : if (CheckFloat(param[k])) 
                            eta        = atof(param[k]);
                         else return -1;

                         break;

              case 'm' : if (CheckFloat(param[k]))
                            mu         = atof(param[k]);
                         else return -1;

                         break;

              case 'f' : if (CheckFloat(param[k]))
                            flat       = atof(param[k]);
                         else return -1;

                         break;

              case 'e' : if (CheckFloat(param[k]))
                            errThres   = atof(param[k]);
                         else return -1;

                         if (flagEp == 0) 
                            epochs = 2000000000;
                         break;

              case 'a' : if (CheckFloat(param[k]))
                            accThres   = atof(param[k]);
                         else return -1;

                         if (flagEp == 0) 
                            epochs = 2000000000;
                         break;

              case 'd' : if (CheckFloat(param[k]))
                            deltaErr   = atof(param[k]);
                         else return -1;

                         if (flagEp == 0) 
                            epochs = 2000000000;
                         break;

              case 's' : if (CheckInt(param[k]))
                            showErr    = atoi(param[k]);
                         else return -1;

                         break;

              case 'i' : if (CheckInt(param[k]))
                         {
                            epochs = atoi(param[k]);
                            flagEp = 1;
                         }
                         else return -1;

                         break;

              case 'q' : if (CheckInt(param[k]))
                            quant      = atoi(param[k]);
                         else return -1;

                         break;

              case 'I' : if (CheckInt(param[k]))
                            nbIn       = atoi(param[k]);
                         else return -1;

                         break;

              case 'H' : if (CheckInt(param[k]))
                         {
                            arch.Insert(atoi(param[k]));

                            char* ptrParam = param[k-1];

                            if (ptrParam[2] != '\0')
                            {
                               char str[80];

                               strcpy(str, ptrParam+2);
                               archInd.Insert(atoi(str));
                            }
                            else
                            {
                               cout << "Which hidden layer (-H) ?\n";
                               return -1;
                            }
                         }
                         else return -1;

                         break;

              case 'O' : if (CheckInt(param[k]))
                            nbOut      = atoi(param[k]);
                         else return -1;

                         break;

              case 'A' : attrFile   = param[k];
                         break;

              case 'W' : weightFile = param[k];
                         break;

              case 'w' : outputWeightFile = param[k];
                         break;

              case 'p' : predTrainFile = param[k];
                         break;

              case 't' : predTestFile = param[k];
                         break;

              case 'v' : predValidationFile = param[k];
                         break;

              case 'r' : consoleFile = param[k];
                         break;

              case 'o' : accuracyFile = param[k];
                         break;      

              case 'L' : learnFile  = param[k];
                         break;

              case 'T' : testFile   = param[k];
                         break;

              case 'V' : validFile   = param[k];
                         break;

              case '1' : learnTar   = param[k];
                         break;

              case '2' : testTar    = param[k];
                         break;

              case '3' : validTar    = param[k];
                         break;

              case 'R' : ruleExtr   = 1;
                         k--;
                         break;

              case 'F' : rulesFile = param[k];
                         break;

              default  : cout << "Illegal option: " << param[k-1] << "\n";
                         return -1;
          }
       }

       else
       {
          cout << "Illegal option: " << param[k] << "\n";
          return -1;
       }
   }

   // Get console results to file
   std::ofstream ofs;
   std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
   if (consoleFile != 0){
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf());  // redirect std::cout to file
   }
   std::ostream& output = consoleFile != 0 ? ofs : std::cout;

   if (eta <=0)
   {
      cout << "The learning parameter must be greater than 0.\n";
      return -1;
   }

   if (mu < 0)
   {
      cout << "The momentum parameter must be greater or equal to 0.\n";
      return -1;
   }

   if (showErr == 0)
   {
      cout << "The number of epochs must be greater than 0.\n";
      return -1;
   }

   if (quant <= 2)
   {
      cout << "The number of quantized levels must be greater than 2.\n";
      return -1;
   }

   if (nbIn == 0)
   {
      cout << "The number of input neurons must be given with option -I.\n";
      return -1;
   }

   if (nbOut <= 1)
   {
      cout << "At least two output neurons must be given with option -O.\n";
      return -1;
   }

// ----------------------------------------------------------------------

   if (arch.GetNbEl() == 0)
   {
      nbLayers       = 3;
      nbWeightLayers = nbLayers - 1;

      vecNbNeurons    = new int[nbLayers];
      vecNbNeurons[0] = nbIn;
      vecNbNeurons[1] = nbIn;
      vecNbNeurons[2] = nbOut;
   }

   else
   {
      archInd.GoToBeg();

      if (archInd.GetVal() == 1)
      {
         arch.GoToBeg();

         if (arch.GetVal() % nbIn != 0)
         {
            cout << "The number of neurons in the first hidden layer must be";
            cout << " a multiple of the number of input neurons.\n";
            return -1;
         }

         nbLayers       = arch.GetNbEl() + 2;
         nbWeightLayers = nbLayers - 1;

         vecNbNeurons             = new int[nbLayers];
         vecNbNeurons[0]          = nbIn;
         vecNbNeurons[nbLayers-1] = nbOut;

         for (k=1, arch.GoToBeg(); k<=arch.GetNbEl(); k++, arch.GoToNext())
         {
             vecNbNeurons[k] = arch.GetVal();
 
             if (vecNbNeurons[k] == 0) 
             {
                cout << "The number of neurons must be greater than 0.\n";
                return -1;
             }
         }
      }

      else
      {
         nbLayers       = arch.GetNbEl() + 3;
         nbWeightLayers = nbLayers - 1;

         vecNbNeurons             = new int[nbLayers];
         vecNbNeurons[0]          = nbIn;
         vecNbNeurons[1]          = nbIn;
         vecNbNeurons[nbLayers-1] = nbOut;

         for (k=1, arch.GoToBeg(); k<=arch.GetNbEl(); k++, arch.GoToNext())
         {
             vecNbNeurons[k+1] = arch.GetVal();
 
             if (vecNbNeurons[k+1] == 0) 
             {
                cout << "The number of neurons must be greater than 0.\n";
                return -1;
             }
         }

      }
   }

// ----------------------------------------------------------------------

   if (learnFile == 0)
   {
      cout << "Give the training file with -L selection please." << "\n";
      return -1;
   }

   if (learnTar != 0)
   {
      static DataSet train(learnFile, nbIn);
      static DataSet trainClass(learnTar, nbOut);

      Train      = train;
      TrainClass = trainClass;
   }
   else
   {
      DataSet data(learnFile, nbIn + nbOut);

      static DataSet train(data.GetNbEx());
      static DataSet trainClass(data.GetNbEx());

      data.ExtractDataAndTarget(train, nbIn, trainClass, nbOut);

      Train      = train;
      TrainClass = trainClass;

      data.Del();
   }

   if (validFile != 0)
   {
      if (validTar != 0)
      {
         static DataSet valid(validFile, nbIn);
         static DataSet validClass(validTar, nbOut);

         Valid      = valid;
         ValidClass = validClass;
      }

      else
      {
         DataSet data(validFile, nbIn + nbOut);

         static DataSet valid(data.GetNbEx());
         static DataSet validClass(data.GetNbEx());

         data.ExtractDataAndTarget(valid, nbIn, validClass, nbOut);

         Valid      = valid;
         ValidClass = validClass;

         data.Del();
      }
   }

   if (testFile != 0)
   {
      if (testTar != 0)
      {
         static DataSet test(testFile, nbIn);
         static DataSet testClass(testTar, nbOut);

         Test      = test;
         TestClass = testClass;
      }

      else
      {
         DataSet data(testFile, nbIn + nbOut);
         static DataSet test(data.GetNbEx());
         static DataSet testClass(data.GetNbEx());

         data.ExtractDataAndTarget(test, nbIn, testClass, nbOut);

         Test      = test;
         TestClass = testClass;

         data.Del();
      }
   }
   if (weightFile == 0)
      net = new Dimlp(eta, mu, flat, errThres, accThres, deltaErr, 
                      quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile);

   else
      net = new Dimlp(weightFile, eta, mu, flat, errThres, accThres, 
                      deltaErr, quant, showErr, epochs, 
                      nbLayers, vecNbNeurons, outputWeightFile);

   if(accuracyFile != 0){
      ofstream accFile (accuracyFile);
      if(accFile.is_open()){
         accFile << "Accuracy : \n\n";
         accFile.close();
      }
      else{
         char errorMsg[] = "Cannot open file for writing";
         WriteError(errorMsg, accuracyFile);
      }
   }

   net->Train(Train, TrainClass, Test, TestClass, Valid, ValidClass, accuracyFile);

   SaveOutputs(Train, net, nbOut, nbWeightLayers, predTrainFile); // Get train predictions
   if (testFile != 0){
      SaveOutputs(Test, net, nbOut, nbWeightLayers, predTestFile); // Get test predictions
   }
   if (validFile != 0){
      SaveOutputs(Valid, net, nbOut, nbWeightLayers, predValidationFile); // Get test predictions
   }
   if (ruleExtr)
   {
      if (attrFile != 0)
      {
         AttrName attr(attrFile, nbIn, nbOut);

         if (attr.ReadAttr())
            cout << "\n\n" << attrFile << ": Read file of attributes.\n\n";

         Attr = attr;
      }

      All = Train;
      if (rulesFile != 0){
         cout << "Extraction Part :: " << endl;
      }

      if (Valid.GetNbEx() > 0)
      {
         static DataSet all2(All, Valid);
         All = all2;
      }

      cout << "\n\n****************************************************\n\n";
      cout << "*** RULE EXTRACTION\n";

      RealHyp ryp1(All, net, quant, nbIn, 
                  vecNbNeurons[1] / nbIn, nbWeightLayers);


      if (rulesFile != 0){
         filebuf buf;

         if (buf.open(rulesFile, ios_base::out) == 0){
            char errorMsg[] = "Cannot open file for writing";
            WriteError(errorMsg, rulesFile);
         }

         ostream rulesFileost(&buf);

         
         ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass, 
                                             Test, TestClass, Attr, rulesFileost);

         if (ryp1.TreeAborted())
         {
            ryp1.Del();

            RealHyp2 ryp2(All, net, quant, nbIn, 
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass, 
                                    Test, TestClass, Attr, rulesFileost);

            ryp2.Del();
         }
         else
            ryp1.Del();

         cout << "\n\n" << rulesFile << ": " << "Written.\n\n";
      }

      else{
         ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass, 
                                             Test, TestClass, Attr, cout);

         if (ryp1.TreeAborted())
         {
            ryp1.Del();

            RealHyp2 ryp2(All, net, quant, nbIn, 
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass, 
                                    Test, TestClass, Attr, cout);

            ryp2.Del();
         }
         else
            ryp1.Del();
      }

      if (attrFile != 0) Attr.Del();
   }
         

 
   Train.Del();
   TrainClass.Del();

   net->Del();

   if (Test.GetNbEx() > 0)
   {
      Test.Del();
      TestClass.Del();
   }

   if (Valid.GetNbEx() > 0)
   {
      Valid.Del();
      ValidClass.Del();
   }

   std::cout.rdbuf(cout_buff); // reset to standard output again
}