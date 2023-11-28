#include "DimlpPredFct.h"

using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamPred()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpPred -T <test set file (path with respect to specified root folder)> ";
  cout << "-W <file of weights> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";

  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "-p <output prediction file (dimlp.out by default)>";
  cout << "-r <file where you redirect console result>"; // If we want to redirect console result to file
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "-Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "-q <number of stairs in staircase activation function (50 by default)>" << std::endl;

  cout << "\n-------------------------------------------------\n"
       << std::endl;
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
    throw CannotOpenFileError("Error : Cannot open output file " + std::string(outfile));
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  cout << "\n\n"
       << outfile << ": "
       << "Writing ..." << std::endl;

  ostream outFile(&buf);
  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "" << std::endl;
  }

  cout << outfile << ": "
       << "Written.\n"
       << std::endl;
}

////////////////////////////////////////////////////////////
int dimlpPred(const string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

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
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        char option = commandList[k - 1][1];
        const char *arg = &(commandList[k])[0];
        const char *lastArg = &(commandList[k - 1])[0];
        switch (option) {
        case 'q':
          if (CheckInt(arg))
            quant = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", integer requested");

          break;

        case 'I':
          if (CheckInt(arg))
            nbIn = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", integer requested");

          break;

        case 'H':
          if (CheckInt(arg)) {
            arch.Insert(atoi(arg));

            const char *ptrParam = lastArg;

            if (ptrParam[2] != '\0') {
              std::string str(ptrParam + 2);
              archInd.Insert(std::atoi(str.c_str()));
            } else {
              throw CommandArgumentException("Error : Which hidden layer (-H) ?");
            }
          } else
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", integer requested");

          break;

        case 'O':
          if (CheckInt(arg))
            nbOut = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", integer requested");

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
          throw CommandArgumentException("Illegal option : " + string(lastArg));
        }
      }

      else {
        throw CommandArgumentException("Illegal option : " + string(&(commandList[k])[0]));
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
    if (consoleFileInit != false) {
      ofs.open(consoleFile);
      std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
    }

    // ----------------------------------------------------------------------

    if (quant <= 2) {
      throw CommandArgumentException("The number of quantized levels must be greater than 2.");
    }

    if (nbIn == 0) {
      throw CommandArgumentException("The number of input neurons must be given with option -I.");
    }

    if (nbOut <= 1) {
      throw CommandArgumentException("At least two output neurons must be given with option -O.");
    }

    if (weightFileInit == false) {
      throw CommandArgumentException("Give a file of weights with -W selection please.");
    }

    if (testFileInit == false) {
      throw CommandArgumentException("Give a testing file with -T selection please.");
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
          throw InternalError("The number of neurons in the first hidden layer must be a multiple of the number of input neurons.");
        }

        nbLayers = arch.GetNbEl() + 2;
        nbWeightLayers = nbLayers - 1;

        vecNbNeurons.assign(nbLayers, 0);
        vecNbNeurons[0] = nbIn;
        vecNbNeurons[nbLayers - 1] = nbOut;

        for (k = 1, arch.GoToBeg(); k <= arch.GetNbEl(); k++, arch.GoToNext()) {
          vecNbNeurons[k] = arch.GetVal();

          if (vecNbNeurons[k] == 0) {
            throw InternalError("The number of neurons must be greater than 0.");
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
            throw InternalError("The number of neurons must be greater than 0.");
          }
        }
      }
    }

    // ----------------------------------------------------------------------

    Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

    SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);

    cout << "\n-------------------------------------------------\n"
         << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();
  } catch (const char *msg) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    cerr << msg << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : ./DimlpPred -T datanormTest -W dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p dimlpDatanormTest.out -r dimlpDatanormPredResult.txt -S ../dimlp/datafiles
