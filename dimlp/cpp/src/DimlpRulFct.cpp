using namespace std;
#include "DimlpRulFct.h"
#define BPNN 1

////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DimlpRul -L <training set file> ";
   cout << "-W <file of weights> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";
   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-A <file of attributes>\n";
   cout << "-V <validation set file>\n";
   cout << "-T <testing set file>\n";
   cout << "-1 <file of train classes>\n";
   cout << "-2 <file of test classes>\n";
   cout << "-3 <file of validation classes>\n";
   cout << "-R <rulesFile>\n"; // If you want to specify the rulesFile not being dimlp.rls
   cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
   cout << "-o <output file with train, test and validation accuracy>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-q <number of stairs in staircase activation function>\n";

   cout << "\n-------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////
#define JOBLOC "/home/ubuntu/job/"
int dimlpRul(string command){

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

   AttrName Attr;

   int quant = 50;
   int nbIn  = 0;
   int nbOut = 0;

   char* learnFile  = 0;
   char* testFile   = 0;
   char* validFile  = 0;
   char* weightFile = 0;
   char* learnTar   = 0;
   char* testTar    = 0;
   char* validTar   = 0;
   char*  rulesFile = (char*) "dimlp.rls";
   char* consoleFile = 0;
   char* accuracyFile = 0;
   char* attrFile   = 0;
   int   nbLayers;
   int   nbWeightLayers;
   int*  vecNbNeurons;

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

              case 'A' : attrFile   = &(commandList[k])[0];
                         break;

              case 'W' : weightFile = &(commandList[k])[0];
                         break;

              case 'L' : learnFile  = &(commandList[k])[0];
                         break;

              case 'T' : testFile   = &(commandList[k])[0];
                         break;

              case 'V' : validFile   = &(commandList[k])[0];
                         break;

              case '1' : learnTar   = &(commandList[k])[0];
                         break;

              case '2' : testTar    = &(commandList[k])[0];
                         break;

              case '3' : validTar    = &(commandList[k])[0];
                         break;

              case 'R' : rulesFile = &(commandList[k])[0];
                         break;

              case 'r' : consoleFile = &(commandList[k])[0];
                         break;

              case 'o' : accuracyFile = &(commandList[k])[0];
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

   if (weightFile == 0)
   {
      cout << "Give a file of weights with -W selection please." << "\n";
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

// ----------------------------------------------------------------------

   Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

   float accTrain, errTrain, accValid, errValid, accTest, errTest;
   errTrain = net.Error(Train, TrainClass, &accTrain);

   cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << errTrain;
   cout << "\n\n*** ACCURACY ON TRAINING SET = " << accTrain << "\n";

   if (Valid.GetNbEx() > 0)
   {
      errValid = net.Error(Valid, ValidClass, &accValid);

      cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << errValid;
      cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "\n";
   }

   if (Test.GetNbEx() > 0)
   {
      errTest = net.Error(Test, TestClass, &accTest);

      cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << errTest;
      cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "\n";
   }

   // Output accuracy stats in file
   if(accuracyFile != 0){
      ofstream accFile (accuracyFile);
      if(accFile.is_open()){
         accFile << "Sum squared error on training set = " << errTrain << "\n";
         accFile << "Accuracy on training set = " << accTrain << "\n\n";
         if (Valid.GetNbEx() > 0){
            accFile << "Sum squared error on validation set = " << errValid << "\n";
            accFile << "Accuracy on validation set = " << accValid << "\n\n";
         }
         if (Test.GetNbEx() > 0){
            accFile << "Sum squared error on testing set = " << errTest << "\n";
            accFile << "Accuracy on testing set = " << accTest << "\n\n";
         }
         accFile.close();
      }
      else{
         cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
         return -1;
      }
   }

   cout << "\n-------------------------------------------------\n\n";


// ----------------------------------------------------------------------

   All = Train;

   cout << "Extraction Part :: " << endl;

   if (Valid.GetNbEx() > 0)
   {
      static DataSet all2(All, Valid);
      All = all2;
   }

   cout << "\n\n****************************************************\n\n";
   cout << "*** RULE EXTRACTION\n";

   if (attrFile != 0)
   {
      AttrName attr(attrFile, nbIn, nbOut);

      if (attr.ReadAttr())
         cout << "\n\n" << attrFile << ": Read file of attributes.\n\n";

      Attr = attr;
   }

   RealHyp ryp(All, &net, quant, nbIn,
               vecNbNeurons[1] / nbIn, nbWeightLayers);

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
      RealHyp2 ryp2(All, &net, quant, nbIn,
                   vecNbNeurons[1] / nbIn, nbWeightLayers);

      ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                      Test, TestClass, Attr, rulesFileost);

      ryp2.Del();
   }

   cout << "\n\n" << rulesFile << ": " << "Written.\n\n";

   std::cout.rdbuf(cout_buff); // reset to standard output again

   ryp.Del();

   if (attrFile != 0) Attr.Del();

   TrainClass.Del();

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
    dimlpRul("DimlpRul -L ../dimlp/datafiles/datanormTrain -1 ../dimlp/datafiles/dataclass2Train -W ../dimlp/datafiles/dimlpDatanorm.wts -T ../dimlp/datafiles/datanormTest -2 ../dimlp/datafiles/dataclass2Test -I 16 -H2 5 -O 2 -R ../dimlp/datafiles/dimlpDatanormRul.rls -o ../dimlp/datafiles/dimlpDatanormRulStats -r ../dimlp/datafiles/dimlpDatanormRulResult.txt");
}
*/
