#include "DensClsFct.h"
using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDensClsParams()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DensCls --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--weights_generic_filename <weights generic name file> (for instance give DimlpBT, files need to have the .wts extension) ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons> ";
  cout << "--nb_dimlp_nets <number of networks>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--train_pred_outfile <output train prediction file (densClsTrain.out by default)>" << std::endl;
  cout << "--test_pred_outfile <output test prediction file (densClsTest.out by default)>" << std::endl;
  cout << "--stats_file <output file with global train and test accuracy>" << std::endl;
  cout << "--H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)" << std::endl;
  cout << "--Hk <number of neurons in the kth hidden layer>" << std::endl;
  cout << "--with_rule_extraction (RULE EXTRACTION)" << std::endl;
  cout << "--global_rules_outfile <extraction ruleFile>" << std::endl; // If we want to extract rules in a rulesFile instead of console
  cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << std::endl;

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDensClsParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultInt(NB_QUANT_LEVELS, 50);
  p.setDefaultBool(WITH_RULE_EXTRACTION, false);
  p.setDefaultString(TRAIN_PRED_OUTFILE, "densClsTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "densClsTest.out", true);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(WEIGHTS_GENERIC_FILENAME);
  p.assertIntExists(NB_DIMLP_NETS);

  // verifying logic between parameters, values range and so on...

  if (p.getInt(NB_ATTRIBUTES) < 1) {
    throw CommandArgumentException("Error : Number of attributes must be strictly positive (>=1).");
  }

  if (p.getInt(NB_CLASSES) < 2) {
    throw CommandArgumentException("Error : Number of classes must be greater than 1.");
  }

  if (p.getInt(NB_QUANT_LEVELS) <= 2) {
    throw CommandArgumentException("Error : Number of stairs in staircase activation function must be greater than 2.");
  }

  if (p.getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of dimlp nets must be strictly positive (>=1).");
  }

  // ----------------------------------------------------------------------

  // Check denormalization parameters

  // If normalizationIndices were not specified, it's all attributes
  if (!p.isStringSet(NORMALIZATION_FILE) && !p.isIntVectorSet(NORMALIZATION_INDICES) && p.isDoubleVectorSet(MUS)) {
    vector<int> normalizationIndicesTemp;
    for (int i = 0; i < p.getInt(NB_ATTRIBUTES); ++i) {
      normalizationIndicesTemp.push_back(i);
    }
    p.setIntVector(NORMALIZATION_INDICES, normalizationIndicesTemp);
  }

  // Check if mus and sigmas are both given or both not
  if ((p.isDoubleVectorSet(MUS) || p.isDoubleVectorSet(SIGMAS)) &&
      !(p.isDoubleVectorSet(MUS) && p.isDoubleVectorSet(SIGMAS))) {
    throw CommandArgumentException("Error : One of Mus(--mus) and sigmas(--sigmas) is given but not the other.");
  }

  if (p.isStringSet(NORMALIZATION_FILE) && p.isDoubleVectorSet(MUS) || p.isStringSet(NORMALIZATION_FILE) && p.isIntVectorSet(NORMALIZATION_INDICES)) {
    throw CommandArgumentException("Error : normlization file (--normalization_file) and mus or normalizationIndices (--normalization_indices) are both given.");
  }

  // Mus, sigmas and normalizationIndices must have the same size and not be empty
  if (p.isDoubleVectorSet(MUS) && (p.getDoubleVector(MUS).size() != p.getDoubleVector(SIGMAS).size() || p.getDoubleVector(MUS).size() != p.getIntVector(NORMALIZATION_INDICES).size() || p.getDoubleVector(MUS).empty())) {
    throw CommandArgumentException("Error : mus (--mus), sigmas (--sigmas) and normalization indices (--normalization_indices) don't have the same size or are empty.");
  }

  // Check normalizationIndices
  if (p.isIntVectorSet(NORMALIZATION_INDICES)) {
    vector<int> tempVect = p.getIntVector(NORMALIZATION_INDICES);
    std::set<int> uniqueIndices(tempVect.begin(), tempVect.end());
    if (uniqueIndices.size() != p.getIntVector(NORMALIZATION_INDICES).size() ||
        *std::max_element(uniqueIndices.begin(), uniqueIndices.end()) >= p.getInt(NB_ATTRIBUTES) ||
        *std::min_element(uniqueIndices.begin(), uniqueIndices.end()) < 0) {
      throw CommandArgumentException("Error : parameter normalization indices (--normalization_indices) must be a list composed of integers between [0, nb_attributes-1] without repeted elements.");
    }
  }
}

int densCls(const string &command) {

  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    string s;
    stringstream ss(command);

    while (getline(ss, s, ' ')) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2) {
      showDensClsParams();
      exit(1);
    }

    // Import parameters
    unique_ptr<Parameters> params;
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      }

      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2]));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("JSON config file name/path is invalid");
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList));
    }

    // getting all program arguments from CLI
    checkDensClsParametersLogicValues(*params);
    std::cout << *params;

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // ----------------------------------------------------------------------

    // Get parameters values
    std::string weightFileSave = "dimlp.wts";

    int nbIn = params->getInt(NB_ATTRIBUTES);
    int nbOut = params->getInt(NB_CLASSES);
    std::string learnFile = params->getString(TRAIN_DATA_FILE);
    std::string predTrainFile = params->getString(TRAIN_PRED_OUTFILE);
    std::string predTestFile = params->getString(TEST_PRED_OUTFILE);
    std::string weightFile = params->getString(WEIGHTS_GENERIC_FILENAME);
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int quant = params->getInt(NB_QUANT_LEVELS);

    int nbLayers;
    int nbWeightLayers;
    std::vector<int> vecNbNeurons;
    StringInt arch = params->getArch();
    StringInt archInd = params->getArchInd();

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

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

    if (params->isStringSet(TRAIN_CLASS_FILE) != false) {
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

    auto net = std::make_shared<BagDimlp>(quant, nbLayers, vecNbNeurons, nbDimlpNets, weightFileSave.c_str());

    net->DefNetsWithWeights(weightFile.c_str());

    float acc;
    float accTest;

    net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile.c_str());
    cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n"
         << std::endl;

    if (Test.GetNbEx() != 0) {
      net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile.c_str());
      cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
    }

    // Output accuracy stats in file
    if (params->isStringSet(STATS_FILE)) {
      ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
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

      All = Train;

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

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

// Exemple to launch the code : densCls("DensCls --train_data_file datanormTrain --train_class_file dataclass2Train --test_data_file datanormTest --test_class_file dataclass2Test --nb_attributes 16 --H2 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_generic_filename dimlpDatanormBT --with_rule_extraction true --global_rules_outfile dimlpDatanormDensClsRul.rls --train_pred_outfile dimlpDatanormDensClsTrain.out --test_pred_outfile dimlpDatanormDensClsTest.out --stats_file dimlpDatanormDensClsStats --console_file dimlpDatanormDensClsResult.txt --root_folder dimlp/datafiles");
