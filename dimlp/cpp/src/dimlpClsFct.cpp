#include "dimlpClsFct.h"

////////////////////////////////////////////////////////////

void showDimlpClsParams()

{
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are localised with respect to root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--test_data_file <str>", "Test data file");
  printOptionDescription("--weights_file <str>", "Weights file");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of input neurons");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of output neurons");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--test_class_file <str>", "Test true class file");
  printOptionDescription("--test_pred_outfile <str>", "Output test prediction file name (default: dimlpTest.out)");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--stats_file <str>", "Output file name with test accuracy");
  printOptionDescription("--hid_file <str>", "Output file name with first hidden layer values (default: dimlpTest.hid)");
  printOptionDescription("--H1 <int k*nb_attributes, k in [1,inf[>", "Number of neurons in the first hidden layer (default: nb_attributes)");
  printOptionDescription("--Hk <int [1,inf[>", "Number of neurons in the kth hidden layer");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "dimlp.dimlpCls(\"--test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_file weights.wts --nb_attributes 16 --H2 5 --nb_classes 2 --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

////////////////////////////////////////////////////////////

static void SaveOutputs(
    DataSet &data,
    Dimlp *net,
    int nbOut,
    int nbWeightLayers,
    const std::string &outfile)

{
  std::filebuf buf;

  if (buf.open(outfile, std::ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(nbWeightLayers - 1);
  const float *out = layer->GetUp();

  std::cout << "\n\n"
            << outfile << ": "
            << "Writing ..." << std::endl;

  std::ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "" << std::endl;
  }

  std::cout << outfile << ": "
            << "Written.\n"
            << std::endl;
}

////////////////////////////////////////////////////////////

void SaveFirstHid(
    DataSet &data,
    Dimlp *net,
    int nbHid,
    const std::string &outfile,
    const std::string &firsthidFile)

{
  std::filebuf buf;

  if (buf.open(firsthidFile, std::ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + outfile);
  }

  std::shared_ptr<Layer> layer = net->GetLayer(0);
  const float *hid = layer->GetUp();

  std::cout << "\n\n"
            << firsthidFile << ": "
            << "Writing ..." << std::endl;

  std::ostream outFile(&buf);

  for (int p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (int h = 0; h < nbHid; h++) {
      outFile << hid[h] << " ";
    }

    outFile << "" << std::endl;
  }

  std::cout << firsthidFile << ": "
            << "Written.\n"
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpClsParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpTest.out", true);
  p.setDefaultString(HID_FILE, "dimlpTest.hid", true);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(WEIGHTS_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
}

int dimlpCls(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"dimlpCls"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpClsParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, WEIGHTS_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, TEST_CLASS_FILE,
                                              TEST_PRED_OUTFILE, CONSOLE_FILE, STATS_FILE, HID_FILE, H, NB_QUANT_LEVELS};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }
      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("JSON config file name/path is invalid");
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkDimlpClsParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // ----------------------------------------------------------------------

    // Get parameters values

    int nbIn = params->getInt(NB_ATTRIBUTES);
    int nbOut = params->getInt(NB_CLASSES);
    std::string testFile = params->getString(TEST_DATA_FILE);
    std::string weightFile = params->getString(WEIGHTS_FILE);
    std::string hidFile = params->getString(HID_FILE);
    std::string predFile = params->getString(TEST_PRED_OUTFILE);
    int quant = params->getInt(NB_QUANT_LEVELS);

    DataSet Test;
    DataSet TestClass;
    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch = params->getArch();
    StringInt archInd = params->getArchInd();

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
        arch.GoToBeg();
        for (int p = 1; p <= arch.GetNbEl(); p++, arch.GoToNext()) {
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

        arch.GoToBeg();
        for (int p = 1; p <= arch.GetNbEl(); p++, arch.GoToNext()) {
          vecNbNeurons[p + 1] = arch.GetVal();

          if (vecNbNeurons[p + 1] == 0) {
            throw InternalError("The number of neurons must be greater than 0.");
          }
        }
      }
    }
    // ----------------------------------------------------------------------

    if (params->isStringSet(TEST_DATA_FILE)) {
      if (params->isStringSet(TEST_CLASS_FILE)) {

        DataSet test(testFile, nbIn, nbOut);
        DataSet testClass(params->getString(TEST_CLASS_FILE), nbIn, nbOut);

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

    Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

    float acc;

    float err = net.Error(Test, TestClass, &acc);

    std::cout << "\n\n*** SUM SQUARED ERROR = " << err;
    std::cout << "\n\n*** ACCURACY = " << acc << "" << std::endl;

    // Output accuracy stats in file
    if (params->isStringSet(STATS_FILE)) {
      std::ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
        accFile << "Sum squared error = " << err << "" << std::endl;
        accFile << "Accuracy = " << acc;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : could not open accuracy file " + params->getString(STATS_FILE));
      }
    }

    SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);
    SaveFirstHid(Test, &net, vecNbNeurons[1], predFile, hidFile);

    std::cout << "\n-------------------------------------------------\n"
              << std::endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();

    if (Test.GetNbEx() > 0) {
      Test.Del();
      TestClass.Del();
    }

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}

// Exemple to launch the code : dimlp.dimlpCls("dimlpCls --test_data_file datanormTest --test_class_file dataclass2Test --weights_file dimlpDatanorm.wts --nb_attributes 16 --H2 5 --nb_classes 2 --nb_quant_levels 50 --test_pred_outfile dimlpDatanormTest.out --stats_file dimlpDatanormClsStats --console_file dimlpDatanormClsResult.txt --root_folder dimlp/datafiles");
