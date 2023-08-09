#include "DensClsFct.h"
using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDensCls()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DensCls -L <training set file(path with respect to specified root folder)> ";
  cout << "-W <Prefix of file of weights> (for instance give DimlpBT) ";
  cout << "-I <number of input neurons> -O <number of output neurons> ";
  cout << "-N <number of networks>";
  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-A <file of attributes>\n";
  cout << "-T <testing set file>\n";
  cout << "-1 <file of train classes>\n";
  cout << "-2 <file of test classes>\n";
  cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  cout << "-p <output train prediction file>\n";           // If we want to specify output train prediction file, not to be densCls.out
  cout << "-t <output test prediction file>\n";            // If we want to specify output test prediction file, not to be densClsTest.out
  cout << "-o <output file with global train and test accuracy>\n";
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)\n";
  cout << "-Hk <number of neurons in the kth hidden layer>\n";
  cout << "-R (RULE EXTRACTION)\n";
  cout << "-F <extraction ruleFile>\n"; // If we want to extract rules in a rulesFile instead of console
  cout << "-q <number of stairs in staircase activation function>\n";

  cout << "\n-------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////

int densCls(const string &command) {

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

  int nbDimlpNets = 0;

  int ruleExtr = 0;

  int nbIn = 0;
  int nbOut = 0;
  int quant = 50;

  string learnFileTemp;
  bool learnFileInit = false;
  string testFileTemp;
  bool testFileInit = false;
  string validFileTemp;
  bool validFileInit = false;
  string weightFileTemp;
  bool weightFileInit = false;
  string weightFileSaveTemp = "dimlp.wts";
  string predTrainFileTemp = "densCls.out";
  string predTestFileTemp = "densClsTest.out";
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

  int nbLayers;
  int nbWeightLayers;
  std::vector<int> vecNbNeurons;

  StringInt arch;
  StringInt archInd;

  if (nbParam <= 1) {
    GiveAllParamDensCls();
    return 0;
  }

  int k = 1; // We skip "DensCls"
  while (k < nbParam) {
    if (commandList[k][0] == '-') {
      k++;

      if (k >= nbParam && commandList[k - 1][1] != 'R') {
        cout << "Missing something at the end of the command.\n";
        return -1;
      }

      char option = commandList[k - 1][1];
      const char *arg = &(commandList[k])[0];
      const char *lastArg = &(commandList[k - 1])[0];
      switch (option) {

      case 'W':
        weightFileTemp = arg;
        weightFileInit = true;
        break;

      case 'q':
        if (CheckInt(arg))
          quant = atoi(arg);
        else
          return -1;

        break;

      case 'N':
        if (CheckInt(arg))
          nbDimlpNets = atoi(arg);
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

      case 'S':
        rootFolderTemp = arg;
        rootFolderInit = true;
        break;

      case 'A':
        attrFileTemp = arg;
        attrFileInit = true;
        break;

      case 'L':
        learnFileTemp = arg;
        learnFileInit = true;
        break;

      case 'T':
        testFileTemp = arg;
        testFileInit = true;
        break;

      case 'r':
        consoleFileTemp = arg;
        consoleFileInit = true;
        break;

      case 'p':
        predTrainFileTemp = arg;
        break;

      case 't':
        predTestFileTemp = arg;
        break;

      case 'o':
        accuracyFileTemp = arg;
        accuracyFileInit = true;
        break;

      case '1':
        learnTarTemp = arg;
        learnTarInit = true;
        break;

      case '2':
        testTarTemp = arg;
        testTarInit = true;
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
  const char *predTrainFile = nullptr;
  const char *predTestFile = nullptr;
  const char *rulesFile = nullptr;
  const char *consoleFile = nullptr;
  const char *accuracyFile = nullptr;
  const char *learnTar = nullptr;
  const char *testTar = nullptr;
  const char *validTar = nullptr;
  const char *attrFile = nullptr;
  const char *weightFileSave = nullptr;

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

  predTrainFileTemp = root + predTrainFileTemp;
  predTrainFile = &predTrainFileTemp[0];

  predTestFileTemp = root + predTestFileTemp;
  predTestFile = &predTestFileTemp[0];

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

  weightFileSaveTemp = root + weightFileSaveTemp;
  weightFileSave = &weightFileSaveTemp[0];

  // ----------------------------------------------------------------------

  // Get console results to file
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf
  if (consoleFileInit != false) {
    ofs.open(consoleFile);
    std::cout.rdbuf(ofs.rdbuf()); // redirect std::cout to file
  }

  // ----------------------------------------------------------------------

  if (quant == 0) {
    cout << "The number of quantized levels must be greater than 0.\n";
    return -1;
  }

  if (nbIn == 0) {
    cout << "The number of input neurons must be given with option -I.\n";
    return -1;
  }

  if (nbOut == 0) {
    cout << "The number of output neurons must be given with option -O.\n";
    return -1;
  }

  if (weightFileInit == false) {
    cout << "Give a file of weights with -W selection please."
         << "\n";
    return -1;
  }

  if (nbDimlpNets == 0) {
    cout << "Give the number of networks with -W selection please."
         << "\n";
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

  auto net = std::make_shared<BagDimlp>(quant, nbLayers, vecNbNeurons, nbDimlpNets, weightFileSave);

  net->DefNetsWithWeights(weightFile);

  float acc;
  float accTest;

  net->ComputeAcc(Train, TrainClass, &acc, 1, predTrainFile);
  cout << "\n\n*** GLOBAL ACCURACY ON TRAINING SET = " << acc << "\n\n";

  if (Test.GetNbEx() != 0) {
    net->ComputeAcc(Test, TestClass, &accTest, 1, predTestFile);
    cout << "*** GLOBAL ACCURACY ON TESTING SET = " << accTest << "\n";
  }

  // Output accuracy stats in file
  if (accuracyFileInit != false) {
    ofstream accFile(accuracyFile);
    if (accFile.is_open()) {
      accFile << "Global accuracy on training set = " << acc << "\n";
      if (Test.GetNbEx() != 0) {
        accFile << "Global accuracy on testing set = " << accTest;
      }
      accFile.close();
    } else {
      cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
      return -1;
    }
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

    if (Valid.GetNbEx() > 0) {
      DataSet all2(All, Valid);
      All = all2;
    }

    cout << "\n\n****************************************************\n\n";
    cout << "*** RULE EXTRACTION\n";

    std::shared_ptr<VirtualHyp> globVirt = net->MakeGlobalVirt(quant, nbIn,
                                                               vecNbNeurons[1] / nbIn);

    RealHyp ryp(globVirt, nbDimlpNets, net->GetGlobalOut(), nbOut,
                All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                nbWeightLayers);

    if (rulesFileInit != false) {
      filebuf buf;

      if (buf.open(rulesFile, ios_base::out) == nullptr) {
        string errorMsg = "Cannot open file for writing";
        WriteError(errorMsg, rulesFile);
      }

      ostream rulesFileost(&buf);
      ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                         Test, TestClass, Attr, rulesFileost);

      if (ryp.TreeAborted()) {

        std::shared_ptr<VirtualHyp> globVirt2 = net->MakeGlobalVirt(quant, nbIn,
                                                                    vecNbNeurons[1] / nbIn);

        RealHyp2 ryp2(globVirt2, nbDimlpNets, net->GetGlobalOut(), nbOut,
                      All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                      nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, rulesFileost);
      }

      cout << "\n\n"
           << rulesFile << ": "
           << "Written.\n\n";
    } else {
      ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                         Test, TestClass, Attr, cout);

      if (ryp.TreeAborted()) {

        std::shared_ptr<VirtualHyp> globVirt3 = net->MakeGlobalVirt(quant, nbIn,
                                                                    vecNbNeurons[1] / nbIn);

        RealHyp2 ryp2(globVirt3, nbDimlpNets, net->GetGlobalOut(), nbOut,
                      All, net, quant, nbIn, vecNbNeurons[1] / nbIn,
                      nbWeightLayers);

        ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                            Test, TestClass, Attr, cout);
      }
    }
  }

  std::cout.rdbuf(cout_buff); // reset to standard output again

  BpNN::resetInitRandomGen();

  return 0;
}
// Exemple to launch the code : densCls("DensCls -L datanormTrain -1 dataclass2Train -T datanormTest -2 dataclass2Test -I 16 -H2 5 -O 2 -N 2 -W dimlpDatanormBT -R -F dimlpDatanormDensClsRul.rls -p dimlpDatanormDensClsTrain.out -t dimlpDatanormDensClsTest.out -o dimlpDatanormDensClsStats -r dimlpDatanormDensClsResult.txt -S dimlp/datafiles");
