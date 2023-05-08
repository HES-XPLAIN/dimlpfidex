using namespace std;

#define BPNN 1
#include "DimlpTrnFct.h"
////////////////////////////////////////////////////////////

void GiveAllParam()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DimlpTrn -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
  cout << "-L <training set file(path with respect to specified root folder)> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";
  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
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
  cout << "-v <output validation prediction file>\n";      // If we want to specify output test prediction file, not to be dimlpValidation.out
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

void SaveOutputs(
    DataSet &data,
    Dimlp *net,
    int nbOut,
    int nbWeightLayers,
    char *outfile)

{
  int p, o;
  filebuf buf;

  if (buf.open(outfile, ios_base::out) == 0) {
    char errorMsg[] = "Cannot open file for writing";
    WriteError(errorMsg, outfile);
  }

  Layer *layer = net->GetLayer(nbWeightLayers - 1);
  float *out = layer->GetUp();

  cout << "\n\n"
       << outfile << ": "
       << "Writing ...\n";

  ostream outFile(&buf);

  for (p = 0; p < data.GetNbEx(); p++) {
    net->ForwardOneExample1(data, p);

    for (o = 0; o < nbOut; o++) {
      outFile << out[o] << " ";
    }

    outFile << "\n";
  }

  cout << outfile << ": "
       << "Written.\n\n";
}

////////////////////////////////////////////////////////////

int dimlpTrn(string command) {

  float temps;
  clock_t t1, t2;

  t1 = clock();

  // Parsing the command
  vector<string> commandList;
  const char delim = ' ';
  string s;
  stringstream ss(command);
  while (std::getline(ss, s, delim)) {
    commandList.push_back(s);
  }
  int nbParam = commandList.size();

  int k;

  DataSet Train;
  DataSet Test;
  DataSet TrainClass;
  DataSet TestClass;
  DataSet Valid;
  DataSet ValidClass;
  DataSet All;

  AttrName Attr;

  Dimlp *net;

  float eta = 0.1;
  float mu = 0.6;
  float flat = 0.01;
  float errThres = -1111111111.0;
  float accThres = 11111111111111.0;
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
  int *vecNbNeurons;

  StringInt arch;
  StringInt archInd;

  if (nbParam == 1) {
    GiveAllParam();
    return -1;
  }

  for (k = 1; k < nbParam; k++) {
    if (commandList[k][0] == '-') {
      k++;

      if (k >= nbParam) {
        if (*(&(commandList[k - 1])[0] + 1) != 'R') {
          cout << "Missing something at the end of the command.\n";
          return -1;
        }
      }

      switch (commandList[k - 1][1]) {
      case 'l':
        if (CheckFloat(&(commandList[k])[0]))
          eta = atof(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'm':
        if (CheckFloat(&(commandList[k])[0]))
          mu = atof(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'f':
        if (CheckFloat(&(commandList[k])[0]))
          flat = atof(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'e':
        if (CheckFloat(&(commandList[k])[0]))
          errThres = atof(&(commandList[k])[0]);
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 'a':
        if (CheckFloat(&(commandList[k])[0]))
          accThres = atof(&(commandList[k])[0]);
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 'd':
        if (CheckFloat(&(commandList[k])[0]))
          deltaErr = atof(&(commandList[k])[0]);
        else
          return -1;

        if (flagEp == 0)
          epochs = 2000000000;
        break;

      case 's':
        if (CheckInt(&(commandList[k])[0]))
          showErr = atoi(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'i':
        if (CheckInt(&(commandList[k])[0])) {
          epochs = atoi(&(commandList[k])[0]);
          flagEp = 1;
        } else
          return -1;

        break;

      case 'q':
        if (CheckInt(&(commandList[k])[0]))
          quant = atoi(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'I':
        if (CheckInt(&(commandList[k])[0]))
          nbIn = atoi(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'H':
        if (CheckInt(&(commandList[k])[0])) {
          arch.Insert(atoi(&(commandList[k])[0]));

          char *ptrParam = &(commandList[k - 1])[0];

          if (ptrParam[2] != '\0') {
            char str[80];

            strcpy(str, ptrParam + 2);
            archInd.Insert(atoi(str));
          } else {
            cout << "Which hidden layer (-H) ?\n";
            return -1;
          }
        } else
          return -1;

        break;

      case 'O':
        if (CheckInt(&(commandList[k])[0]))
          nbOut = atoi(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'z':
        if (CheckInt(&(commandList[k])[0]))
          seed = atoi(&(commandList[k])[0]);
        else
          return -1;

        break;

      case 'S':
        rootFolderTemp = &(commandList[k])[0];
        rootFolderInit = true;
        break;

      case 'A':
        attrFileTemp = &(commandList[k])[0];
        attrFileInit = true;
        break;

      case 'W':
        weightFileTemp = &(commandList[k])[0];
        weightFileInit = true;
        break;

      case 'w':
        outputWeightFileTemp = &(commandList[k])[0];
        break;

      case 'p':
        predTrainFileTemp = &(commandList[k])[0];
        break;

      case 't':
        predTestFileTemp = &(commandList[k])[0];
        break;

      case 'v':
        predValidationFileTemp = &(commandList[k])[0];
        break;

      case 'r':
        consoleFileTemp = &(commandList[k])[0];
        consoleFileInit = true;
        break;

      case 'o':
        accuracyFileTemp = &(commandList[k])[0];
        accuracyFileInit = true;
        break;

      case 'L':
        learnFileTemp = &(commandList[k])[0];
        learnFileInit = true;
        break;

      case 'T':
        testFileTemp = &(commandList[k])[0];
        testFileInit = true;
        break;

      case 'V':
        validFileTemp = &(commandList[k])[0];
        validFileInit = true;
        break;

      case '1':
        learnTarTemp = &(commandList[k])[0];
        learnTarInit = true;
        break;

      case '2':
        testTarTemp = &(commandList[k])[0];
        testTarInit = true;
        break;

      case '3':
        validTarTemp = &(commandList[k])[0];
        validTarInit = true;
        break;

      case 'R':
        ruleExtr = 1;
        k--;
        break;

      case 'F':
        rulesFileTemp = &(commandList[k])[0];
        rulesFileInit = true;
        break;

      default:
        cout << "Illegal option: " << &(commandList[k - 1])[0] << "\n";
        return -1;
      }
    }

    else {
      cout << "Illegal option: " << &(commandList[k])[0] << "\n";
      return -1;
    }
  }

  // ----------------------------------------------------------------------
  // create paths with root foler
  char learnFileTmp[160], testFileTmp[160], validFileTmp[160], weightFileTmp[160], outputWeightFileTmp[160], predTrainFileTmp[160], predTestFileTmp[160], predValidationFileTmp[160], rulesFileTmp[160], consoleFileTmp[160], accuracyFileTmp[160], learnTarTmp[160], testTarTmp[160], validTarTmp[160], attrFileTmp[160];

  char *learnFile = 0;
  char *testFile = 0;
  char *validFile = 0;
  char *weightFile = 0;
  char *outputWeightFile = 0;
  char *predTrainFile = 0;
  char *predTestFile = 0;
  char *predValidationFile = 0;
  char *rulesFile = 0;
  char *consoleFile = 0;
  char *accuracyFile = 0;
  char *learnTar = 0;
  char *testTar = 0;
  char *validTar = 0;
  char *attrFile = 0;

#ifdef __unix__
  string root = rootFolderTemp + "/";
#elif defined(_WIN32)
  string root = rootFolderTemp + "\\";
#endif
  if (learnFileInit) {
    learnFileTemp = root + learnFileTemp;
    if (learnFileTemp.length() >= 160) {
      cout << "Path " << learnFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(learnFileTmp, learnFileTemp.c_str());
    learnFile = learnFileTmp;
  }

  if (testFileInit) {
    testFileTemp = root + testFileTemp;
    if (testFileTemp.length() >= 160) {
      cout << "Path " << testFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(testFileTmp, testFileTemp.c_str());
    testFile = testFileTmp;
  }

  if (validFileInit) {
    validFileTemp = root + validFileTemp;
    if (validFileTemp.length() >= 160) {
      cout << "Path " << validFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(validFileTmp, validFileTemp.c_str());
    validFile = validFileTmp;
  }

  if (weightFileInit) {
    weightFileTemp = root + weightFileTemp;
    if (weightFileTemp.length() >= 160) {
      cout << "Path " << weightFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(weightFileTmp, weightFileTemp.c_str());
    weightFile = weightFileTmp;
  }

  outputWeightFileTemp = root + outputWeightFileTemp;
  if (outputWeightFileTemp.length() >= 160) {
    cout << "Path " << outputWeightFileTemp << "is too long"
         << "\n";
    return -1;
  }
  strcpy(outputWeightFileTmp, outputWeightFileTemp.c_str());
  outputWeightFile = outputWeightFileTmp;

  predTrainFileTemp = root + predTrainFileTemp;
  if (predTrainFileTemp.length() >= 160) {
    cout << "Path " << predTrainFileTemp << "is too long"
         << "\n";
    return -1;
  }
  strcpy(predTrainFileTmp, predTrainFileTemp.c_str());
  predTrainFile = predTrainFileTmp;

  predTestFileTemp = root + predTestFileTemp;
  if (predTestFileTemp.length() >= 160) {
    cout << "Path " << predTestFileTemp << "is too long"
         << "\n";
    return -1;
  }
  strcpy(predTestFileTmp, predTestFileTemp.c_str());
  predTestFile = predTestFileTmp;

  predValidationFileTemp = root + predValidationFileTemp;
  if (predValidationFileTemp.length() >= 160) {
    cout << "Path " << predValidationFileTemp << "is too long"
         << "\n";
    return -1;
  }
  strcpy(predValidationFileTmp, predValidationFileTemp.c_str());
  predValidationFile = predValidationFileTmp;

  if (rulesFileInit) {
    rulesFileTemp = root + rulesFileTemp;
    if (rulesFileTemp.length() >= 160) {
      cout << "Path " << rulesFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(rulesFileTmp, rulesFileTemp.c_str());
    rulesFile = rulesFileTmp;
  }

  if (consoleFileInit) {
    consoleFileTemp = root + consoleFileTemp;
    if (consoleFileTemp.length() >= 160) {
      cout << "Path " << consoleFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(consoleFileTmp, consoleFileTemp.c_str());
    consoleFile = consoleFileTmp;
  }

  if (accuracyFileInit) {
    accuracyFileTemp = root + accuracyFileTemp;
    if (accuracyFileTemp.length() >= 160) {
      cout << "Path " << accuracyFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(accuracyFileTmp, accuracyFileTemp.c_str());
    accuracyFile = accuracyFileTmp;
  }

  if (learnTarInit) {
    learnTarTemp = root + learnTarTemp;
    if (learnTarTemp.length() >= 160) {
      cout << "Path " << learnTarTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(learnTarTmp, learnTarTemp.c_str());
    learnTar = learnTarTmp;
  }

  if (testTarInit) {
    testTarTemp = root + testTarTemp;
    if (testTarTemp.length() >= 160) {
      cout << "Path " << testTarTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(testTarTmp, testTarTemp.c_str());
    testTar = testTarTmp;
  }

  if (validTarInit) {
    validTarTemp = root + validTarTemp;
    if (validTarTemp.length() >= 160) {
      cout << "Path " << validTarTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(validTarTmp, validTarTemp.c_str());
    validTar = validTarTmp;
  }

  if (attrFileInit) {
    attrFileTemp = root + attrFileTemp;
    if (attrFileTemp.length() >= 160) {
      cout << "Path " << attrFileTemp << "is too long"
           << "\n";
      return -1;
    }
    strcpy(attrFileTmp, attrFileTemp.c_str());
    attrFile = attrFileTmp;
  }

  // ----------------------------------------------------------------------

  // Get console results to file
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  if (consoleFileInit != false) {
    ofs.open(consoleFile);
    std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
  }
  std::ostream &output = consoleFileInit != false ? ofs : std::cout;

  // ----------------------------------------------------------------------

  if (rootFolderInit == false) {
    cout << "Give a root folder to save results with -S selection please."
         << "\n";
    return -1;
  }

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

    vecNbNeurons = new int[nbLayers];
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

      vecNbNeurons = new int[nbLayers];
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

      vecNbNeurons = new int[nbLayers];
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
    net = new Dimlp(eta, mu, flat, errThres, accThres, deltaErr,
                    quant, showErr, epochs, nbLayers, vecNbNeurons, outputWeightFile, seed);

  else
    net = new Dimlp(weightFile, eta, mu, flat, errThres, accThres,
                    deltaErr, quant, showErr, epochs,
                    nbLayers, vecNbNeurons, outputWeightFile, seed);

  if (accuracyFileInit != false) {
    ofstream accFile(accuracyFile);
    if (accFile.is_open()) {
      accFile << "Accuracy : \n\n";
      accFile.close();
    } else {
      char errorMsg[] = "Cannot open file for writing";
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
      static DataSet all2(All, Valid);
      All = all2;
    }

    cout << "\n\n****************************************************\n\n";
    cout << "*** RULE EXTRACTION\n";

    RealHyp ryp1(All, net, quant, nbIn,
                 vecNbNeurons[1] / nbIn, nbWeightLayers);

    if (rulesFileInit != false) {
      filebuf buf;

      if (buf.open(rulesFile, ios_base::out) == 0) {
        char errorMsg[] = "Cannot open file for writing";
        WriteError(errorMsg, rulesFile);
      }

      ostream rulesFileost(&buf);

      ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                          Test, TestClass, Attr, rulesFileost);

      if (ryp1.TreeAborted()) {
        ryp1.Del();

        RealHyp2 ryp2(All, net, quant, nbIn,
                      vecNbNeurons[1] / nbIn, nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost);

        ryp2.Del();
      } else
        ryp1.Del();

      cout << "\n\n"
           << rulesFile << ": "
           << "Written.\n\n";
    }

    else {
      ryp1.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                          Test, TestClass, Attr, cout);

      if (ryp1.TreeAborted()) {
        ryp1.Del();

        RealHyp2 ryp2(All, net, quant, nbIn,
                      vecNbNeurons[1] / nbIn, nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, cout);

        ryp2.Del();
      } else
        ryp1.Del();
    }

    if (attrFileInit != false)
      Attr.Del();
  }

  // Train.Del();
  // TrainClass.Del();

  net->Del();

  /*if (Test.GetNbEx() > 0)
  {
     Test.Del();
     TestClass.Del();
  }

  if (Valid.GetNbEx() > 0)
  {
     Valid.Del();
     ValidClass.Del();
  }*/

  t2 = clock();
  temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
  std::cout << "\nFull execution time = " << temps << " sec\n";

  std::cout.rdbuf(cout_buff); // reset to standard output again

  return 0;
}

/*
int main(int nbParam, char** param)
{
    dimlpTrn("DimlpTrn -L ../dimlp/datafiles/datanormTrain -1 ../dimlp/datafiles/dataclass2Train -T ../dimlp/datafiles/datanormTest -2 ../dimlp/datafiles/dataclass2Test -w ../dimlp/datafiles/dimlpDatanorm.wts -I 16 -H2 5 -O 2 -p ../dimlp/datafiles/dimlpDatanormTrain.out -t ../dimlp/datafiles/dimlpDatanormTest.out -R -F ../dimlp/datafiles/dimlpDatanormTrn.rls -o ../dimlp/datafiles/dimlpDatanormTrnStats -r ../dimlp/datafiles/dimlpDatanormTrnResult.txt");
}*/
