#include "DimlpTrnFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpTrn()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DimlpTrn -L <training set file(path with respect to specified root folder)> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";
  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-A <file of attributes>\n";
  cout << "-V <validation set file>\n";
  cout << "-T <testing set file>\n";
  cout << "-W <file of pretrained weights>\n";
  cout << "-1 <file of train classes>\n";
  cout << "-2 <file of test classes>\n";
  cout << "-3 <file of validation classes>\n";
  cout << "-w <output weight file>\n";                     // If we want to specify weight output file, not to be weights.wts
  cout << "-p <output train prediction file>\n";           // If we want to specify output train prediction file, not to be dimlp.out
  cout << "-t <output test prediction file>\n";            // If we want to specify output test prediction file, not to be dimlpTest.out
  cout << "-v <output validation prediction file>\n";      // If we want to specify output validation prediction file, not to be dimlpValidation.out
  cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  cout << "-o <output file with train, test and validation accuracy>\n";
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)\n";
  cout << "-Hk <number of neurons in the kth hidden layer>\n";
  cout << "-R (RULE EXTRACTION)\n";
  cout << "-F <extraction ruleFile>\n"; // If we want to extract rules in a rulesFile instead of console
  cout << "-l <back-propagation learning parameter (Eta)>\n";
  cout << "-m <back-propagation momentum parameter (Mu)>\n";
  cout << "-f <back-propagation flat spot elimination parameter (Flat)>\n";
  cout << "-q <number of stairs in staircase activation function>\n";
  cout << "-e <error threshold>\n";
  cout << "-a <accuracy threshold>\n";
  cout << "-d <absolute difference error threshold>\n";
  cout << "-i <number of epochs>\n";
  cout << "-s <number of epochs to show error>\n";
  cout << "-z <seed (0=ranodom)>";

  cout << "\n-------------------------------------------------\n\n";
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

int dimlpTrn(const string &command) {

  float temps;
  clock_t t1;
  clock_t t2;

  t1 = clock();

  // Parsing the command
  vector<string> commandList;
  const char delim = ' ';
  string s;
  stringstream ss(command);
  while (std::getline(ss, s, delim)) {
    commandList.push_back(s);
  }
  size_t nbParam = commandList.size();

  DataSet Train;
  DataSet Test;
  DataSet TrainClass;
  DataSet TestClass;
  DataSet Valid;
  DataSet ValidClass;
  DataSet All;

  AttrName Attr;

  std::shared_ptr<Dimlp> net;

  float eta = 0.1f;
  float mu = 0.6f;
  float flat = 0.01f;
  float errThres = -1111111111.0f;
  float accThres = 11111111111111.0f;
  float deltaErr = 0;
  int showErr = 10;
  int epochs = 1500;
  int quant = 50;

  int ruleExtr = 0;

  int nbIn = 0;
  int nbOut = 0;
  int seed = 0;

  string learnFileTemp;
  bool learnFileInit = false;
  string testFileTemp;
  bool testFileInit = false;
  string validFileTemp;
  bool validFileInit = false;
  string weightFileTemp;
  bool weightFileInit = false;
  string outputWeightFileTemp = "dimlp.wts";
  string predTrainFileTemp = "dimlp.out";
  string predTestFileTemp = "dimlpTest.out";
  string predValidationFileTemp = "dimlpValidation.out";
  string rulesFileTemp;
  bool rulesFileInit = false;
  string consoleFileTemp;
  bool consoleFileInit = false;
  string accuracyFileTemp;
  bool accuracyFileInit = false;
  string learnTarTemp;
  bool learnTarInit = false;
  string testTarTemp;
  bool testTarInit = false;
  string validTarTemp;
  bool validTarInit = false;
  string attrFileTemp;
  bool attrFileInit = false;
  string rootFolderTemp;
  bool rootFolderInit = false;

  int flagEp = 0;

  int nbLayers;
  int nbWeightLayers;
  std::vector<int> vecNbNeurons;

  StringInt arch;
  StringInt archInd;

  if (nbParam <= 1) {
    GiveAllParamDimlpTrn();
    return 0;
  }

  int k = 1; // We skip "DimlpTrn"
  while (k < nbParam) {
    if (commandList[k][0] == '-') {
      k++;

      if (k >= nbParam && *(&(commandList[k - 1])[0] + 1) != 'R') {
        cout << "Missing something at the end of the command.\n";
        return -1;
      }

      char option = commandList[k - 1][1];
      const char *arg = &(commandList[k])[0];
      const char *lastArg = &(commandList[k - 1])[0];
      switch (option) {
      case 'l':
        if (CheckFloat(arg))
          eta = static_cast<float>(atof(arg));
        else
          return -1;

        break;

      case 'm':
        if (CheckFloat(arg))
          mu = static_cast<float>(atof(arg));
        else
          return -1;

        break;

      case 'f':
        if (CheckFloat(arg))
          flat = static_cast<float>(atof(arg));
        else
          return -1;

        break;

      case 'e':
        if (CheckFloat(arg))
          errThres = static_cast<float>(atof(arg));
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 'a':
        if (CheckFloat(arg))
          accThres = static_cast<float>(atof(arg));
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 'd':
        if (CheckFloat(arg))
          deltaErr = static_cast<float>(atof(arg));
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 's':
        if (CheckInt(arg))
          showErr = atoi(arg);
        else
          return -1;

        break;

      case 'i':
        if (CheckInt(arg)) {
          epochs = atoi(arg);
          flagEp = 1;
        } else
          return -1;

        break;

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

      case 'z':
        if (CheckInt(arg))
          seed = atoi(arg);
        else
          return -1;

        break;

      case 'S':
        rootFolderTemp = arg;
        rootFolderInit = true;
        break;

      case 'A':
        attrFileTemp = arg;
        attrFileInit = true;
        break;

      case 'W':
        weightFileTemp = arg;
        weightFileInit = true;
        break;

      case 'w':
        outputWeightFileTemp = arg;
        break;

      case 'p':
        predTrainFileTemp = arg;
        break;

      case 't':
        predTestFileTemp = arg;
        break;

      case 'v':
        predValidationFileTemp = arg;
        break;

      case 'r':
        consoleFileTemp = arg;
        consoleFileInit = true;
        break;

      case 'o':
        accuracyFileTemp = arg;
        accuracyFileInit = true;
        break;

      case 'L':
        learnFileTemp = arg;
        learnFileInit = true;
        break;

      case 'T':
        testFileTemp = arg;
        testFileInit = true;
        break;

      case 'V':
        validFileTemp = arg;
        validFileInit = true;
        break;

      case '1':
        learnTarTemp = arg;
        learnTarInit = true;
        break;

      case '2':
        testTarTemp = arg;
        testTarInit = true;
        break;

      case '3':
        validTarTemp = arg;
        validTarInit = true;
        break;

      case 'R':
        ruleExtr = 1;
        k--;
        break;

      case 'F':
        rulesFileTemp = arg;
        rulesFileInit = true;
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

  const char *learnFile = nullptr;
  const char *testFile = nullptr;
  const char *validFile = nullptr;
  const char *weightFile = nullptr;
  const char *outputWeightFile = nullptr;
  const char *predTrainFile = nullptr;
  const char *predTestFile = nullptr;
  const char *predValidationFile = nullptr;
  const char *rulesFile = nullptr;
  const char *consoleFile = nullptr;
  const char *accuracyFile = nullptr;
  const char *learnTar = nullptr;
  const char *testTar = nullptr;
  const char *validTar = nullptr;
  const char *attrFile = nullptr;

  string root = "";
  if (rootFolderInit) {
#if defined(__unix__) || defined(__APPLE__)
    root = rootFolderTemp + "/";
#elif defined(_WIN32)
    root = rootFolderTemp + "\\";
#endif
  }

  if (learnFileInit) {
    learnFileTemp = root + learnFileTemp;
    learnFile = &learnFileTemp[0];
  }

  if (testFileInit) {
    testFileTemp = root + testFileTemp;
    testFile = &testFileTemp[0];
  }

  if (validFileInit) {
    validFileTemp = root + validFileTemp;
    validFile = &validFileTemp[0];
  }

  if (weightFileInit) {
    weightFileTemp = root + weightFileTemp;
    weightFile = &weightFileTemp[0];
  }

  outputWeightFileTemp = root + outputWeightFileTemp;
  outputWeightFile = &outputWeightFileTemp[0];

  predTrainFileTemp = root + predTrainFileTemp;
  predTrainFile = &predTrainFileTemp[0];

  predTestFileTemp = root + predTestFileTemp;
  predTestFile = &predTestFileTemp[0];

  predValidationFileTemp = root + predValidationFileTemp;
  predValidationFile = &predValidationFileTemp[0];

  if (rulesFileInit) {
    rulesFileTemp = root + rulesFileTemp;
    rulesFile = &rulesFileTemp[0];
  }

  if (consoleFileInit) {
    consoleFileTemp = root + consoleFileTemp;
    consoleFile = &consoleFileTemp[0];
  }

  if (accuracyFileInit) {
    accuracyFileTemp = root + accuracyFileTemp;
    accuracyFile = &accuracyFileTemp[0];
  }

  if (learnTarInit) {
    learnTarTemp = root + learnTarTemp;
    learnTar = &learnTarTemp[0];
  }

  if (testTarInit) {
    testTarTemp = root + testTarTemp;
    testTar = &testTarTemp[0];
  }

  if (validTarInit) {
    validTarTemp = root + validTarTemp;
    validTar = &validTarTemp[0];
  }

  if (attrFileInit) {
    attrFileTemp = root + attrFileTemp;
    attrFile = &attrFileTemp[0];
  }

  // ----------------------------------------------------------------------

  // Get console results to file
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  if (consoleFileInit != false) {
    ofs.open(consoleFile);
    std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
  }

  // ----------------------------------------------------------------------

  if (eta <= 0) {
    cout << "The learning parameter must be greater than 0.\n";
    return -1;
  }

  if (mu < 0) {
    cout << "The momentum parameter must be greater or equal to 0.\n";
    return -1;
  }

  if (showErr == 0) {
    cout << "The number of epochs must be greater than 0.\n";
    return -1;
  }

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
  }

  else {
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

  if (learnFileInit == false) {
    cout << "Give the training file with -L selection please."
         << "\n";
    return -1;
  }

  if (learnTarInit != false) {
    DataSet train(learnFile, nbIn);
    DataSet trainClass(learnTar, nbOut);

    Train = train;
    TrainClass = trainClass;
  } else {
    DataSet data(learnFile, nbIn + nbOut);

    DataSet train(data.GetNbEx());
    DataSet trainClass(data.GetNbEx());

    data.ExtractDataAndTarget(train, nbIn, trainClass, nbOut);

    Train = train;
    TrainClass = trainClass;

    data.Del();
  }
  if (validFileInit != false) {
    if (validTarInit != false) {
      DataSet valid(validFile, nbIn);
      DataSet validClass(validTar, nbOut);

      Valid = valid;
      ValidClass = validClass;
    }

    else {
      DataSet data(validFile, nbIn + nbOut);

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
      accFile << "Accuracy : \n\n";
      accFile.close();
    } else {
      string errorMsg = "Cannot open file for writing";
      WriteError(errorMsg, accuracyFile);
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

    if (attrFileInit != false) {
      AttrName attr(attrFile, nbIn, nbOut);

      if (attr.ReadAttr())
        cout << "\n\n"
             << attrFile << ": Read file of attributes.\n\n";

      Attr = attr;
    }

    All = Train;
    if (rulesFileInit != false) {
      cout << "Extraction Part :: " << endl;
    }

    if (Valid.GetNbEx() > 0) {
      DataSet all2(All, Valid);
      All = all2;
    }

    cout << "\n\n****************************************************\n"
         << endl;
    cout << "*** RULE EXTRACTION" << endl;
    RealHyp ryp1(All, net, quant, nbIn,
                 vecNbNeurons[1] / nbIn, nbWeightLayers);
    if (rulesFileInit != false) {
      filebuf buf;

      if (buf.open(rulesFile, ios_base::out) == nullptr) {
        string errorMsg = "Cannot open file for writing";
        WriteError(errorMsg, rulesFile);
      }
      ostream rulesFileost(&buf);
      ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                          Test, TestClass, Attr, rulesFileost);

      if (ryp1.TreeAborted()) {

        RealHyp2 ryp2(All, net, quant, nbIn,
                      vecNbNeurons[1] / nbIn, nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost);

      } else

        cout << "\n\n"
             << rulesFile << ": "
             << "Written.\n\n";
    }

    else {
      ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                          Test, TestClass, Attr, cout);

      if (ryp1.TreeAborted()) {

        RealHyp2 ryp2(All, net, quant, nbIn,
                      vecNbNeurons[1] / nbIn, nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, cout);
      }
    }
  }

  t2 = clock();
  temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
  std::cout << "\nFull execution time = " << temps << " sec\n";

  std::cout.rdbuf(cout_buff); // reset to standard output again

  BpNN::resetInitRandomGen();

  return 0;
}

/* Exemples to launch the code :

./DimlpTrn -L covidTrainData.txt -1 covidTrainClass.txt -T covidTestData.txt -2 covidTestClass.txt -w covid.wts -I 20 -H2 5 -O 2 -p covidTrainPred.out -t covidTestPred.out -R -F covidTrn.rls -o covidTrnStats -r covidTrnResult.txt -S dimlp/datafiles/covidDataset -A attributes.txt
./DimlpTrn -L spamTrainData.txt -1 spamTrainClass.txt -T spamTestData.txt -2 spamTestClass.txt -w spam.wts -I 57 -H2 5 -O 2 -p spamTrainPred.out -t spamTestPred.out -R -F spamTrn.rls -o spamTrnStats -r spamTrnResult.txt -S dimlp/datafiles/spamDataset -A attributes.txt
./DimlpTrn -L isoletTrainData.txt -1 isoletTrainClass.txt -T isoletTestData.txt -2 isoletTestClass.txt -w isoletV3.wts -I 617 -H2 5 -O 26 -p isoletTrainPredV3.out -t isoletTestPredV3.out -R -F isoletTrnV3.rls -o isoletTrnStatsV3 -r isoletTrnResultV3.txt -S dimlp/datafiles/isoletDataset -A attributes.txt
./DimlpTrn -L Train/X_train.txt -1 Train/y_train.txt -T test/X_test.txt -2 Test/y_test.txt -w HAPT.wts -I 561 -H2 5 -O 12 -p Train/pred_train.out -t Test/pred_test.out -R -F HAPTTrain.rls -o HAPTTrnStats -r HAPTTrnResult.txt -S dimlp/datafiles/HAPTDataset -A attributes.txt

*/
