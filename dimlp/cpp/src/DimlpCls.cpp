using namespace std;
#include "dimlp.h"
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

   cout << "DimlpCls -T <file of examples> ";
   cout << "-W <file of weights> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";
   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-2 <file of classes>\n";
   cout << "-p <output prediction file>\n"; // If we want to specify output prediction file, not to be dimlp.out
   cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
   cout << "-o <output file with test accuracy>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-q <number of stairs in staircase activation function>\n";

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

void SaveFirstHid
(
   DataSet& data,
   Dimlp*   net,
   int      nbHid,
   char*    outfile
)

{  int     p, h;
   filebuf buf;

   if (buf.open(HidFile, ios_base::out) == 0){
      char errorMsg[] = "Cannot open file for writing";
      WriteError(errorMsg, outfile);
   }

   Layer* layer = net->GetLayer(0);
   float* hid   = layer->GetUp();

   cout << "\n\n" << HidFile << ": " << "Writing ...\n";

   ostream outFile(&buf);

   for (p=0; p<data.GetNbEx(); p++)
   {
       net->ForwardOneExample1(data, p);

       for (h=0; h<nbHid; h++)
       {
           outFile << hid[h] << " ";
       }

       outFile << "\n";
   }

   cout << HidFile << ": " << "Written.\n\n";
}

////////////////////////////////////////////////////////////

int main(int nbParam, char** param)

{  int k;

   DataSet  Test;
   DataSet  TestClass;

   int quant = 50;
   int nbIn  = 0;
   int nbOut = 0;

   char* testFile   = 0;
   char* weightFile = 0;
   char* predFile = (char*) "dimlp.out";
   char* consoleFile = 0;
   char* accuracyFile = 0;
   char* testTar    = 0;

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
             cout << "Missing something at the end of the command.\n";
             return -1;
          }

          switch(*(param[k-1] + 1))
          {
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

              case 'W' : weightFile = param[k];
                         break;

              case 'p' : predFile = param[k];
                         break;

              case 'r' : consoleFile = param[k];
                         break;

              case 'o' : accuracyFile = param[k];
                         break;

              case 'T' : testFile   = param[k];
                         break;

              case '2' : testTar    = param[k];
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


   if (testFile == 0)
   {
      cout << "Give a testing file with -T selection please." << "\n";
      return -1;
   }

   else // if (testFile != 0)
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
   {
      cout << "Give a file of weights with -W selection please." << "\n";
      return -1;
   }

   Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

   float acc;

   float err = net.Error(Test, TestClass, &acc);

   cout << "\n\n*** SUM SQUARED ERROR = " << err;
   cout << "\n\n*** ACCURACY = " << acc << "\n";

   // Output accuracy stats in file
   if(accuracyFile != 0){
      ofstream accFile (accuracyFile);
      if(accFile.is_open()){
         accFile << "Sum squared error = " << err << "\n";
         accFile << "Accuracy = " << acc;
         accFile.close();
      }
      else{
         cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
         return -1;
      }
   }

   SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);
   SaveFirstHid(Test, &net, vecNbNeurons[1], predFile);

   Test.Del();
   TestClass.Del();

   cout << "\n-------------------------------------------------\n\n";

   std::cout.rdbuf(cout_buff); // reset to standard output again
}
