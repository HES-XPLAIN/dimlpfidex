using namespace std;
#include "DensClsFct.h"
#define BPNN 1


////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DensCls -L <training set file> ";
   cout << "-W <Prefix of file of weights> (for instance give DimlpBT) ";
   cout << "-I <number of input neurons> -O <number of output neurons> ";
   cout << "-N <number of networks>";
   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-A <file of attributes>\n";
   cout << "-T <testing set file>\n";
   cout << "-1 <file of train classes>\n";
   cout << "-2 <file of test classes>\n";
   cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
   cout << "-p <output train prediction file>\n"; // If we want to specify output train prediction file, not to be densCls.out
   cout << "-t <output test prediction file>\n"; // If we want to specify output test prediction file, not to be densClsTest.out
   cout << "-o <output file with global train and test accuracy>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-R (RULE EXTRACTION)\n";
   cout << "-F <extraction ruleFile>\n"; // If we want to extract rules in a rulesFile instead of console
   cout << "-q <number of stairs in staircase activation function>\n";

   cout << "\n-------------------------------------------------\n\n";
}


////////////////////////////////////////////////////////////

int densCls(string command){

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

   DataSet  Train;
   DataSet  Test;
   DataSet  TrainClass;
   DataSet  TestClass;
   DataSet  Valid;
   DataSet  ValidClass;
   DataSet  All;

   AttrName  Attr;

   BagDimlp* net;

   int nbDimlpNets = 0;

   int ruleExtr   = 0;

   int nbIn      = 0;
   int nbOut     = 0;
   int quant     = 50;

   char* learnFile  = 0;
   char* testFile   = 0;
   char* validFile  = 0;
   char* learnTar   = 0;
   char* testTar    = 0;
   char* validTar   = 0;
   char* attrFile   = 0;
   char* weightFile = 0;
   char* weightFileSave = (char*)"dimlp.wts";
   char* predTrainFile = (char*) "densCls.out";
   char* predTestFile = (char*) "densClsTest.out";
   char* rulesFile = 0;
   char* consoleFile = 0;
   char* accuracyFile = 0;

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
             if (commandList[k-1][1] != 'R')
             {
                cout << "Missing something at the end of the command.\n";
                return -1;
             }
          }

          switch(commandList[k-1][1])
          {

              case 'W' : weightFile = &(commandList[k])[0];
                         break;

              case 'q' : if (CheckInt(&(commandList[k])[0]))
                            quant      = atoi(&(commandList[k])[0]);
                         else return -1;

                         break;

              case 'N' : if (CheckInt(&(commandList[k])[0]))
                            nbDimlpNets = atoi(&(commandList[k])[0]);
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

              case 'A' : attrFile   = &(commandList[k])[0];
                         break;

              case 'L' : learnFile  = &(commandList[k])[0];
                         break;

              case 'T' : testFile   = &(commandList[k])[0];
                         break;

              case 'r' : consoleFile = &(commandList[k])[0];
                         break;

              case 'p' : predTrainFile = &(commandList[k])[0];
                         break;

              case 't' : predTestFile = &(commandList[k])[0];
                         break;

              case 'o' : accuracyFile = &(commandList[k])[0];
                         break;

              case '1' : learnTar   = &(commandList[k])[0];
                         break;

              case '2' : testTar    = &(commandList[k])[0];
                         break;

              case 'R' : ruleExtr   = 1;
                         k--;
                         break;

              case 'F' : rulesFile = &(commandList[k])[0];
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

   // Get console results to file
   std::ofstream ofs;
   std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
   if (consoleFile != 0){
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf());  // redirect std::cout to file
   }
   std::ostream& output = consoleFile != 0 ? ofs : std::cout;


   if (quant == 0)
   {
      cout << "The number of quantized levels must be greater than 0.\n";
      return -1;
   }

   if (nbIn == 0)
   {
      cout << "The number of input neurons must be given with option -I.\n";
      return -1;
   }

   if (nbOut == 0)
   {
      cout << "The number of output neurons must be given with option -O.\n";
      return -1;
   }

   if (weightFile == 0)
   {
      cout << "Give a file of weights with -W selection please." << "\n";
      return -1;
   }

   if (nbDimlpNets == 0)
   {
      cout << "Give the number of networks with -W selection please." << "\n";
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


   net = new BagDimlp(quant, nbLayers, vecNbNeurons, nbDimlpNets, weightFileSave);


   net->DefNetsWithWeights(weightFile);


   float acc, accTest;

   net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile);
   cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n\n";

   if (Test.GetNbEx() != 0)
   {
      net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile);
      cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "\n";
   }

   // Output accuracy stats in file
   if(accuracyFile != 0){
      ofstream accFile (accuracyFile);
      if(accFile.is_open()){
         accFile << "Global accuracy on training set = " << acc << "\n";
         if (Test.GetNbEx() != 0){
            accFile << "Global accuracy on testing set = " << accTest;
         }
         accFile.close();
      }
      else{
         cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
         return -1;
      }
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

/*
      if (Test.GetNbEx() > 0)
      {
         static DataSet all(Train, Test);
         All = all;
      }

      else
*/
         All = Train;

      if (Valid.GetNbEx() > 0)
      {
         static DataSet all2(All, Valid);
         All = all2;
      }

      cout << "\n\n****************************************************\n\n";
      cout << "*** RULE EXTRACTION\n";

      VirtualHyp* globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                 vecNbNeurons[1] / nbIn);

      RealHyp ryp (globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                   All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                   nbWeightLayers);

      if (rulesFile != 0){
         filebuf buf;

         if (buf.open(rulesFile, ios_base::out) == 0){
            char errorMsg[] = "Cannot open file for writing";
            WriteError(errorMsg, rulesFile);
         }

         ostream rulesFileost(&buf);
         ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                             Test, TestClass, Attr, rulesFileost);

         if (ryp.TreeAborted())
         {
            ryp.Del();

            VirtualHyp* globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                      vecNbNeurons[1] / nbIn);

            RealHyp2 ryp2(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                          Test, TestClass, Attr, rulesFileost);

            ryp2.Del();
         }
         else
            ryp.Del();

         cout << "\n\n" << rulesFile << ": " << "Written.\n\n";
      }
      else{
         ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                             Test, TestClass, Attr, cout);

         if (ryp.TreeAborted())
         {
            ryp.Del();

            VirtualHyp* globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                      vecNbNeurons[1] / nbIn);

            RealHyp2 ryp2(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                          Test, TestClass, Attr, cout);

            ryp2.Del();
         }
         else
            ryp.Del();
      }

      if (attrFile != 0) Attr.Del();
   }

   std::cout.rdbuf(cout_buff); // reset to standard output again

   Train.Del();
   TrainClass.Del();

   //net->Del();

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

    return 0;
}
/*
int main(int nbParam, char** param)
{
    densCls("DensCls -L ../dimlp/datafiles/datanormTrain -1 ../dimlp/datafiles/dataclass2Train -T ../dimlp/datafiles/datanormTest -2 ../dimlp/datafiles/dataclass2Test -I 16 -H2 5 -O 2 -N 2 -W ../dimlp/datafiles/dimlpDatanormBT -R -F ../dimlp/datafiles/dimlpDatanormDensClsRul.rls -p ../dimlp/datafiles/dimlpDatanormDensClsTrain.out -t ../dimlp/datafiles/dimlpDatanormDensClsTest.out -o ../dimlp/datafiles/dimlpDatanormDensClsStats -r ../dimlp/datafiles/dimlpDatanormDensClsResult.txt");
}
*/
