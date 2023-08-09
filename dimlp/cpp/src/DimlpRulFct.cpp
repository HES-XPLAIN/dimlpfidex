#include "DimlpRulFct.h"

using namespace std;

const int BPNN = 1;

////////////////////////////////////////////////////////////

void GiveAllParamDimlpRul()

{
  cout << "\n-------------------------------------------------\n\n";

  cout << "DimlpRul -L <training set file(path with respect to specified root folder)> ";
  cout << "-W <file of weights> ";
  cout << "-I <number of input neurons> -O <number of output neurons>";
  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>\n";
  cout << "-A <file of attributes>\n";
  cout << "-V <validation set file>\n";
  cout << "-T <testing set file>\n";
  cout << "-1 <file of train classes>\n";
  cout << "-2 <file of test classes>\n";
  cout << "-3 <file of validation classes>\n";
  cout << "-R <rulesFile>\n";                              // If you want to specify the rulesFile not being dimlp.rls
  cout << "-r <file where you redirect console result>\n"; // If we want to redirect console result to file
  cout << "-o <output file with train, test and validation accuracy>\n";
  cout << "-H1 <number of neurons in the first hidden layer> ";
  cout << "(if not specified this number will be equal to the ";
  cout << "number of input neurons)\n";
  cout << "-Hk <number of neurons in the kth hidden layer>\n";
  cout << "-q <number of stairs in staircase activation function>\n";

  cout << "\n-------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////

int dimlpRul(const string &command) {

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

  int quant = 50;
  int nbIn = 0;
  int nbOut = 0;

  string learnFileTemp;
  bool learnFileInit = false;
  string testFileTemp;
  bool testFileInit = false;
  string validFileTemp;
  bool validFileInit = false;
  string weightFileTemp;
  bool weightFileInit = false;
  string learnTarTemp;
  bool learnTarInit = false;
  string testTarTemp;
  bool testTarInit = false;
  string validTarTemp;
  bool validTarInit = false;
  string rulesFileTemp = "dimlp.rls";
  string consoleFileTemp;
  bool consoleFileInit = false;
  string accuracyFileTemp;
  bool accuracyFileInit = false;
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
    GiveAllParamDimlpRul();
    return 0;
  }

  int k = 1; // We skip "DimlpRul"
  while (k < nbParam) {
    if (commandList[k][0] == '-') {
      k++;

      if (k >= nbParam) {
        cout << "Missing something at the end of the command.\n";
        return -1;
      }

      char option = commandList[k - 1][1];
      const char *arg = &(commandList[k])[0];
      const char *lastArg = &(commandList[k - 1])[0];
      switch (option) {
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
        rulesFileTemp = arg;
        break;

      case 'r':
        consoleFileTemp = arg;
        consoleFileInit = true;
        break;

      case 'o':
        accuracyFileTemp = arg;
        accuracyFileInit = true;
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
  const char *learnTar = nullptr;
  const char *testTar = nullptr;
  const char *validTar = nullptr;
  const char *rulesFile = nullptr;
  const char *consoleFile = nullptr;
  const char *accuracyFile = nullptr;
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

  rulesFileTemp = root + rulesFileTemp;
  rulesFile = &rulesFileTemp[0];

  if (consoleFileInit) {
    consoleFileTemp = root + consoleFileTemp;
    consoleFile = &consoleFileTemp[0];
  }

  if (accuracyFileInit) {
    accuracyFileTemp = root + accuracyFileTemp;
    accuracyFile = &accuracyFileTemp[0];
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

  if (weightFileInit == false) {
    cout << "Give a file of weights with -W selection please."
         << "\n";
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

  // ----------------------------------------------------------------------

  auto net = std::make_shared<Dimlp>(weightFile, nbLayers, vecNbNeurons, quant);

  float accTrain;
  float errTrain;
  float accValid;
  float errValid;
  float accTest;
  float errTest;

  errTrain = net->Error(Train, TrainClass, &accTrain);

  cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << errTrain;
  cout << "\n\n*** ACCURACY ON TRAINING SET = " << accTrain << "\n";

  if (Valid.GetNbEx() > 0) {
    errValid = net->Error(Valid, ValidClass, &accValid);

    cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << errValid;
    cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "\n";
  }

  if (Test.GetNbEx() > 0) {
    errTest = net->Error(Test, TestClass, &accTest);

    cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << errTest;
    cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "\n";
  }

  // Output accuracy stats in file
  if (accuracyFileInit != false) {
    ofstream accFile(accuracyFile);
    if (accFile.is_open()) {
      accFile << "Sum squared error on training set = " << errTrain << "\n";
      accFile << "Accuracy on training set = " << accTrain << "\n\n";
      if (Valid.GetNbEx() > 0) {
        accFile << "Sum squared error on validation set = " << errValid << "\n";
        accFile << "Accuracy on validation set = " << accValid << "\n\n";
      }
      if (Test.GetNbEx() > 0) {
        accFile << "Sum squared error on testing set = " << errTest << "\n";
        accFile << "Accuracy on testing set = " << accTest << "\n\n";
      }
      accFile.close();
    } else {
      cout << "Error : could not open accuracy file " << accuracyFile << " not found.\n";
      return -1;
    }
  }

  cout << "\n-------------------------------------------------\n\n";

  // ----------------------------------------------------------------------

  All = Train;

  cout << "Extraction Part :: " << endl;

  if (Valid.GetNbEx() > 0) {
    DataSet all2(All, Valid);
    All = all2;
  }

  cout << "\n\n****************************************************\n\n";
  cout << "*** RULE EXTRACTION\n";

  if (attrFileInit != false) {
    AttrName attr(attrFile, nbIn, nbOut);

    if (attr.ReadAttr())
      cout << "\n\n"
           << attrFile << ": Read file of attributes.\n\n";

    Attr = attr;
  }

  RealHyp ryp(All, net, quant, nbIn,
              vecNbNeurons[1] / nbIn, nbWeightLayers);

  filebuf buf;

  if (buf.open(rulesFile, ios_base::out) == nullptr) {
    string errorMsg = "Cannot open file for writing";
    WriteError(errorMsg, rulesFile);
  }

  ostream rulesFileost(&buf);

  ryp.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                     Test, TestClass, Attr, rulesFileost);

  if (ryp.TreeAborted()) {
    RealHyp2 ryp2(All, net, quant, nbIn,
                  vecNbNeurons[1] / nbIn, nbWeightLayers);

    ryp2.RuleExtraction(All, Train, TrainClass, Valid, ValidClass,
                        Test, TestClass, Attr, rulesFileost);
  }

  cout << "\n\n"
       << rulesFile << ": "
       << "Written.\n\n";

  std::cout.rdbuf(cout_buff); // reset to standard output again

  BpNN::resetInitRandomGen();

  return 0;
}

// Exemple to launch the code : dimlp.dimlpRul("DimlpRul -L datanormTrain -1 dataclass2Train -W dimlpDatanorm.wts -T datanormTest -2 dataclass2Test -I 16 -H2 5 -O 2 -R dimlpDatanormRul.rls -o dimlpDatanormRulStats -r dimlpDatanormRulResult.txt -S dimlp/datafiles")
