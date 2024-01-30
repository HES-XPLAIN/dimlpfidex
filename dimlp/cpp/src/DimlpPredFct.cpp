#include "DimlpPredFct.h"

using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamPred()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpPred --test_data_file <test set file (path with respect to specified root folder)> ";
  cout << "--weights_file <file of weights> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";

  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--test_pred_file <output prediction file (dimlp.out by default)>";
  cout << "--console_file <file where you redirect console result>"; // If we want to redirect console result to file
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;

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

enum ParameterDimlpPredEnum {
  TEST_DATA_FILE,
  WEIGHTS_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  ROOT_FOLDER,
  TEST_PRED_FILE,
  CONSOLE_FILE,
  H,
  NB_QUANT_LEVELS,
  INVALID
};

const std::unordered_map<std::string, ParameterDimlpPredEnum> parameterMap = {
    {"test_data_file", TEST_DATA_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"root_folder", ROOT_FOLDER},
    {"test_pred_file", TEST_PRED_FILE},
    {"console_file", CONSOLE_FILE},
    {"H", H},
    {"nb_quant_levels", NB_QUANT_LEVELS}};

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

    int p = 1; // We skip "DimlpPred"
    while (p < nbParam) {
      string param = commandList[p];

      if (param.substr(0, 2) == "--") {
        param = param.substr(2);
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        if (p + 1 < nbParam && commandList[p + 1].substr(0, 2) != "--") {
          throw CommandArgumentException("There is a parameter without -- (" + commandList[p + 1] + ").");
        }

        const char *arg = commandList[p].c_str();
        string stringArg = arg;

        ParameterDimlpPredEnum option;
        auto it = parameterMap.find(param);
        if (it != parameterMap.end()) {
          option = it->second;
        } else {
          if (param[0] == 'H') {
            std::string numberPart = param.substr(1);
            if (CheckInt(numberPart.c_str())) {
              option = H;
            } else {
              option = INVALID;
            }
          } else {
            option = INVALID;
          }
        }

        switch (option) { // After --
        case NB_QUANT_LEVELS:
          if (CheckInt(arg))
            quant = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_ATTRIBUTES:
          if (CheckInt(arg))
            nbIn = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case H:
          if (CheckInt(arg)) {
            arch.Insert(atoi(arg));

            const char *ptrParam = param.c_str();

            if (ptrParam[1] != '\0') {
              std::string str(ptrParam + 1);
              archInd.Insert(std::atoi(str.c_str()));
            } else {
              throw CommandArgumentException("Error : Which hidden layer (-H) ?");
            }
          } else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case NB_CLASSES:
          if (CheckInt(arg))
            nbOut = atoi(arg);
          else
            throw CommandArgumentException("Error : invalide type for parameter " + param + ", integer requested");

          break;

        case ROOT_FOLDER:
          rootFolderTemp = arg;
          rootFolderInit = true;
          break;

        case WEIGHTS_FILE:
          weightFileTemp = arg;
          weightFileInit = true;
          break;

        case TEST_PRED_FILE:
          predFileTemp = arg;
          break;

        case CONSOLE_FILE:
          consoleFileTemp = arg;
          consoleFileInit = true;
          break;

        case TEST_DATA_FILE:
          testFileTemp = arg;
          testFileInit = true;
          break;

        default:
          throw CommandArgumentException("Illegal option : " + param + ".");
        }
      }

      else {
        throw CommandArgumentException("Illegal option : " + string(&(commandList[p])[0]));
      }

      p++;
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
      throw CommandArgumentException("The number of input neurons must be given with option --nb_attributes.");
    }

    if (nbOut <= 1) {
      throw CommandArgumentException("At least two output neurons must be given with option --nb_classes.");
    }

    if (weightFileInit == false) {
      throw CommandArgumentException("Give a file of weights with --weights_file selection please.");
    }

    if (testFileInit == false) {
      throw CommandArgumentException("Give a testing file with --test_data_file selection please.");
    }

    else // if (testFileInit != false)
    {
      DataSet test(testFile, nbIn, nbOut);
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

        for (p = 1, arch.GoToBeg(); p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p] = arch.GetVal();

          if (vecNbNeurons[p] == 0) {
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

        for (p = 1, arch.GoToBeg(); p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p + 1] = arch.GetVal();

          if (vecNbNeurons[p + 1] == 0) {
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

  } catch (const errorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : ./DimlpPred --test_data_file datanormTest --weights_file dimlpDatanorm.wts --nb_attributes 16 --H2 5 --nb_classes 2 --nb_quant_levels 50 --test_pred_file dimlpDatanormTest.out --console_file dimlpDatanormPredResult.txt --root_folder ../dimlp/datafiles
