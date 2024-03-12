#include "dimlpBTFct.h"
const int BPNN = 1;

using namespace std;
////////////////////////////////////////////////////////////

void showDimlpBTParams()

{
  std::cout << "---------------------------------------------------------------------" << std::endl;
  std::cout << "Warning! The files are localised with respect to root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl;
  std::cout << "----------------------------" << std::endl;
  std::cout << "Required parameters:" << std::endl;

  cout << "dimlpBT --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";

  cout << " <Options>\n"
       << std::endl;

  std::cout << "----------------------------" << std::endl;
  std::cout << "Optional parameters: \n"
            << std::endl;
  std::cout << "--json_config_file <JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path>" << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--nb_dimlp_nets <number of networks (25 by default)>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--console_file <file with console logs redirection>" << std::endl; // If we want to redirect console result to file
  cout << "--weights_generic_outfilename <output weights generic name file(without .wts, dimlpBT by default)>" << std::endl;
  cout << "--train_pred_outfile <output train prediction file (dimlpBTTrain.out by default)>" << std::endl;
  cout << "--test_pred_outfile <output test prediction file (dimlpBTTest.out by default)>" << std::endl;
  cout << "--stats_file <output file with train, test and validation accuracy and with the global accuracy for train and test>" << std::endl;
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--with_rule_extraction (RULE EXTRACTION)" << std::endl;
  cout << "--global_rules_outfile <extraction ruleFile>" << std::endl; // If we want to extract rules in a rulesFile instead of console
  cout << "--learning_rate <back-propagation learning parameter (Eta, 0.1 by default)>" << std::endl;
  cout << "--momentum <back-propagation momentum parameter (Mu, 0.6 by default)>" << std::endl;
  cout << "--flat <back-propagation flat spot elimination parameter (Flat, 0.01 by default)>" << std::endl;
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  cout << "--error_thresh <error threshold (None by default)>" << std::endl;
  cout << "--acc_thresh <accuracy threshold (None by default)>" << std::endl;
  cout << "--abs_error_thresh <absolute difference error threshold>" << std::endl;
  cout << "--nb_epochs <number of epochs (0 by default)>" << std::endl;
  cout << "--nb_epochs_error <number of epochs to show error (1500 by default)>" << std::endl;
  cout << "--nb_ex_per_net <number of examples for one single network (all examples by default (value 0, it is recommended not to change this value))>" << std::endl;
  cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << std::endl;
  cout << "--seed <seed (0=random, default)>";

  std::cout << "\n---------------------------------------------------------------------\n"
            << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpBTParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultString(TRAIN_PRED_OUTFILE, "dimlpBTTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpBTTest.out", true);
  p.setDefaultString(WEIGHTS_GENERIC_OUTFILENAME, "dimlpBT", true);
  p.setDefaultNbQuantLevels();
  p.setDefaultDimlpTrn();
  p.setDefaultInt(NB_DIMLP_NETS, 25);
  p.setDefaultInt(NB_EX_PER_NET, 0);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersDimlpTrn();
  p.checkParametersNormalization();

  if (p.getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of dimlp nets must be strictly positive (>=1).");
  }

  if (p.getInt(NB_EX_PER_NET) < 0) {
    throw CommandArgumentException("Error : Number of examples per net must be positive (>=0, 0=all examples).");
  }
}

int dimlpBT(const string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {
    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList = {"dimlpBT"};
    string s;
    stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showDimlpBTParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, NB_DIMLP_NETS, ATTRIBUTES_FILE,
                                              TEST_DATA_FILE, TRAIN_CLASS_FILE, TEST_CLASS_FILE, CONSOLE_FILE, WEIGHTS_GENERIC_OUTFILENAME,
                                              TRAIN_PRED_OUTFILE, TEST_PRED_OUTFILE, STATS_FILE, H, WITH_RULE_EXTRACTION, GLOBAL_RULES_OUTFILE,
                                              LEARNING_RATE, MOMENTUM, FLAT, NB_QUANT_LEVELS, ERROR_THRESH, ACC_THRESH, ABS_ERROR_THRESH,
                                              NB_EPOCHS, NB_EPOCHS_ERROR, NB_EX_PER_NET, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES, SEED};
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
    checkDimlpBTParametersLogicValues(*params);

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
    std::string learnFile = params->getString(TRAIN_DATA_FILE);
    std::string predTrainFile = params->getString(TRAIN_PRED_OUTFILE);
    std::string predTestFile = params->getString(TEST_PRED_OUTFILE);
    std::string genericWeightsFile = params->getString(WEIGHTS_GENERIC_OUTFILENAME);

    float eta = params->getFloat(LEARNING_RATE);
    float mu = params->getFloat(MOMENTUM);
    float flat = params->getFloat(FLAT);
    float errThres = params->getFloat(ERROR_THRESH);
    float accThres = params->getFloat(ACC_THRESH);
    float deltaErr = params->getFloat(ABS_ERROR_THRESH);
    int showErr = params->getInt(NB_EPOCHS_ERROR);
    int epochs = params->getInt(NB_EPOCHS);
    int quant = params->getInt(NB_QUANT_LEVELS);
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbExInOne = params->getInt(NB_EX_PER_NET);
    int seed = params->getInt(SEED);

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch = params->getArch();
    StringInt archInd = params->getArchInd();

    string weightFile = "dimlp.wts";
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

    if (params->isStringSet(TRAIN_CLASS_FILE)) {
      DataSet train(learnFile, nbIn, nbOut);
      DataSet trainClass(params->getString(TRAIN_CLASS_FILE), nbIn, nbOut);

      Train = train;
      TrainClass = trainClass;
    } else {
      DataSet data(learnFile, nbIn, nbOut);

      DataSet train(data.GetNbEx());
      DataSet trainClass(data.GetNbEx());

      data.ExtractDataAndTarget(train, nbIn, trainClass, nbOut);

      Train = train;
      TrainClass = trainClass;

      data.Del();
    }

    if (params->isStringSet(TEST_DATA_FILE)) {
      if (params->isStringSet(TEST_CLASS_FILE)) {
        DataSet test(params->getString(TEST_DATA_FILE), nbIn, nbOut);
        DataSet testClass(params->getString(TEST_CLASS_FILE), nbIn, nbOut);

        Test = test;
        TestClass = testClass;
      }

      else {
        DataSet data(params->getString(TEST_DATA_FILE), nbIn, nbOut);

        DataSet test(data.GetNbEx());
        DataSet testClass(data.GetNbEx());

        data.ExtractDataAndTarget(test, nbIn, testClass, nbOut);

        Test = test;
        TestClass = testClass;

        data.Del();
      }
    }
    auto net = std::make_shared<BagDimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                          quant, showErr, epochs, nbLayers, vecNbNeurons,
                                          nbDimlpNets, weightFile, seed);

    if (nbExInOne == 0)
      nbExInOne = Train.GetNbEx();
    else if (nbExInOne > Train.GetNbEx())
      nbExInOne = Train.GetNbEx();

    net->MakeDataSets(Train, TrainClass, nbExInOne);
    if (params->isStringSet(STATS_FILE)) {
      ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
        accFile << "Accuracy for Bag training : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + params->getString(STATS_FILE));
      }
    }
    net->TrainAll(Test, TestClass, genericWeightsFile, params->getString(STATS_FILE), seed);
    float acc;
    float accTest;

    net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile);
    cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n"
         << std::endl;

    if (Test.GetNbEx() != 0) {
      net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile);
      cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (params->isStringSet(STATS_FILE)) {
      ofstream accFile(params->getString(STATS_FILE), ios::app);
      if (accFile.is_open()) {
        accFile << "-------------------------------------------------------" << std::endl;
        accFile << "-------------------------------------------------------\n"
                << std::endl;
        accFile << "Global accuracy on training set = " << acc << "" << std::endl;
        if (Test.GetNbEx() != 0) {
          accFile << "Global accuracy on testing set = " << accTest;
        }
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : could not open accuracy file " + params->getString(STATS_FILE));
      }
    }

    if (params->getBool(WITH_RULE_EXTRACTION)) {
      vector<string> attributeNames;
      if (params->isStringSet(ATTRIBUTES_FILE)) {
        AttrName attr(params->getString(ATTRIBUTES_FILE), nbIn, nbOut);

        if (attr.ReadAttr())
          cout << "\n\n"
               << params->getString(ATTRIBUTES_FILE) << ": Read file of attributes.\n"
               << std::endl;

        Attr = attr;
        attributeNames = Attr.GetListAttr();
      }

      All = Train;
      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        cout << "Extraction Part :: " << std::endl;
      }

      vector<int> normalizationIndices;
      vector<double> mus;
      vector<double> sigmas;

      // Get mus, sigmas and normalizationIndices from normalizationFile for denormalization :
      if (params->isStringSet(NORMALIZATION_FILE)) {
        auto results = parseNormalizationStats(params->getString(NORMALIZATION_FILE), params->getInt(NB_ATTRIBUTES), attributeNames);
        normalizationIndices = std::get<0>(results);
        mus = std::get<2>(results);
        sigmas = std::get<3>(results);
        params->setIntVector(NORMALIZATION_INDICES, normalizationIndices);
        params->setDoubleVector(MUS, mus);
        params->setDoubleVector(SIGMAS, sigmas);
      }

      cout << "\n\n****************************************************\n"
           << std::endl;
      cout << "*** RULE EXTRACTION" << std::endl;

      std::shared_ptr<VirtualHyp> globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                                 vecNbNeurons[1] / nbIn);

      RealHyp ryp(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                  All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                  nbWeightLayers);

      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        filebuf buf;

        if (buf.open(params->getString(GLOBAL_RULES_OUTFILE), ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + params->getString(GLOBAL_RULES_OUTFILE));
        }

        ostream rulesFileost(&buf);

        if (params->isDoubleVectorSet(MUS)) {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
        } else {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, rulesFileost);
        }

        if (ryp.TreeAborted()) {

          std::shared_ptr<VirtualHyp> globVirt2 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt2, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost);
          }
        }

        cout << "\n\n"
             << params->getString(GLOBAL_RULES_OUTFILE) << ": "
             << "Written.\n"
             << std::endl;
      } else {
        if (params->isDoubleVectorSet(MUS)) {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, cout, mus, sigmas, normalizationIndices);
        } else {
          ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                             Test, TestClass, Attr, cout);
        }

        if (ryp.TreeAborted()) {

          std::shared_ptr<VirtualHyp> globVirt3 = net->MakeGlobalVirt(quant, nbIn,
                                                                      vecNbNeurons[1] / nbIn);

          RealHyp2 ryp2(globVirt3, nbDimlpNets, net->GetGlobalOut(), nbOut,
                        All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                        nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, cout, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, cout);
          }
        }
      }
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again
    BpNN::resetInitRandomGen();

    Train.Del();
    TrainClass.Del();

    if (Test.GetNbEx() > 0) {
      Test.Del();
      TestClass.Del();
    }

    if (Valid.GetNbEx() > 0) {
      Valid.Del();
      ValidClass.Del();
    }

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : dimlp.dimlpBT("dimlpBT --train_data_file datanormTrain --train_class_file dataclass2Train --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --H2 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_generic_outfilename dimlpDatanormBT --with_rule_extraction true --global_rules_outfile dimlpDatanormBTRul.rls --train_pred_outfile dimlpDatanormBTTrain.out --test_pred_outfile dimlpDatanormBTTest.out --stats_file dimlpDatanormBTStats --console_file dimlpDatanormBTResult.txt --root_folder dimlp/datafiles");
