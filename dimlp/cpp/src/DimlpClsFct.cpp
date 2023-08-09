#include "DimlpClsFct.h"
using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpCls()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DimlpCls -T <file of examples(path with respect to specified root folder)> ";
  cout << "-W <file of weights> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";
  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-2 <file of classes>\n";
  cout << "-p <output prediction file>\n";                 // If we want to specify output prediction file, not to be dimlp.out
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

void SaveFirstHid(
    DataSet &data,
    Dimlp *net,
    int nbHid,
    const char *outfile,
    const char *firsthidFile)

{
  filebuf buf;

  if (buf.open(firsthidFile, ios_base::out) == nullptr) {
    string errorMsg = "Cannot open file for writing";
    WriteError(errorMsg, outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(0);
  const float *hid = layer->GetUp();

  cout << "\n\n"
       << firsthidFile << ": "
       << "Writing ...\n";

  ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int h = 0; h < nbHid; h++) {
      outFile << hid[h] << " ";
    }

    outFile << "\n";
  }

  cout << firsthidFile << ": "
       << "Written.\n\n";
}

////////////////////////////////////////////////////////////

int dimlpCls(const string &command) {

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
  DataSet TestClass;

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
  string accuracyFileTemp;
  bool accuracyFileInit = false;
  string testTarTemp;
  bool testTarInit = false;
  string hidFileTemp = "dimlp.hid";
  string rootFolderTemp;
  bool rootFolderInit = false;

  int nbLayers;
  int nbWeightLayers;
  std::vector<int> vecNbNeurons;

  StringInt arch;
  StringInt archInd;

  if (nbParam <= 1) {
    GiveAllParamDimlpCls();
    return 0;
  }

  int k = 1; // We skip "DimlpCls"
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

      case 'o':
        accuracyFileTemp = arg;
        accuracyFileInit = true;
        break;

      case 'h':
        hidFileTemp = arg;
        break;

      case 'T':
        testFileTemp = arg;
        testFileInit = true;
        break;

      case '2':
        testTarTemp = arg;
        testTarInit = true;
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
  const char *accuracyFile = nullptr;
  const char *testTar = nullptr;
  const char *hidFile = nullptr;

  string root = "";
  if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
    root = rootFolderTemp + "/";
#elif defined(_WIN32)
    root = rootFolderTemp + "\\";
#endif
  }

  if (testFileInit) {
    testFileTemp = root + testFileTemp;
    testFile = &testFileTemp[0];
  }

  if (weightFileInit) {
    weightFileTemp = root + weightFileTemp;
    weightFile = &weightFileTemp[0];
  }

  predFileTemp = root + predFileTemp;
  predFile = &predFileTemp[0];

  if (consoleFileInit) {
    consoleFileTemp = root + consoleFileTemp;
    consoleFile = &consoleFileTemp[0];
  }

  if (accuracyFileInit) {
    accuracyFileTemp = root + accuracyFileTemp;
    accuracyFile = &accuracyFileTemp[0];
  }

  if (testTarInit) {
    testTarTemp = root + testTarTemp;
    testTar = &testTarTemp[0];
  }

  hidFileTemp = root + hidFileTemp;
  hidFile = &hidFileTemp[0];

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

  // ----------------------------------------------------------------------
  if (arch.GetNbEl() == 0) {
    nbLayers = 3;
    nbWeightLayers = nbLayers - 1;

    vecNbNeurons.assign(nbLayers, 0);
    vecNbNeurons[0] = nbIn;
    vecNbNeurons[1] = nbIn;
    vecNbNeurons[2] = nbOut;
  } else {
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

  if (testFileInit == false) {
    cout << "Give a testing file with -T selection please."
         << "\n";
    return -1;
  }

  else // if (testFileInit != false)
  {
    if (testTarInit != false) {

      DataSet test(testFile, nbIn);
      DataSet testClass(testTar, nbOut);

      Test = test;
      TestClass = testClass;
    }

    else {
      DataSet data(testFile, nbIn + nbOut);

      DataSet test(data.GetNbEx());
      DataSet testClass(data.GetNbEx());

      data.ExtractDataAndTarget(test, nbIn, testClass, nbOut);

      Test = test;
      TestClass = testClass;

      data.Del();
    }
  }
  if (weightFileInit == false) {
    cout << "Give a file of weights with -W selection please."
         << "\n";
    return -1;
  }

  Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

  float acc;

  float err = net.Error(Test, TestClass, &acc);

  cout << "\n\n*** SUM SQUARED ERROR = " << err;
  cout << "\n\n*** ACCURACY = " << acc << "\n";

  // Output accuracy stats in file
  if (accuracyFileInit != false) {
    ofstream accFile(accuracyFile);
    if (accFile.is_open()) {
      accFile << "Sum squared error = " << err << "\n";
      accFile << "Accuracy = " << acc;
      accFile.close();
    } else {
      cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
      return -1;
    }
  }

  SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);
  SaveFirstHid(Test, &net, vecNbNeurons[1], predFile, hidFile);

  cout << "\n-------------------------------------------------\n\n";

  std::cout.rdbuf(cout_buff); // reset to standard output again

  BpNN::resetInitRandomGen();

  return 0;
}

// Exemple to launch the code : dimlpCls("DimlpCls -T datanormTest -2 dataclass2Test -W dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p dimlpDatanormTest.out -o dimlpDatanormClsStats -r dimlpDatanormClsResult.txt -S dimlp/datafiles");
