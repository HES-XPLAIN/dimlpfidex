#include "dimlpPredFct.h"

////////////////////////////////////////////////////////////

void showDimlpPredParams()

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
  printOptionDescription("--hidden_layers_file <str>", "Hidden layers file name");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder");
  printOptionDescription("--test_pred_outfile <str>", "Output test prediction file name (default: dimlpTest.out)");
  printOptionDescription("--console_file <str>", "File with console logs redirection");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in staircase activation function (default: 50)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "dimlp.dimlpPred(\"--test_data_file datanormTest.txt --weights_file dimlpDatanorm.wts --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --test_pred_outfile predTest.out --root_folder dimlp/datafiles\")" << std::endl
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

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpPredParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpTest.out", true);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(WEIGHTS_FILE);
  p.assertStringExists(HIDDEN_LAYERS_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
}

int dimlpPred(const std::string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList = {"dimlpPred"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpPredParams();
      return 0;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TEST_DATA_FILE, WEIGHTS_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER,
                                              TEST_PRED_OUTFILE, CONSOLE_FILE, HIDDEN_LAYERS_FILE, NB_QUANT_LEVELS};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }
      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &e) {
        throw CommandArgumentException("Some value inside your JSON config file '" + commandList[2] + "' is out of range.\n(Probably due to a too large or too tiny numeric value).");
      } catch (const std::exception &e) {
        std::string msg(e.what()) ;
        throw CommandArgumentException("Unknown JSON config file error: " + msg);
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkDimlpPredParametersLogicValues(*params);

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
    std::string predFile = params->getString(TEST_PRED_OUTFILE);
    int quant = params->getInt(NB_QUANT_LEVELS);

    DataSet Test;
    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch;
    StringInt archInd;
    params->readHiddenLayersFile(arch, archInd);

    if (params->isStringSet(TEST_DATA_FILE)) {
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

    Dimlp net(weightFile, nbLayers, vecNbNeurons, quant);

    SaveOutputs(Test, &net, nbOut, nbWeightLayers, predFile);

    std::cout << "\n-------------------------------------------------\n"
              << std::endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

    BpNN::resetInitRandomGen();

    if (Test.GetNbEx() > 0) {
      Test.Del();
    }

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << std::endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : ./dimlpPred --test_data_file datanormTest --weights_file dimlpDatanorm.wts --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --nb_quant_levels 50 --test_pred_outfile dimlpDatanormTest.out --console_file dimlpDatanormPredResult.txt --root_folder ../dimlp/datafiles
