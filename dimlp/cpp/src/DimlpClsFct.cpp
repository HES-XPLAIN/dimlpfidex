#include "DimlpClsFct.h"
using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpCls()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpCls -T <test set file (path with respect to specified root folder)> ";
  cout << "-W <file of weights> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "-2 <file of classes>" << std::endl;
  cout << "-p <output prediction file (dimlp.out by default)>" << std::endl;
  cout << "-r <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "-o <output file with test accuracy>" << std::endl;
  cout << "-h <output file with first hidden layer values (dimlp.hid by default)>" << std::endl;
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

void SaveFirstHid(
    DataSet &data,
    Dimlp *net,
    int nbHid,
    const char *outfile,
    const char *firsthidFile)

{
  filebuf buf;

  if (buf.open(firsthidFile, ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + std::string(outfile));
  }

  std::shared_ptr<Layer> layer = net->GetLayer(0);
  const float *hid = layer->GetUp();

  cout << "\n\n"
       << firsthidFile << ": "
       << "Writing ..." << std::endl;

  ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int h = 0; h < nbHid; h++) {
      outFile << hid[h] << " ";
    }

    outFile << "" << std::endl;
  }

  cout << firsthidFile << ": "
       << "Written.\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

int dimlpCls(const string &command) {
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

    if (testFileInit == false) {
      throw CommandArgumentException("Give the training file with -L selection please.");
    }

    else // if (testFileInit != false)
    {
      if (testTarInit != false) {

        DataSet test(testFile, nbIn, nbOut);
        DataSet testClass(testTar, nbIn, nbOut);

        Test = test;
        TestClass = testClass;
      }

      else {
        DataSet data(testFile, nbIn, nbOut);

        DataSet test(data.GetNbEx());
        DataSet testClass(data.GetNbEx());

        data.ExtractDataAndTarget(test, nbIn, testClass, nbOut);

        Test = test;
        TestClass = testClass;

        data.Del();
      }
    }
    if (weightFileInit == false) {
      throw CommandArgumentException("Give a file of weights with -W selection please.");
    }

    Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

    float acc;

    float err = net.Error(Test, TestClass, &acc);

    cout << "\n\n*** SUM SQUARED ERROR = " << err;
    cout << "\n\n*** ACCURACY = " << acc << "" << std::endl;

    // Output accuracy stats in file
    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
        accFile << "Sum squared error = " << err << "" << std::endl;
        accFile << "Accuracy = " << acc;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : could not open accuracy file " + std::string(accuracyFile));
      }
    }

    SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);
    SaveFirstHid(Test, &net, vecNbNeurons[1], predFile, hidFile);

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

// Exemple to launch the code : dimlpCls("DimlpCls -T datanormTest -2 dataclass2Test -W dimlpDatanorm.wts -I 16 -H2 5 -O 2 -q 50 -p dimlpDatanormTest.out -o dimlpDatanormClsStats -r dimlpDatanormClsResult.txt -S dimlp/datafiles");
