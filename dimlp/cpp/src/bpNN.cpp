#include "bpNN.h"

const int LLL = 1;
const int LD3 = 1;
const int LD4 = 1;

///////////////////////////////////////////////////////////////////

int BpNN::Max(const std::vector<float> &vec) const

{
  float max = vec[0];
  int indMax = 0;

  for (int k = 1; k < vec.size(); k++) {
    if (vec[k] > max) {
      max = vec[k];
      indMax = k;
    }
  }

  return indMax;
}

///////////////////////////////////////////////////////////////////

int BpNN::initRandomGen = 0;

void BpNN::resetInitRandomGen() {
  initRandomGen = 0;
}

void BpNN::InitRandomGen(int seed) const {

  if (initRandomGen == 0) {

    IntRandomFunction irf(seed);
    FloatRandomFunction frf(seed);

    initRandomGen++;

    std::cout << "\nRandom number generator initialized.\n"
              << std::endl;
  }
}

///////////////////////////////////////////////////////////////////

void BpNN::CreateNetStruct(std::vector<int> nbNeurons)

{
  int l;

  NbNeurons.resize(NbLayers);

  for (l = 0; l < NbLayers; l++)
    NbNeurons[l] = nbNeurons[l];

  VecLayer.resize(NbWeightLayers);

  for (l = 1; l < NbWeightLayers; l++)

    VecLayer[l] = std::make_shared<Layer>(Eta, Mu, Flat,
                                          NbNeurons[l], NbNeurons[l + 1],
                                          NbNeurons[l] * NbNeurons[l + 1],
                                          NbNeurons[l] + 1);

  for (l = 2; l < NbWeightLayers; l++) {
    VecLayer[l]->SetDown(VecLayer[l - 1]->GetUp());
    VecLayer[l]->SetDeltaDown(VecLayer[l - 1]->GetDeltaUp());
  }
}

///////////////////////////////////////////////////////////////////

void BpNN::WriteArchParam() const

{
  int l;

  std::cout << "\n\nArchitecture: ";

  for (l = 0; l < NbLayers - 1; l++)
    std::cout << NbNeurons[l] << "-";
  std::cout << NbNeurons[l] << "\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::AssignParam(
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::string &saveFile)

{
  Eta = eta;
  Mu = mu;
  Flat = flat;
  ErrParam = errParam;
  AccuracyParam = accuracyParam;
  DeltaErrParam = deltaErrParam;
  ShowErrParam = showErrParam;
  NbEpochsParam = nbEpochsParam;
  NbLayers = nbLayers;
  SaveFile = saveFile;

  NbWeightLayers = nbLayers - 1;
}

///////////////////////////////////////////////////////////////////

void BpNN::WriteParam() const {
  std::cout << "Parameters:\n"
            << std::endl;

  std::cout << "Eta                   = " << Eta << "" << std::endl;
  std::cout << "Mu                    = " << Mu << "" << std::endl;
  std::cout << "Flat                  = " << Flat << "" << std::endl;

  if (ErrParam >= 0)
    std::cout << "Error Threshold       = " << ErrParam << "" << std::endl;

  if (AccuracyParam <= 1)
    std::cout << "Accuracy Threshold    = " << AccuracyParam << "" << std::endl;

  if (DeltaErrParam != 0)
    std::cout << "Delta Error Threshold = " << DeltaErrParam << "" << std::endl;

  std::cout << "Show Error            = " << ShowErrParam << "" << std::endl;

  if (NbEpochsParam < 1000000000)
    std::cout << "Epochs                = " << NbEpochsParam << "" << std::endl;

  std::cout << "" << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::SaveWeights() const

{
  std::filebuf buf;

  if (buf.open(SaveFile, std::ios_base::out) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open save file " + std::string(SaveFile));
  }

  std::ostream outFile(&buf);

  std::cout << "\n\n"
            << SaveFile << ": "
            << "Writing ..." << std::endl;

  for (int n = 0; n < NbWeightLayers; n++)
    VecLayer[n]->WriteWeights(outFile);

  std::cout << SaveFile << ": "
            << "Written.\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::SaveWeights(const std::string &strSave, int netId) const

{
  std::filebuf buf;

  if (buf.open(strSave, std::ios_base::out | std::ios_base::app) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open save file " + strSave);
  }
  std::ostream outFile(&buf);

  std::cout << "\n\n"
            << strSave << ": "
            << "Writing net " << netId << "..." << std::endl;

  outFile << "Network " << netId << " " << std::endl
          << std::endl;
  for (int n = 0; n < NbWeightLayers; n++)
    VecLayer[n]->WriteWeights(outFile);

  outFile << std::endl;

  std::cout << strSave << ": "
            << "Written network " << netId << ".\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::ReadWeights() const

{
  std::filebuf buf;

  if (buf.open(ReadFile, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open input file " + ReadFile);
  }

  std::istream inFile(&buf);

  std::string line;
  bool isCurrentNetwork = false;
  bool containsNetworkMarkers = false;

  while (std::getline(inFile, line)) {
    if (line.find("Network ") != std::string::npos) {
      containsNetworkMarkers = true; // Network markers found
      break;
    }
  }
  inFile.clear();
  inFile.seekg(0, std::ios::beg); // Reinitialise cursor to the beginning

  // If the file contains network markers, look for the specific network ID.
  if (containsNetworkMarkers) {
    std::cout << "\n\n"
              << ReadFile << " network " << NetId << ": "
              << "Reading ..." << std::endl;
    int n = 0;
    while (std::getline(inFile, line)) {
      if (line.find("Network " + std::to_string(NetId)) != std::string::npos) {
        isCurrentNetwork = true; // Start of the desired network block
        continue;
      }
      if (isCurrentNetwork && line.find("Network ") != std::string::npos) {
        break; // Start of the next network block, stop reading
      }

      // When in the desired network block
      if (isCurrentNetwork && !checkStringEmpty(line) && n < NbLayers - 1) {
        std::string weightsData = line + "\n";
        if (std::getline(inFile, line)) { // Assuming the next line of weights immediately follows
          weightsData += line;            // Merge two lines for weights
          std::istringstream iss(weightsData);
          VecLayer[n]->ReadWeights(iss);
          n += 1;
        }
      }
    }
  } else { // The file contains just one network
    std::cout << "\n\n"
              << ReadFile << ": "
              << "Reading ..." << std::endl;
    for (int m = 0; m < NbLayers - 1; m++)
      VecLayer[m]->ReadWeights(inFile);
  }

  std::cout << ReadFile << ": "
            << "Read.\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::Push() const

{
  for (int l = 0; l < NbWeightLayers; l++)
    VecLayer[l]->PushWeights();
}

///////////////////////////////////////////////////////////////////

void BpNN::Pop() const

{
  for (int l = 0; l < NbWeightLayers; l++)
    VecLayer[l]->PopWeights();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1(DataSet &data, int index)

{
  VecLayer[0]->SetDown(data.GetExample(index));
  for (int l = 0; l < NbWeightLayers; l++) {
    VecLayer[l]->ForwAndTransf1();
  }
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1(float *ex)

{

  VecLayer[0]->SetDown(ex);

  for (int l = 0; l < NbWeightLayers; l++)
    VecLayer[l]->ForwAndTransf1();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample1()

{
  for (int l = 0; l < NbWeightLayers; l++)
    VecLayer[l]->ForwAndTransf1();
}

///////////////////////////////////////////////////////////////////

void BpNN::ForwardOneExample2(DataSet &data, int index) const

{

  VecLayer[0]->SetDown(data.GetExample(index));

  for (int l = 0; l < NbWeightLayers; l++)
    VecLayer[l]->ForwAndTransf2();
}

///////////////////////////////////////////////////////////////////

float BpNN::ComputeErrorSameAct(
    DataSet &data,
    DataSet &target,
    float *accuracy) const

{
  int p;
  int ansNet;
  int ansTar;
  int good;
  int bad;
  float sum;
  std::vector<float> ptrOut;
  std::vector<float> ptrTar;
  const int nbPat = data.GetNbEx();
  const int nbOut = target.GetNbAttr();

  for (p = 0, sum = 0.0, good = 0, bad = 0; p < nbPat; p++) {
    ForwardOneExample2(data, p);

    ptrOut.assign(VecLayer[NbWeightLayers - 1]->GetUp(), VecLayer[NbWeightLayers - 1]->GetUp() + nbOut);
    ptrTar.assign(target.GetExample(p), target.GetExample(p) + nbOut);

    ansNet = Max(ptrOut);
    ansTar = Max(ptrTar);

    sum += VecLayer[NbWeightLayers - 1]->HalfErrFunct(nbOut, ptrOut, ptrTar);

    if (ansNet == ansTar)
      good++;
    else
      bad++;
  }

  *accuracy = static_cast<float>(good) + static_cast<float>(bad);
  *accuracy = (float)good / *accuracy;

  return sum;
}

///////////////////////////////////////////////////////////////////

void BpNN::BackOneExample(DataSet &target, int index) const

{

  VecLayer[NbWeightLayers - 1]->ComputeDeltaOut(target.GetExample(index));

  for (int l = NbWeightLayers - 1; l > 0; l--)
    VecLayer[l]->BackLayer();

  VecLayer[0]->BackLayerWithout();
}

///////////////////////////////////////////////////////////////////

void BpNN::TrainOneEpoch(DataSet &data, DataSet &target, const IntRandomFunction *r) const

{
  int indPat;

  const int nbPat = data.GetNbEx();

  for (int p = 0; p < nbPat; p++) {
    indPat = r->RandomInteger();

    ForwardOneExample2(data, indPat);
    BackOneExample(target, indPat);
  }
}

///////////////////////////////////////////////////////////////////

void BpNN::PrintSpecErrDimlp(float specErr, float specAcc) const {
  std::ostringstream oss;
  oss << "    (SSE = " << std::setprecision(12) << specErr << "    ACC = " << std::setprecision(8) << specAcc << ")";
  std::string temp = oss.str();
  std::cout << temp << std::endl;
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSmlp()

{
  const int nbWeights = NbNeurons[0] * NbNeurons[1];
  const int nbWeightsForInit = NbNeurons[0] + 1;

  VecLayer[0] = std::make_shared<Layer>(Eta, Mu, Flat,
                                        NbNeurons[0], NbNeurons[1],
                                        nbWeights, nbWeightsForInit);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineDimlp(int discrLevels)

{
  const int nbWeights = NbNeurons[1];
  const int nbWeightsForInit = 2;

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             nbWeights, nbWeightsForInit,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineQmlp(int discrLevels)

{
  const int nbWeights = NbNeurons[1];
  const int nbWeightsForInit = 2;

  VecLayer[0] = std::make_shared<LayerDimlp4>(Eta, Mu, Flat,
                                              NbNeurons[0], NbNeurons[1],
                                              nbWeights, nbWeightsForInit,
                                              discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineFdimlp(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerDimlp3>(Eta, Mu, Flat,
                                              NbNeurons[1], NbNeurons[2],
                                              NbNeurons[2], 2,
                                              discrLevels);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerFdimlp>(Eta, Mu, Flat,
                                              NbNeurons[0], NbNeurons[1],
                                              NbNeurons[1], 2);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineFdimlp2(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerDimlp2>(Eta, Mu, Flat,
                                              NbNeurons[1], NbNeurons[2],
                                              NbNeurons[2], 2,
                                              discrLevels);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerFdimlp>(Eta, Mu, Flat,
                                              NbNeurons[0], NbNeurons[1],
                                              NbNeurons[1], 2);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSD(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerSD>(NbNeurons[1], NbNeurons[2],
                                          NbNeurons[1] * NbNeurons[2],
                                          NbNeurons[1] + 1);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             NbNeurons[1], 2,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP5(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerSP5>(NbNeurons[1], NbNeurons[2],
                                           NbNeurons[1] * NbNeurons[2],
                                           NbNeurons[1] + 1);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             NbNeurons[1], 2,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP3(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerSP3>(NbNeurons[1], NbNeurons[2],
                                           NbNeurons[1] * NbNeurons[2],
                                           NbNeurons[1] + 1);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             NbNeurons[1], 2,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSR(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerRad>(NbNeurons[1], NbNeurons[2],
                                           NbNeurons[1] * NbNeurons[2],
                                           NbNeurons[1] + 1);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             NbNeurons[1], 2,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

void BpNN::DefineSP4(int discrLevels)

{
  VecLayer[1] = std::make_shared<LayerSP4>(NbNeurons[1], NbNeurons[2],
                                           NbNeurons[1] * NbNeurons[2],
                                           NbNeurons[1] + 1);

  VecLayer[2]->SetDown(VecLayer[1]->GetUp());
  VecLayer[2]->SetDeltaDown(VecLayer[1]->GetDeltaUp());

  VecLayer[0] = std::make_shared<LayerDimlp>(Eta, Mu, Flat,
                                             NbNeurons[0], NbNeurons[1],
                                             NbNeurons[1], 2,
                                             discrLevels);

  VecLayer[1]->SetDown(VecLayer[0]->GetUp());
  VecLayer[1]->SetDeltaDown(VecLayer[0]->GetDeltaUp());
}

///////////////////////////////////////////////////////////////////

float BpNN::ComputeError(
    DataSet &data,
    DataSet &target,
    float *accuracy)

{
  int p;
  int ansNet;
  int ansTar;
  int good;
  int bad;
  float sum;
  std::vector<float> ptrOut;
  std::vector<float> ptrTar;
  const int nbPat = data.GetNbEx();
  const int nbOut = target.GetNbAttr();
  for (p = 0, sum = 0.0, good = 0, bad = 0; p < nbPat; p++) {
    ForwardOneExample1(data, p);
    ptrOut.assign(VecLayer[NbWeightLayers - 1]->GetUp(), VecLayer[NbWeightLayers - 1]->GetUp() + nbOut);
    ptrTar.assign(target.GetExample(p), target.GetExample(p) + nbOut);
    ansNet = Max(ptrOut);
    ansTar = Max(ptrTar);

    sum += VecLayer[NbWeightLayers - 1]->HalfErrFunct(nbOut, ptrOut, ptrTar);

    if (ansNet == ansTar)
      good++;
    else
      bad++;
  }
  *accuracy = static_cast<float>(good) + static_cast<float>(bad);
  *accuracy = (float)good / *accuracy;

  return sum;
}

///////////////////////////////////////////////////////////////////

void BpNN::TrainPhase(
    DataSet &train,
    DataSet &trainTar,
    DataSet &test,
    DataSet &testTar,
    DataSet &valid,
    DataSet &validTar,
    const std::string &accuracyFile,
    bool fromBT)

{
  float oldErr = 0;
  float acc;
  float err;
  float specAcc;
  float specErr;
  float prevValidErr;
  float validErr;
  float accValid;
  float testErr;
  float accTest;

  IntRandomFunction randInt(0, train.GetNbEx() - 1);
  err = ComputeError(train, trainTar, &acc);
  std::ostringstream oss;
  oss << "     0:  SSE = " << std::setprecision(12) << err << "    ACC = " << std::setprecision(8) << acc;
  std::string temp = oss.str();
  std::cout << temp;

  specErr = ComputeErrorSameAct(train, trainTar, &specAcc);
  PrintSpecErr(specErr, specAcc);

  if (valid.GetNbEx() != 0) {
    prevValidErr = ComputeError(valid, validTar, &accValid);

    std::cout << "Validation set: ";
    std::ostringstream ossVal;
    ossVal << " SSE = " << std::setprecision(12) << prevValidErr << "    ACC = " << std::setprecision(8) << accValid << "\n"
           << std::endl;
    temp = ossVal.str();
    std::cout << temp;

    Push();
  }

  if (acc >= AccuracyParam) {
    std::cout << "\n\n*** REACHED ACCURACY THRESHOLD";
    std::cout << " (" << AccuracyParam << ")\n"
              << std::endl;
    return;
  }

  if (err < ErrParam) {
    std::cout << "\n\n*** REACHED ERROR THRESHOLD";
    std::cout << " (" << ErrParam << ")\n"
              << std::endl;
    return;
  }

  if (fabs(oldErr - specErr) < DeltaErrParam) {
    std::cout << "\n\n*** REACHED VARIATION CRITERION THRESHOLD";
    std::cout << " (" << DeltaErrParam << ")\n"
              << std::endl;
    return;
  }

  for (int e = 1; e <= NbEpochsParam; e++) {
    TrainOneEpoch(train, trainTar, &randInt);

    if (e % ShowErrParam == 0) {
      err = ComputeError(train, trainTar, &acc);

      std::ostringstream oss2;
      oss2 << std::setfill(' ') << std::setw(6 - static_cast<int>(std::to_string(e).length())) << "" << e << ":  SSE = " << std::setprecision(12) << err << "    ACC = " << std::setprecision(8) << acc;
      temp = oss2.str();
      std::cout << temp;

      specErr = ComputeErrorSameAct(train, trainTar, &specAcc);
      PrintSpecErr(specErr, specAcc);

      if (valid.GetNbEx() != 0) {
        validErr = ComputeError(valid, validTar, &accValid);

        std::cout << "Validation set: ";
        std::ostringstream ossVal2;
        ossVal2 << " SSE = " << std::setprecision(12) << validErr << "    ACC = " << std::setprecision(8) << accValid << "\n"
                << std::endl;
        temp = ossVal2.str();
        std::cout << temp;

        if (validErr < prevValidErr) {
          std::cout << " (Better validation error, saving weights)";
          Push();
          prevValidErr = validErr;
        }

        std::cout << "\n"
                  << std::endl;
      }

      if (acc >= AccuracyParam) {
        std::cout << "\n\n*** REACHED ACCURACY THRESHOLD";
        std::cout << " (" << AccuracyParam << ")\n"
                  << std::endl;
        break;
      }

      if (err < ErrParam) {
        std::cout << "\n\n*** REACHED ERROR THRESHOLD";
        std::cout << " (" << ErrParam << ")\n"
                  << std::endl;
        break;
      }

      if (fabs(oldErr - specErr) < DeltaErrParam) {
        std::cout << "\n\n*** REACHED VARIATION CRITERION THRESHOLD";
        std::cout << " (" << DeltaErrParam << ")\n"
                  << std::endl;
        break;
      }

      oldErr = specErr;
    }
  }

  err = ComputeError(train, trainTar, &acc);

  std::cout << "\n\n*** SUM SQUARED ERROR ON TRAINING SET = " << err;
  std::cout << "\n\n*** ACCURACY ON TRAINING SET = " << acc << "\n"
            << std::endl;
  if (valid.GetNbEx() != 0) {
    Pop(); // Get the right weights from best validation error
    validErr = ComputeError(valid, validTar, &accValid);

    std::cout << "\n\n*** SUM SQUARED ERROR ON VALIDATION SET = " << validErr;
    std::cout << "\n\n*** ACCURACY ON VALIDATION SET = " << accValid << "" << std::endl;
  }

  if (test.GetNbEx() != 0) {
    testErr = ComputeError(test, testTar, &accTest);

    std::cout << "\n\n*** SUM SQUARED ERROR ON TESTING SET = " << testErr;
    std::cout << "\n\n*** ACCURACY ON TESTING SET = " << accTest << "" << std::endl;
  }
  // Output accuracy stats in file
  if (!accuracyFile.empty()) {
    std::ofstream accFile(accuracyFile, std::ios::app);
    if (accFile.is_open()) {
      accFile << "Sum squared error on training set = " << err << "" << std::endl;
      accFile << "Accuracy on training set = " << acc << "\n"
              << std::endl;
      if (valid.GetNbEx() > 0) {
        accFile << "Sum squared error on validation set = " << validErr << "" << std::endl;
        accFile << "Accuracy on validation set = " << accValid << "\n"
                << std::endl;
      }
      if (test.GetNbEx() > 0) {
        accFile << "Sum squared error on testing set = " << testErr << "" << std::endl;
        accFile << "Accuracy on testing set = " << accTest << "\n"
                << std::endl;
      }
      accFile.close();
    } else {
      throw CannotOpenFileError("Error : Cannot open accuracy file " + accuracyFile);
    }
  }

  if (!fromBT) {
    SaveWeights();
  }
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN(
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const std::string &saveFile,
    const std::string &printNetType,
    int seed)

{

  InitRandomGen(seed);

  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
  std::cout << "Creating " << printNetType << " structures ...";

  AssignParam(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
              showErrParam, nbEpochsParam, nbLayers, saveFile);

  CreateNetStruct(nbNeurons);

  WriteArchParam();
  WriteParam();

  std::cout << printNetType << " network created.\n"
            << std::endl;
  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN(
    const std::string &readFile,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const std::string &printNetType,
    int netId)
    : ReadFile(readFile), NetId(netId), NbLayers(nbLayers), NbWeightLayers(nbLayers - 1) {

  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
  std::cout << "Creating " << printNetType << " structures ...";

  CreateNetStruct(nbNeurons);

  WriteArchParam();

  std::cout << printNetType << " network created.\n"
            << std::endl;
  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////

BpNN::BpNN(
    const std::string &readFile,
    float eta,
    float mu,
    float flat,
    float errParam,
    float accuracyParam,
    float deltaErrParam,
    int showErrParam,
    int nbEpochsParam,
    int nbLayers,
    const std::vector<int> &nbNeurons,
    const std::string &saveFile,
    const std::string &printNetType,
    int seed)
    : ReadFile(readFile) {
  InitRandomGen(seed);

  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
  std::cout << "Creating " << printNetType << " structures ...";

  AssignParam(eta, mu, flat, errParam, accuracyParam, deltaErrParam,
              showErrParam, nbEpochsParam, nbLayers, saveFile);

  CreateNetStruct(nbNeurons);

  WriteArchParam();
  WriteParam();

  std::cout << printNetType << " network created.\n"
            << std::endl;
  std::cout << "\n\n-----------------------------------------";
  std::cout << "-------------------------------------\n"
            << std::endl;
}

///////////////////////////////////////////////////////////////////
