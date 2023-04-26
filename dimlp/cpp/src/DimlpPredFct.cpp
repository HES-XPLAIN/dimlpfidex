using namespace std;
#include "DimlpPredFct.h"

////////////////////////////////////////////////////////////

void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "DimlpPred -T <file of examples> ";
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

    char* testFile   = 0;
    char* weightFile = 0;
    char* predFile = (char*) "dimlp.out";
    char* consoleFile = 0;

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

                case 'W' : weightFile = &(commandList[k])[0];
                            break;

                case 'p' : predFile = &(commandList[k])[0];
                            break;

                case 'r' : consoleFile = &(commandList[k])[0];
                            break;

                case 'T' : testFile   = &(commandList[k])[0];
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
    std::streambuf *cout_buff = std::cout.rdbuf();  // Save old buf
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
        static DataSet test(testFile, nbIn);
        Test = test;
    }

    if (weightFile == 0)
    {
        cout << "Give a file of weights with -W selection please." << "\n";
        return -1;
    }

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
