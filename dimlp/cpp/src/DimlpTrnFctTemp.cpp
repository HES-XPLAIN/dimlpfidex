#include "DimlpTrnFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void showDimlpTrnParams()

{
  cout << "\n-------------------------------------------------\n"
       << std::endl;

  cout << "DimlpTrn --train_data_file <training set file(path with respect to specified root folder)> ";
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
  cout << "--train_pred_outfile <output train prediction file (dimlp.out by default)>" << std::endl;
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
  cout << "--error_thresh <error threshold (-1111111111 by default)>" << std::endl;
  cout << "--acc_thresh <accuracy threshold (11111111111111 by default)>" << std::endl;
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
  p.setDefaultInt(NB_QUANT_LEVELS, 50);
  p.setDefaultFloat(LEARNING_RATE, 0.1f);
  p.setDefaultFloat(MOMENTUM, 0.6f);
  p.setDefaultFloat(FLAT, 0.01f);
  p.setDefaultFloat(ERROR_THRESH, -1111111111.0f);
  p.setDefaultFloat(ACC_THRESH, 11111111111111.0f);
  p.setDefaultFloat(ABS_ERROR_THRESH, 0.0f);
  p.setDefaultInt(NB_EPOCHS_ERROR, 10);
  p.setDefaultInt(NB_EPOCHS, 1500);
  p.setDefaultBool(WITH_RULE_EXTRACTION, false);
  p.setDefaultInt(SEED, 0);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);

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

  if (p.getFloat(LEARNING_RATE) <= 0) {
    throw CommandArgumentException("The learning parameter must be strictly positive (>=1).");
  }

  if (p.getFloat(MOMENTUM) < 0) {
    throw CommandArgumentException("The momentum parameter must be positive (>=0).");
  }

  if (p.getInt(NB_EPOCHS) < 1) {
    throw CommandArgumentException("Error : Number of epochs must be strictly positive (>=1).");
  }

  if (p.getInt(NB_EPOCHS_ERROR) < 1) {
    throw CommandArgumentException("Error : Number of epochs to show errors must be strictly positive (>=1).");
  }

  if (p.getInt(SEED) < 0) {
    throw CommandArgumentException("Error : random seed mus be positive (>=0).");
  }
}

int dimlpTrn(const string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  try {

    // Parsing the command
    vector<string> commandList;
    string s;
    stringstream ss(command);

    while (getline(ss, s, ' ')) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2) {
      showDimlpTrnParams();
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
    checkDimlpTrnParametersLogicValues(*params);
    std::cout << *params;

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // ----------------------------------------------------------------------

    // Get parameters values

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

    if (learnFileInit == false) {
      throw CommandArgumentException("Give the training file with --train_data_file selection please.");
    }

    if (learnTarInit != false) {
      DataSet train(learnFile, nbIn, nbOut);
      DataSet trainClass(learnTar, nbIn, nbOut);

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
    if (validFileInit != false) {
      if (validTarInit != false) {
        DataSet valid(validFile, nbIn, nbOut);
        DataSet validClass(validTar, nbIn, nbOut);

        Valid = valid;
        ValidClass = validClass;
      }

      else {
        DataSet data(validFile, nbIn, nbOut);

        DataSet valid(data.GetNbEx());
        DataSet validClass(data.GetNbEx());

        data.ExtractDataAndTarget(valid, nbIn, validClass, nbOut);

        Valid = valid;
        ValidClass = validClass;

        data.Del();
      }
    }

    if (testFileInit != false) {
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

    if (weightFileInit == false)
      net = std::make_shared<Dimlp>(eta, mu, flat, errThres, accThres, deltaErr,
                                    quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile, seed);

    else
      net = std::make_shared<Dimlp>(weightFile, eta, mu, flat, errThres, accThres,
                                    deltaErr, quant, showErr, epochs,
                                    nbLayers, vecNbNeurons, outputWeightFile, seed);

    if (accuracyFileInit != false) {
      ofstream accFile(accuracyFile);
      if (accFile.is_open()) {
        accFile << "Accuracy : \n"
                << std::endl;
        accFile.close();
      } else {
        throw CannotOpenFileError("Error : Cannot open accuracy file " + std::string(accuracyFile));
      }
    }

    net->Train(Train, TrainClass, Test, TestClass, Valid, ValidClass, accuracyFile);

    SaveOutputs(Train, net, nbOut, nbWeightLayers, predTrainFile); // Get train predictions
    if (testFileInit != false) {
      SaveOutputs(Test, net, nbOut, nbWeightLayers, predTestFile); // Get test predictions
    }
    if (validFileInit != false) {
      SaveOutputs(Valid, net, nbOut, nbWeightLayers, predValidationFile); // Get validation predictions
    }

    if (ruleExtr) {
      vector<string> attributeNames;
      if (attrFileInit != false) {
        AttrName attr(attrFile, nbIn, nbOut);

        if (attr.ReadAttr())
          cout << "\n\n"
               << attrFile << ": Read file of attributes.\n"
               << std::endl;

        Attr = attr;
        attributeNames = Attr.GetListAttr();
      }

      All = Train;
      if (rulesFileInit != false) {
        cout << "Extraction Part :: " << std::endl;
      }

      if (Valid.GetNbEx() > 0) {
        DataSet all2(All, Valid);
        All = all2;
      }

      // Get mus, sigmas and normalizationIndices from normalizationFile for denormalization :
      if (normalizationFileInit) {
        auto results = parseNormalizationStats(normalizationFile, nbIn, attributeNames);
        normalizationIndices = std::get<0>(results);
        mus = std::get<2>(results);
        sigmas = std::get<3>(results);
        hasMus = true;
      }

      cout << "\n\n****************************************************\n"
           << endl;
      cout << "*** RULE EXTRACTION" << std::endl;
      RealHyp ryp1(All, net, quant, nbIn,
                   vecNbNeurons[1] / nbIn, nbWeightLayers);
      if (rulesFileInit != false) {
        filebuf buf;

        if (buf.open(rulesFile, ios_base::out) == nullptr) {
          throw CannotOpenFileError("Error : Cannot open rules file " + std::string(rulesFile));
        }
        ostream rulesFileost(&buf);
        if (hasMus) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, rulesFileost);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

          if (hasMus) {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost, mus, sigmas, normalizationIndices);
          } else {
            ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                                Test, TestClass, Attr, rulesFileost);
          }

        } else

          cout << "\n\n"
               << rulesFile << ": "
               << "Written.\n"
               << std::endl;
      }

      else {
        if (hasMus) {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, cout, mus, sigmas, normalizationIndices);
        } else {
          ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                              Test, TestClass, Attr, cout);
        }

        if (ryp1.TreeAborted()) {

          RealHyp2 ryp2(All, net, quant, nbIn,
                        vecNbNeurons[1] / nbIn, nbWeightLayers);

          if (hasMus) {
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

./DimlpTrn --train_data_file irisTrainData.txt --train_class_file irisTrainClass.txt --test_data_file irisTestData.txt --test_class_file irisTestClass.txt --weights_outfile weights.wts --nb_attributes 4 --H2 5 --nb_classes 3 --train_pred_outfile predTrain.out --test_pred_file predTest.out --with_rule_extraction --global_rules_outfile rules.rls --stats_file stats --console_file results.txt --root_folder ../dimlp/datafiles/IrisDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file spamTrainData.txt --train_class_file spamTrainClass.txt --test_data_file spamTestData.txt --test_class_file spamTestClass.txt --weights_outfile spam.wts --nb_attributes 57 --H2 5 --nb_classes 2 --train_pred_outfile spamTrainPred.out --test_pred_file spamTestPred.out --with_rule_extraction --global_rules_outfile spamTrn.rls --stats_file spamTrnStats --console_file spamTrnResult.txt --root_folder ../dimlp/datafiles/spamDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file isoletTrainData.txt --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --test_class_file isoletTestClass.txt --weights_outfile isoletV3.wts --nb_attributes 617 --H2 5 --nb_classes 26 --train_pred_outfile isoletTrainPredV3.out --test_pred_file isoletTestPredV3.out --with_rule_extraction --global_rules_outfile isoletTrnV3.rls --stats_file isoletTrnStatsV3 --console_file isoletTrnResultV3.txt --root_folder ../dimlp/datafiles/isoletDataset --attributes_file attributes.txt
./DimlpTrn --train_data_file Train/X_train.txt --train_class_file Train/y_train.txt --test_data_file test/X_test.txt --test_class_file Test/y_test.txt --weights_outfile HAPT.wts --nb_attributes 561 --H2 5 --nb_classes 12 --train_pred_outfile Train/pred_train.out --test_pred_file Test/pred_test.out --with_rule_extraction --global_rules_outfile HAPTTrain.rls --stats_file HAPTTrnStats --console_file HAPTTrnResult.txt --root_folder ../dimlp/datafiles/HAPTDataset --attributes_file attributes.txt

*/
