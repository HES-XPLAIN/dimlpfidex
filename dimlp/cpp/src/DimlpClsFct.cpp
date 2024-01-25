#include "DimlpClsFct.h"
using namespace std;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpCls()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpCls --test_data_file <test set file (path with respect to specified root folder)> ";
  cout << "--weights_file <file of weights> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--test_class_file <file of classes>" << std::endl;
  cout << "--test_pred_file <output prediction file (dimlp.out by default)>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--stats_file <output file with test accuracy>" << std::endl;
  cout << "--hid_file <output file with first hidden layer values (dimlp.hid by default)>" << std::endl;
  cout << "--h1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--hk <number of neurons in the kth hidden layer>" << std::endl;
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

enum ParameterDimlpClsEnum {
  TEST_DATA_FILE,
  WEIGHTS_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  ROOT_FOLDER,
  TEST_CLASS_FILE,
  TEST_PRED_FILE,
  CONSOLE_FILE,
  STATS_FILE,
  HID_FILE,
  H,
  NB_QUANT_LEVELS,
  INVALID
};

const std::unordered_map<std::string, ParameterDimlpClsEnum> parameterMap = {
    {"test_data_file", TEST_DATA_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"root_folder", ROOT_FOLDER},
    {"test_class_file", TEST_CLASS_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"console_file", CONSOLE_FILE},
    {"stats_file", STATS_FILE},
    {"hid_file", HID_FILE},
    {"H", H},
    {"nb_quant_levels", NB_QUANT_LEVELS}};

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

    int p = 1; // We skip "DimlpCls"
    while (p < nbParam) {
      string param = commandList[p];

      if (param.substr(0, 2) == "--") {
        param = param.substr(2);
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }
        const char *arg = commandList[p].c_str();
        string stringArg = arg;

        ParameterDimlpClsEnum option;
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

        case STATS_FILE:
          accuracyFileTemp = arg;
          accuracyFileInit = true;
          break;

        case HID_FILE:
          hidFileTemp = arg;
          break;

        case TEST_DATA_FILE:
          testFileTemp = arg;
          testFileInit = true;
          break;

        case TEST_CLASS_FILE:
          testTarTemp = arg;
          testTarInit = true;
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
      throw CommandArgumentException("The number of input neurons must be given with option --nb_attributes.");
    }

    if (nbOut <= 1) {
      throw CommandArgumentException("At least two output neurons must be given with option --nb_classes.");
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
      throw CommandArgumentException("Give a file of weights with --weights_file selection please.");
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

  } catch (const errorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }

  return 0;
}

// Exemple to launch the code : dimlpCls("DimlpCls --test_data_file datanormTest --test_class_file dataclass2Test --weights_file dimlpDatanorm.wts --nb_attributes 16 --H2 5 --nb_classes 2 --nb_quant_levels 50 --test_pred_file dimlpDatanormTest.out --stats_file dimlpDatanormClsStats --console_file dimlpDatanormClsResult.txt --root_folder dimlp/datafiles");
