#include "DimlpPredFct.h"

using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamPred()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DimlpPred -T <file of examples(path with respect to specified root folder)> ";
  cout << "-W <file of weights> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";

  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-p <output prediction file>";                 // If we want to specify output prediction file, not to be dimlp.out
  cout << "-r <file where you redirect console result>"; // If we want to redirect console result to file
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)\n";
  cout << "-Hk <number of neurons in the kth hidden layer>\n";
  cout << "-q <number of stairs in staircase activation function>\n";

  cout << "\n-------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////

static void SaveOutputs(
    DataSet &data,
    Dimlp *net,
    int nbOut,
    int nbWeightLayers,
    const char *outfile)

{
  filebuf buf;

  if (buf.open(outfile, ios_base::out) == nullptr) {
    string errorMsg = "Cannot open file for writing";
    WriteError(errorMsg, outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  cout << "\n\n"
       << outfile << ": "
       << "Writing ...\n";

  ostream outFile(&buf);
  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "\n";
  }

  cout << outfile << ": "
       << "Written.\n\n";
}

////////////////////////////////////////////////////////////
int dimlpPred(const string &command) {

  // Parsing the command
  vector<string> commandList;
  const char delim = ' ';
  string s;
  stringstream ss(command);
  while (std::getline(ss, s, delim)) {
    commandList.push_back(s);
  }
  size_t nbParam = commandList.size();

  DataSet Test;

  int quant = 50;
  int nbIn = 0;
  int nbOut = 0;

  string testFileTemp;
  bool testFileInit = false;
  string weightFileTemp;
  bool weightFileInit = false;
  string predFileTemp = "dimlp.out";
  string consoleFileTemp;
  bool consoleFileInit = false;
  string rootFolderTemp;
  bool rootFolderInit = false;

  int nbLayers;
  int nbWeightLayers;
  std::vector<int> vecNbNeurons;

  StringInt arch;
  StringInt archInd;

  // Import parameters

  if (nbParam <= 1) {
    GiveAllParamPred();
    return 0;
  }

  int k = 1; // We skip "DimlpPred"
  while (k < nbParam) {
    if (commandList[k][0] == '-') {
      k++;

      if (k >= nbParam) {
        cout << "Missing something at the end of the command.\n";
        return -1;
      }

      char option = commandList[k - 1][1];
      const char *arg = &(commandList[k])[0];
      const char *lastArg = &(commandList[k - 1])[0];
      switch (option) {
      case 'q':
        if (CheckInt(arg))
          quant = atoi(arg);
        else
          return -1;

        break;

      case 'I':
        if (CheckInt(arg))
          nbIn = atoi(arg);
        else
          return -1;

        break;

      case 'H':
        if (CheckInt(arg)) {
          arch.Insert(atoi(arg));

          const char *ptrParam = lastArg;

          if (ptrParam[2] != '\0') {
            std::string str(ptrParam + 2);
            archInd.Insert(std::atoi(str.c_str()));
          } else {
            cout << "Which hidden layer (-H) ?\n";
            return -1;
          }
        } else
          return -1;

        break;

      case 'O':
        if (CheckInt(arg))
          nbOut = atoi(arg);
        else
          return -1;

        break;

      case 'S':
        rootFolderTemp = arg;
        rootFolderInit = true;
        break;

      case 'W':
        weightFileTemp = arg;
        weightFileInit = true;
        break;

      case 'p':
        predFileTemp = arg;
        break;

      case 'r':
        consoleFileTemp = arg;
        consoleFileInit = true;
        break;

      case 'T':
        testFileTemp = arg;
        testFileInit = true;
        break;

      default:
        cout << "Illegal option: " << lastArg << "\n";
        return -1;
      }
    }

    else {
      cout << "Illegal option: " << &(commandList[k])[0] << "\n";
      return -1;
    }

    k++;
  }

  // ----------------------------------------------------------------------

  // create paths with root foler

  const char *testFile = nullptr;
  const char *weightFile = nullptr;
  const char *predFile = nullptr;
  const char *consoleFile = nullptr;

  string root = "";
  if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
    root = rootFolderTemp + "/";
#elif defined(_WIN32)
    root = rootFolderTemp + "\\";
#endif
  }

  predFileTemp = root + predFileTemp;
  predFile = &predFileTemp[0];

  if (consoleFileInit) {
    consoleFileTemp = root + consoleFileTemp;
    consoleFile = &consoleFileTemp[0];
  }

  if (testFileInit) {
    testFileTemp = root + testFileTemp;
    testFile = &testFileTemp[0];
  }

  if (weightFileInit) {
    weightFileTemp = root + weightFileTemp;
    weightFile = &weightFileTemp[0];
  }

  // ----------------------------------------------------------------------

  // Get console results to file
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  if (consoleFileInit != false) {
    ofs.open(consoleFile);
    std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
  }

  // ----------------------------------------------------------------------

  if (quant <= 2) {
    cout << "The number of quantized levels must be greater than 2.\n";
    return -1;
  }

  if (nbIn == 0) {
    cout << "The number of input neurons must be given with option -I.\n";
    return -1;
  }

  if (nbOut <= 1) {
    cout << "At least two output neurons must be given with option -O.\n";
    return -1;
  }

  if (weightFileInit == false) {
    cout << "Give a file of weights with -W selection please."
         << "\n";
    return -1;
  }

  if (testFileInit == false) {
    cout << "Give a testing file with -T selection please."
         << "\n";
    return -1;
  }

  else // if (testFileInit != false)
  {
    DataSet test(testFile, nbIn);
    Test = test;
  }

  // ----------------------------------------------------------------------

  if (arch.GetNbEl() == 0) {
    nbLayers = 3;
    nbWeightLayers = nbLayers - 1;

    vecNbNeurons.assign(nbLayers, 0);
    vecNbNeurons[0] = nbIn;
    vecNbNeurons[1] = nbIn;
    vecNbNeurons[2] = nbOut;
  }

  else {
    archInd.GoToBeg();

    if (archInd.GetVal() == 1) {
      arch.GoToBeg();

      if (arch.GetVal() % nbIn != 0) {
        cout << "The number of neurons in the first hidden layer must be";
        cout << " a multiple of the number of input neurons.\n";
        return -1;
      }

      nbLayers = arch.GetNbEl() + 2;
      nbWeightLayers = nbLayers - 1;

      vecNbNeurons.assign(nbLayers, 0);
      vecNbNeurons[0] = nbIn;
      vecNbNeurons[nbLayers - 1] = nbOut;

      for (k = 1, arch.GoToBeg(); k <= arch.GetNbEl(); k++, arch.GoToNext()) {
        vecNbNeurons[k] = arch.GetVal();

        if (vecNbNeurons[k] == 0) {
          cout << "The number of neurons must be greater than 0.\n";
          return -1;
        }
      }
    }

    else {
      nbLayers = arch.GetNbEl() + 3;
      nbWeightLayers = nbLayers - 1;

      vecNbNeurons.assign(nbLayers, 0);
      vecNbNeurons[0] = nbIn;
      vecNbNeurons[1] = nbIn;
      vecNbNeurons[nbLayers - 1] = nbOut;

      for (k = 1, arch.GoToBeg(); k <= arch.GetNbEl(); k++, arch.GoToNext()) {
        vecNbNeurons[k + 1] = arch.GetVal();

        if (vecNbNeurons[k + 1] == 0) {
          cout << "The number of neurons must be greater than 0.\n";
          return -1;
        }
      }
    }
  }

  // ----------------------------------------------------------------------

  Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

  SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);

  cout << "\n-------------------------------------------------\n\n";

  std::cout.rdbuf(cout_buff); // reset to standard output again

  BpNN::resetInitRandomGen();

  return 0;
}

// Exemple to launch the code : ./DimlpPred -T datanormTest -W dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p dimlpDatanormTest.out -r dimlpDatanormPredResult.txt -S dimlp/datafiles
