using namespace std;
#include "DimlpPredFct.h"

////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DimlpPred -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
   cout << "-T <file of examples(path with respect to specified root folder)> ";
   cout << "-W <file of weights> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";

   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-p <output prediction file>"; // If we want to specify output prediction file, not to be dimlp.out
   cout << "-r <file where you redirect console result>"; // If we want to redirect console result to file
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
int dimlpPred(string command){

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

    int quant = 50;
    int nbIn  = 0;
    int nbOut = 0;

    string testFileTemp;
    bool testFileInit = false;
    string weightFileTemp;
    bool weightFileInit = false;
    string predFileTemp = "dimlp.out";
    bool predFileInit = false;
    string consoleFileTemp;
    bool consoleFileInit = false;
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
                            predFileInit = true;
                            break;

                case 'r' :
                            consoleFileTemp = &(commandList[k])[0];
                            consoleFileInit = true;
                            break;

                case 'T' :
                            testFileTemp   = &(commandList[k])[0];
                            testFileInit   = true;
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


    char testFileTmp[160], weightFileTmp[160], predFileTmp[160], consoleFileTmp[160];

    char* testFile = 0;
    char* weightFile = 0;
    char* predFile = 0;
    char* consoleFile = 0;

    #ifdef __unix__
    string root = rootFolderTemp + "/";
    #elif defined(_WIN32)
    string root = rootFolderTemp + "\\";
    #endif

    predFileTemp = root + predFileTemp;
    if(predFileTemp.length()>=160){
        cout << "Path " << predFileTemp << "is too long" << "\n";
        return -1;
    }
    strcpy(predFileTmp, predFileTemp.c_str());
    predFile = predFileTmp;

    if (consoleFileInit){
        consoleFileTemp = root + consoleFileTemp;
        if(consoleFileTemp.length()>=160){
            cout << "Path " << consoleFileTemp << "is too long" << "\n";
            return -1;
        }
        strcpy(consoleFileTmp, consoleFileTemp.c_str());
        consoleFile = consoleFileTmp;
    }

    if(testFileInit){
        testFileTemp = root + testFileTemp;
        if(testFileTemp.length()>=160){
            cout << "Path " << testFileTemp << "is too long" << "\n";
            return -1;
        }
        strcpy(testFileTmp, testFileTemp.c_str());
        testFile = testFileTmp;
    }

    if(weightFileInit){
        weightFileTemp = root + weightFileTemp;
        if(weightFileTemp.length()>=160){
            cout << "Path " << weightFileTemp << "is too long" << "\n";
            return -1;
        }
        strcpy(weightFileTmp, weightFileTemp.c_str());
        weightFile = weightFileTmp;
    }


    // ----------------------------------------------------------------------

    // Get console results to file
    std::ofstream ofs;
    std::streambuf *cout_buff = std::cout.rdbuf();  // Save old buf
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

    if (weightFileInit == false)
    {
        cout << "Give a file of weights with -W selection please." << "\n";
        return -1;
    }

    if (testFileInit == false)
    {
        cout << "Give a testing file with -T selection please." << "\n";
        return -1;
    }

    else // if (testFileInit != false)
    {
        static DataSet test(testFile, nbIn);
        Test = test;
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



    Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

    SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);

    Test.Del();

    cout << "\n-------------------------------------------------\n\n";

    std::cout.rdbuf(cout_buff); // reset to standard output again

    return 0;
}
/*
int main(int nbParam, char** param)
{
    dimlpPred("DimlpPred -T ../dimlp/datafiles/datanormTest -W ../dimlp/datafiles/dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p ../dimlp/datafiles/dimlpDatanormTest.out -r ../dimlp/datafiles/dimlpDatanormPredResult.txt");
}
*/
