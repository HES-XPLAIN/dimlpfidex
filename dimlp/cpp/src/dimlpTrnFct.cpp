#include "dimlpTrnFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDimlpTrnParams()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "dimlpTrn --train_data_file <training set file(path with respect to specified root folder)> ";
  cout << "--nb_attributes <number of input neurons> --nb_classes <number of output neurons>";
  cout << " <Options>\n"
       << std::endl;

  cout << "Options are: \n"
       << std::endl;
  cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  cout << "--attributes_file <file of attributes>" << std::endl;
  cout << "--valid_data_file <validation set file>" << std::endl;
  cout << "--test_data_file <testing set file>" << std::endl;
  cout << "--weights_file <file of pretrained weights>" << std::endl;
  cout << "--train_class_file <file of train classes>" << std::endl;
  cout << "--test_class_file <file of test classes>" << std::endl;
  cout << "--valid_class_file <file of validation classes>" << std::endl;
  cout << "--weights_outfile <output weight file (dimlp.wts by default)>" << std::endl;
  cout << "--train_pred_outfile <output train prediction file (dimlpTrain.out by default)>" << std::endl;
  cout << "--test_pred_file <output test prediction file (dimlpTest.out by default)>" << std::endl;
  cout << "--valid_pred_outfile <output validation prediction file (dimlpValidation.out by default)>" << std::endl;
  cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  cout << "--stats_file <output file with train, test and validation accuracy>" << std::endl;
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
  cout << "--error_thresh <error threshold (None by default )>" << std::endl;
  cout << "--acc_thresh <accuracy threshold (None by default)>" << std::endl;
  cout << "--abs_error_thresh <absolute difference error threshold (0 by default)>" << std::endl;
  cout << "--nb_epochs <number of epochs (1500 by default)>" << std::endl;
  cout << "--nb_epochs_error <number of epochs to show error (10 by default)>" << std::endl;
  cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>" << std::endl;
  cout << "--seed <seed (0=random, default)>";

  cout << "\n-------------------------------------------------\n"
       << std::endl;
}

////////////////////////////////////////////////////////////

static void SaveOutputs(
    DataSet &data,
    std::shared_ptr<Dimlp> net,
    int nbOut,
    int nbWeightLayers,
    const string &outfile)

{
  filebuf buf;

  if (buf.open(outfile, ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open output file " + outfile);
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

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkDimlpTrnParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultString(TRAIN_PRED_OUTFILE, "dimlpTrain.out", true);
  p.setDefaultString(TEST_PRED_OUTFILE, "dimlpTest.out", true);
  p.setDefaultString(VALID_PRED_OUTFILE, "dimlpValidation.out", true);
  p.setDefaultString(WEIGHTS_OUTFILE, "dimlp.wts", true);
  p.setDefaultNbQuantLevels();
  p.setDefaultDimlpTrn();

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersDimlpTrn();
  p.checkParametersNormalization();
}

int dimlpTrn(const string &command) {
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
    stringstream ss(" " + command);

    while (getline(ss, s, ' ')) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2) {
      showDimlpTrnParams();
      return 0;
    }

    // Import parameters
    unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams;
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
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
    checkDimlpTrnParametersLogicValues(*params);

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
    std::string predValidationFile = params->getString(VALID_PRED_OUTFILE);
    std::string outputWeightFile = params->getString(WEIGHTS_OUTFILE);

    float eta = params->getFloat(LEARNING_RATE);
    float mu = params->getFloat(MOMENTUM);
    float flat = params->getFloat(FLAT);
    float errThres = params->getFloat(ERROR_THRESH);
    float accThres = params->getFloat(ACC_THRESH);
    float deltaErr = params->getFloat(ABS_ERROR_THRESH);
    int showErr = params->getInt(NB_EPOCHS_ERROR);
    int epochs = params->getInt(NB_EPOCHS);
    int quant = params->getInt(NB_QUANT_LEVELS);
    int seed = params->getInt(SEED);

    DataSet Train;
    DataSet Test;
    DataSet TrainClass;
    DataSet TestClass;
    DataSet Valid;
    DataSet ValidClass;
    DataSet All;

    AttrName Attr;

    std::shared_ptr<Dimlp> net;

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
    if (params->isStringSet(VALID_DATA_FILE)) {
      if (params->isStringSet(VALID_CLASS_FILE)) {
        DataSet valid(params->getString(VALID_DATA_FILE), nbIn, nbOut);
        DataSet validClass(params->getString(VALID_CLASS_FILE), nbIn, nbOut);

        Valid = valid;
        ValidClass = validClass;
      }

      else {
        DataSet data(params->getString(VALID_DATA_FILE), nbIn, nbOut);

        DataSet valid(data.GetNbEx());
        DataSet validClass(data.GetNbEx());

        data.ExtractDataAndTarget(valid, nbIn, validClass, nbOut);

        Valid = valid;
        ValidClass = validClass;

        data.Del();
      }
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

    if (!params->isStringSet(WEIGHTS_FILE))
      net = std::make_shared<Dimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                    quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile, seed);

    else
      net = std::make_shared<Dimlp>(params->getString(WEIGHTS_FILE), eta, mu, flat, errThres, accThres,
                                    deltaErr, quant, showErr, epochs,
                                    nbLayers, vecNbNeurons, outputWeightFile, seed);

    if (params->isStringSet(STATS_FILE)) {
      ofstream accFile(params->getString(STATS_FILE));
      if (accFile.is_open()) {
        accFile << "Accuracy : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + params->getString(STATS_FILE));
      }
    }

    net->Train(Train, TrainClass, Test, TestClass, Valid, ValidClass, params->getString(STATS_FILE));

    SaveOutputs(Train, net, nbOut, nbWeightLayers, predTrainFile); // Get train predictions
    if (params->isStringSet(TEST_DATA_FILE)) {
      SaveOutputs(Test, net, nbOut, nbWeightLayers, predTestFile); // Get test predictions
    }
    if (params->isStringSet(VALID_DATA_FILE)) {
      SaveOutputs(Valid, net, nbOut, nbWeightLayers, predValidationFile); // Get validation predictions
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

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
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
           << endl;
      cout << "*** RULE EXTRACTION" << std::endl;
      RealHyp ryp1(All, net, quant, nbIn,
                   vecNbNeurons[1] / nbIn, nbWeightLayers);
      if (params->isStringSet(GLOBAL_RULES_OUTFILE)) {
        filebuf buf;

        if (buf.open(params->getString(GLOBAL_RULES_OUTFILE), ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + params->getString(GLOBAL_RULES_OUTFILE));
        }
        ostream rulesFileost(&buf);
        if (params->isDoubleVectorSet(MUS)) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

          if (params->isDoubleVectorSet(MUS)) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost);
          }

        } else

          cout << "\n\n"
               << params->getString(GLOBAL_RULES_OUTFILE) << ": "
               << "Written.\n"
               << std::endl;
      }

      else {
        if (params->isDoubleVectorSet(MUS)) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, cout, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, cout);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

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

/* Exemples to launch the code :

./dimlpTrn --train_data_file irisTrainData.txt --train_class_file irisTrainClass.txt --test_data_file irisTestData.txt --test_class_file irisTestClass.txt --weights_outfile weights.wts --nb_attributes 4 --H2 5 --nb_classes 3 --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --with_rule_extraction true --global_rules_outfile rules.rls --stats_file stats --console_file results.txt --root_folder ../dimlp/datafiles/IrisDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file spamTrainData.txt --train_class_file spamTrainClass.txt --test_data_file spamTestData.txt --test_class_file spamTestClass.txt --weights_outfile spam.wts --nb_attributes 57 --H2 5 --nb_classes 2 --train_pred_outfile spamTrainPred.out --test_pred_outfile spamTestPred.out --with_rule_extraction true --global_rules_outfile spamTrn.rls --stats_file spamTrnStats --console_file spamTrnResult.txt --root_folder ../dimlp/datafiles/spamDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file isoletTrainData.txt --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --test_class_file isoletTestClass.txt --weights_outfile isoletV3.wts --nb_attributes 617 --H2 5 --nb_classes 26 --train_pred_outfile isoletTrainPredV3.out --test_pred_outfile isoletTestPredV3.out --with_rule_extraction true --global_rules_outfile isoletTrnV3.rls --stats_file isoletTrnStatsV3 --console_file isoletTrnResultV3.txt --root_folder ../dimlp/datafiles/isoletDataset --attributes_file attributes.txt
./dimlpTrn --train_data_file Train/X_train.txt --train_class_file Train/y_train.txt --test_data_file test/X_test.txt --test_class_file Test/y_test.txt --weights_outfile HAPT.wts --nb_attributes 561 --H2 5 --nb_classes 12 --train_pred_outfile Train/pred_train.out --test_pred_outfile Test/pred_test.out --with_rule_extraction true --global_rules_outfile HAPTTrain.rls --stats_file HAPTTrnStats --console_file HAPTTrnResult.txt --root_folder ../dimlp/datafiles/HAPTDataset --attributes_file attributes.txt

*/
