using namespace std;
#include "DimlpClsFct.h"

////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DimlpCls -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
   cout << "-T <file of examples(path with respect to specified root folder)> ";
   cout << "-W <file of weights> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";
   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-2 <file of classes>\n";
   cout << "-p <output prediction file>\n"; // If we want to specify output prediction file, not to be dimlp.out
   cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
   cout << "-o <output file with test accuracy>\n";
   cout << "-h <output file with first hidden layer values>\n"; // Not to be dimlp.hid
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
   char*    outfile,
   char*    firsthidFile
)

{  int     p, h;
   filebuf buf;

   if (buf.open(firsthidFile, ios_base::out) == 0){
      char errorMsg[] = "Cannot open file for writing";
      WriteError(errorMsg, outfile);
   }

   Layer* layer = net->GetLayer(0);
   float* hid   = layer->GetUp();

   cout << "\n\n" << firsthidFile << ": " << "Writing ...\n";

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

   cout << firsthidFile << ": " << "Written.\n\n";
}





////////////////////////////////////////////////////////////

int dimlpCls(string command){


    // Parsing the command
    vector<string> commandList;
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (std::getline(ss, s, delim)) {
            commandList.push_back(s);
        }
    int nbParam = commandList.size();

   int k;

   DataSet  Test;
   DataSet  TestClass;

   int quant = 50;
   int nbIn  = 0;
   int nbOut = 0;

   string testFileTemp;
   bool testFileInit = false;
   string weightFileTemp;
   bool weightFileInit = false;
   string predFileTemp = "dimlp.out";
   string consoleFileTemp;
   bool consoleFileInit = false;
   string accuracyFileTemp;
   bool accuracyFileInit = false;
   string testTarTemp;
   bool testTarInit = false;
   string hidFileTemp = "dimlp.hid";
   string rootFolderTemp;
   bool rootFolderInit = false;

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
       if (commandList[k][0] == '-')
       {
          k++;

          if (k >= nbParam)
          {
             cout << "Missing something at the end of the command.\n";
             return -1;
          }

          switch(commandList[k-1][1])
          {
              case 'q' : if (CheckInt(&(commandList[k])[0]))
                            quant      = atoi(&(commandList[k])[0]);
                         else return -1;

                         break;

              case 'I' : if (CheckInt(&(commandList[k])[0]))
                            nbIn       = atoi(&(commandList[k])[0]);
                         else return -1;

                         break;

              case 'H' : if (CheckInt(&(commandList[k])[0]))
                         {
                            arch.Insert(atoi(&(commandList[k])[0]));

                            char* ptrParam = &(commandList[k-1])[0];

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

              case 'O' : if (CheckInt(&(commandList[k])[0]))
                            nbOut      = atoi(&(commandList[k])[0]);
                         else return -1;

                         break;

              case 'S' :
                         rootFolderTemp = &(commandList[k])[0];
                         rootFolderInit = true;
                         break;

              case 'W' :
                         weightFileTemp = &(commandList[k])[0];
                         weightFileInit = true;
                         break;

              case 'p' :
                         predFileTemp = &(commandList[k])[0];
                         break;

              case 'r' :
                         consoleFileTemp = &(commandList[k])[0];
                         consoleFileInit = true;
                         break;

              case 'o' :
                         accuracyFileTemp = &(commandList[k])[0];
                         accuracyFileInit = true;
                         break;

               case 'h' :
                         hidFileTemp = &(commandList[k])[0];
                         break;

              case 'T' :
                         testFileTemp   = &(commandList[k])[0];
                         testFileInit = true;
                         break;

              case '2' :
                         testTarTemp    = &(commandList[k])[0];
                         testTarInit = true;
                         break;

              default  : cout << "Illegal option: " << &(commandList[k-1])[0] << "\n";
                         return -1;
          }
       }

       else
       {
          cout << "Illegal option: " << &(commandList[k])[0] << "\n";
          return -1;
       }
   }

// ----------------------------------------------------------------------

   // create paths with root foler
   char testFileTmp[160], weightFileTmp[160], predFileTmp[160], consoleFileTmp[160], accuracyFileTmp[160], testTarTmp[160], hidFileTmp[160];

   char* testFile = 0;
   char* weightFile = 0;
   char* predFile = 0;
   char* consoleFile = 0;
   char* accuracyFile = 0;
   char* testTar = 0;
   char* hidFile = 0;

   #ifdef __unix__
   string root = rootFolderTemp + "/";
   #elif defined(_WIN32)
   string root = rootFolderTemp + "\\";
   #endif

   if (testFileInit){
      testFileTemp = root + testFileTemp;
      if(testFileTemp.length()>=160){
         cout << "Path " << testFileTemp << "is too long" << "\n";
         return -1;
      }
      strcpy(testFileTmp, testFileTemp.c_str());
      testFile = testFileTmp;
   }

   if (weightFileInit){
      weightFileTemp = root + weightFileTemp;
      if(weightFileTemp.length()>=160){
         cout << "Path " << weightFileTemp << "is too long" << "\n";
         return -1;
      }
      strcpy(weightFileTmp, weightFileTemp.c_str());
      weightFile = weightFileTmp;
   }

   predFileTemp = root + predFileTemp;
   if(predFileTemp.length()>=160){
      cout << "Path " << predFileTemp << "is too long" << "\n";
      return -1;
   }
   strcpy(predFileTmp, predFileTemp.c_str());
   predFile = predFileTmp;

   if(consoleFileInit){
      consoleFileTemp = root + consoleFileTemp;
      if(consoleFileTemp.length()>=160){
         cout << "Path " << consoleFileTemp << "is too long" << "\n";
         return -1;
      }
      strcpy(consoleFileTmp, consoleFileTemp.c_str());
      consoleFile = consoleFileTmp;
   }

   if(accuracyFileInit){
      accuracyFileTemp = root + accuracyFileTemp;
      if(accuracyFileTemp.length()>=160){
         cout << "Path " << accuracyFileTemp << "is too long" << "\n";
         return -1;
      }
      strcpy(accuracyFileTmp, accuracyFileTemp.c_str());
      accuracyFile = accuracyFileTmp;
   }

   if(testTarInit){
      testTarTemp = root + testTarTemp;
      if(testTarTemp.length()>=160){
         cout << "Path " << testTarTemp << "is too long" << "\n";
         return -1;
      }
      strcpy(testTarTmp, testTarTemp.c_str());
      testTar = testTarTmp;
   }


   hidFileTemp = root + hidFileTemp;
   if(hidFileTemp.length()>=160){
      cout << "Path " << hidFileTemp << "is too long" << "\n";
      return -1;
   }
   strcpy(hidFileTmp, hidFileTemp.c_str());
   hidFile = hidFileTmp;

// ----------------------------------------------------------------------
   // Get console results to file
   std::ofstream ofs;
   std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
   if (consoleFileInit != false){
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf());  // redirect std::cout to file
   }
   std::ostream& output = consoleFileInit != false ? ofs : std::cout;

// ----------------------------------------------------------------------
   if (rootFolderInit == false)
   {
      cout << "Give a root folder to save results with -S selection please." << "\n";
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


   if (testFileInit == false)
   {
      cout << "Give a testing file with -T selection please." << "\n";
      return -1;
   }

   else // if (testFileInit != false)
   {
      if (testTarInit != false)
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
   if (weightFileInit == false)
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
   if(accuracyFileInit != false){
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
   SaveFirstHid(Test, &net, vecNbNeurons[1], predFile, hidFile);

   Test.Del();
   TestClass.Del();

   cout << "\n-------------------------------------------------\n\n";

   std::cout.rdbuf(cout_buff); // reset to standard output again

   return 0;
}

/*
int main(int nbParam, char** param)

{
    dimlpCls("DimlpCls -T ../dimlp/datafiles/datanormTest -2 ../dimlp/datafiles/dataclass2Test -W ../dimlp/datafiles/dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p ../dimlp/datafiles/dimlpDatanormTest.out -o ../dimlp/datafiles/dimlpDatanormClsStats -r ../dimlp/datafiles/dimlpDatanormClsResult.txt");
}*/
