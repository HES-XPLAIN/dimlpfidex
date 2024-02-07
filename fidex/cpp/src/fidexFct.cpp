#include "fidexFct.h"

using namespace std;

void showFidexParams() {
  std::cout << "\n-------------------------------------------------\n"
            << std::endl;

  std::cout << "Obligatory parameters : \n"
            << std::endl;
  std::cout << "fidex --train_data_file <train data file> --train_pred_file <train prediction file> --train_class_file <train true class file, not mendatory if classes are specified in train data file> ";
  std::cout << "--test_data_file <test sample(s) data file with data, prediction(if no --test_pred_file) and true class(if no --test_class_file)> ";
  std::cout << "--weights_file <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] ";
  std::cout << "--rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] ";
  std::cout << "--rules_outfile <Rule(s) output file> ";
  std::cout << "--nb_attributes <number of attributes> ";
  std::cout << "--nb_classes <number of classes> ";
  std::cout << "<Options>\n"
            << std::endl;

  std::cout << "Options are: \n"
            << std::endl;
  std::cout << "--root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>" << std::endl;
  std::cout << "--nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>" << std::endl;
  std::cout << "--test_pred_file <test prediction file> ";
  std::cout << "--test_class_file <test true class file> If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)" << std::endl;
  std::cout << "--attributes_file <file of attributes>" << std::endl;
  std::cout << "--stats_file <output statistic file>" << std::endl;
  std::cout << "--console_file <file where you redirect console result>" << std::endl; // If we want to redirect console result to file
  std::cout << "--max_iterations <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)>" << std::endl;
  std::cout << "--min_covering <minimum covering number (2 by default)>" << std::endl;
  std::cout << "--covering_strategy <decrement by 1 the min covering number each time the minimal covering size is not reached (False by default)>" << std::endl;
  std::cout << "--max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>" << std::endl;
  std::cout << "--min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)>" << std::endl;
  std::cout << "--dropout_dim <dimension dropout parameter (None by default)>" << std::endl;
  std::cout << "--dropout_hyp <hyperplan dropout parameter (None by default)>" << std::endl;
  std::cout << "--nb_quant_levels <number of stairs in staircase activation function (50 by default)>" << std::endl;
  std::cout << "--decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>" << std::endl;
  std::cout << "--positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)>" << std::endl;
  std::cout << "--normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>" << std::endl;
  std::cout << "--mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>" << std::endl;
  std::cout << "--normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_stats is given)>" << std::endl;
  std::cout << "--seed <seed (0=random, default)>";

  std::cout << "\n-------------------------------------------------\n"
            << std::endl;
}

/**
 * @brief Used to set default hyperparameters values and to check the sanity of all used values like boundaries and logic.
 *
 * @param p is the Parameter class containing all hyperparameters that rule the entire algorithm execution.
 */
void checkFidexParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultInt(NB_DIMLP_NETS, 1);
  p.setDefaultInt(MAX_ITERATIONS, 10);
  p.setDefaultInt(MIN_COVERING, 2);
  p.setDefaultBool(COVERING_STRATEGY, false);
  p.setDefaultFloat(DROPOUT_DIM, 0.0f);
  p.setDefaultFloat(DROPOUT_HYP, 0.0f);
  p.setDefaultInt(MAX_FAILED_ATTEMPTS, 30);
  p.setDefaultInt(NB_QUANT_LEVELS, 50);
  p.setDefaultFloat(DECISION_THRESHOLD, -1.0f);
  p.setDefaultInt(POSITIVE_CLASS_INDEX, -1);
  p.setDefaultFloat(MIN_FIDELITY, 1.0f);
  p.setDefaultFloat(HI_KNOT, 5.0f);
  p.setDefaultInt(SEED, 0);
  if (p.isStringSet(WEIGHTS_FILE)) {
    p.setWeightsFiles(); // must be called to initialize
  }

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(TRAIN_PRED_FILE);
  p.assertStringExists(TEST_DATA_FILE);
  p.assertStringExists(RULES_OUTFILE);

  // verifying logic between parameters, values range and so on...

  if (p.getInt(NB_ATTRIBUTES) < 1) {
    throw CommandArgumentException("Error : Number of attributes must be strictly positive (>=1).");
  }

  if (p.getInt(NB_CLASSES) < 1) {
    throw CommandArgumentException("Error : Number of classes must be strictly positive (>=1).");
  }

  if (p.isStringSet(TEST_CLASS_FILE) && !p.isStringSet(TEST_PRED_FILE)) {
    throw CommandArgumentException("Error : The test prediction data file(--test_pred_file) needs to be specified if the test class data file(--test_class_file) is given.");
  }

  if (p.isStringSet(WEIGHTS_FILE) && p.isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : Do not specify both a weight file and an rules input file. Choose one of them.");
  }

  if (!p.isStringSet(WEIGHTS_FILE) && !p.isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
  }

  if (p.getInt(NB_QUANT_LEVELS) < 1) {
    throw CommandArgumentException("Error : Number of stairs in staircase activation function must be strictly positive (>=1).");
  }

  if (p.getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of networks must be strictly positive (>=1).");
  }

  if (p.getInt(MAX_ITERATIONS) < 1) {
    throw CommandArgumentException("Error : Maximum number of iterations must be strictly positive (>=1).");
  }

  if (p.getInt(MIN_COVERING) < 1) {
    throw CommandArgumentException("Error : Minimium covering must be strictly positive (>=1).");
  }

  if (p.getFloat(DROPOUT_DIM) < 0.0f || p.getFloat(DROPOUT_DIM) > 1.0f) {
    throw CommandArgumentException("Error : Dropout dim must be between [0.0, 1.0].");
  }

  if (p.getFloat(DROPOUT_HYP) < 0.0f || p.getFloat(DROPOUT_HYP) > 1.0f) {
    throw CommandArgumentException("Error : Dropout hyp must be between [0.0, 1.0].");
  }

  if (p.getInt(MAX_FAILED_ATTEMPTS) < 0) {
    throw CommandArgumentException("Error : Maximum number of failed attempts has to be positive (>=0)");
  }

  if (p.getFloat(MIN_FIDELITY) < 0.0f || p.getFloat(MIN_FIDELITY) > 1.0f) {
    throw CommandArgumentException("Error : Minimum fidelity has to be between [0.0, 1.0]");
  }

  if ((p.getFloat(DECISION_THRESHOLD) < 0.0f || p.getFloat(DECISION_THRESHOLD) > 1.0f) && p.getFloat(DECISION_THRESHOLD) != -1.0f) {
    throw CommandArgumentException("Error : Decision threshold must be between [0.0, 1.0].");
  }

  if (p.getInt(POSITIVE_CLASS_INDEX) < 0 && p.getInt(POSITIVE_CLASS_INDEX) != -1) {
    throw CommandArgumentException("Error : Positive class index must be positive (>=0)");
  }

  if (p.getInt(POSITIVE_CLASS_INDEX) >= p.getInt(NB_CLASSES)) {
    throw CommandArgumentException("Error : The index of positive class cannot be greater or equal to the number of classes (" + to_string(p.getInt(NB_CLASSES)) + ").");
  }

  if (p.getFloat(DECISION_THRESHOLD) != -1 && p.getInt(POSITIVE_CLASS_INDEX) == -1) {
    throw CommandArgumentException("Error : The positive class index has to be given with option --positive_class_index if the decision threshold is given (--decision_threshold)");
  }

  if (p.getInt(SEED) < 0) {
    throw CommandArgumentException("Error : Minimium covering must be null or positive (>=0).");
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

int fidex(const string &command) {

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
      showFidexParams();
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
    checkFidexParametersLogicValues(*params);
    cout << *params;

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // ----------------------------------------------------------------------

    // Get parameters values

    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    int nbClasses = params->getInt(NB_CLASSES);
    std::string trainDataFile = params->getString(TRAIN_DATA_FILE);
    std::string trainDataFilePred = params->getString(TRAIN_PRED_FILE);
    std::string mainSamplesDataFile = params->getString(TEST_DATA_FILE);
    std::string ruleFile = params->getString(RULES_OUTFILE);
    float decisionThreshold = params->getFloat(DECISION_THRESHOLD);
    int positiveClassIndex = params->getInt(POSITIVE_CLASS_INDEX);
    int minNbCover = params->getInt(MIN_COVERING);
    int seed = params->getInt(SEED);
    int nbDimlpNets = params->getInt(NB_DIMLP_NETS);
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    float hiKnot = params->getFloat(HI_KNOT);
    float minFidelity = params->getFloat(MIN_FIDELITY);
    int maxIterations = params->getInt(MAX_ITERATIONS);
    float dropoutDim = params->getFloat(DROPOUT_DIM);
    float dropoutHyp = params->getFloat(DROPOUT_HYP);
    bool hasdd = dropoutDim > 0.001;
    bool hasdh = dropoutHyp > 0.001;
    bool minCoverStrategy = params->getBool(COVERING_STRATEGY);
    int maxFailedAttempts = params->getInt(MAX_FAILED_ATTEMPTS);

    string weightsFile;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFile = params->getString(WEIGHTS_FILE);
    }

    string inputRulesFile;
    if (params->isStringSet(RULES_FILE)) {
      inputRulesFile = params->getString(RULES_FILE);
    }

    vector<string> weightsFiles;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFiles = params->getWeightsFiles();
    }

    // ----------------------------------------------------------------------

    // Import files

    std::cout << "Import files..." << std::endl;

    float importTime;
    clock_t impt1;
    clock_t impt2;

    impt1 = clock();

    std::unique_ptr<DataSetFid> trainDatas;
    if (!params->isStringSet(TRAIN_CLASS_FILE)) {
      trainDatas.reset(new DataSetFid("trainDatas from Fidex", trainDataFile, trainDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      if (!trainDatas->getHasClasses()) {
        throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
      }
    } else {
      trainDatas.reset(new DataSetFid("trainDatas from Fidex", trainDataFile, trainDataFilePred, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex, params->getString(TRAIN_CLASS_FILE)));
    }

    vector<vector<double>> &trainData = trainDatas->getDatas();
    vector<int> &trainPreds = trainDatas->getPredictions();
    vector<vector<double>> &trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
    vector<int> &trainTrueClass = trainDatas->getClasses();

    int nbTrainSamples = trainDatas->getNbSamples();

    if (minNbCover > nbTrainSamples) {
      throw CommandArgumentException("Error : invalide type for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
    }

    // Get test data
    vector<int> mainSamplesPreds;
    vector<int> mainSamplesTrueClass;
    vector<vector<double>> mainSamplesValues;
    vector<vector<double>> mainSamplesOutputValuesPredictions;
    std::unique_ptr<DataSetFid> testDatas;
    bool hasTrueClasses;
    if (!params->isStringSet(TEST_PRED_FILE)) { // If we have only one test data file with data, pred and class
      testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      mainSamplesValues = testDatas->getDatas();
      mainSamplesPreds = testDatas->getPredictions();
      mainSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();
      hasTrueClasses = testDatas->getHasClasses();
      if (hasTrueClasses) {
        mainSamplesTrueClass = testDatas->getClasses();
      }

    } else { // We have different files for test predictions and test classes

      if (params->isStringSet(TEST_CLASS_FILE)) {
        testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex, params->getString(TEST_CLASS_FILE)));
      } else {
        testDatas.reset(new DataSetFid("testDatas from Fidex", mainSamplesDataFile, params->getString(TEST_PRED_FILE), nbAttributes, nbClasses, decisionThreshold, positiveClassIndex));
      }
      mainSamplesValues = testDatas->getDatas();
      mainSamplesPreds = testDatas->getPredictions();
      mainSamplesOutputValuesPredictions = testDatas->getOutputValuesPredictions();

      // Classes :
      if (params->isStringSet(TEST_CLASS_FILE)) {
        hasTrueClasses = true;
        mainSamplesTrueClass = testDatas->getClasses();
      } else {
        hasTrueClasses = false;
      }
    }

    int nbTestSamples = testDatas->getNbSamples();

    // Get attributes
    vector<string> attributeNames;
    vector<string> classNames;
    bool hasClassNames = false;
    if (params->isStringSet(ATTRIBUTES_FILE)) {
      testDatas->setAttributes(params->getString(ATTRIBUTES_FILE), nbAttributes, nbClasses);
      attributeNames = testDatas->getAttributeNames();
      hasClassNames = testDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = testDatas->getClassNames();
      }
    }

    impt2 = clock();
    importTime = (float)(impt2 - impt1) / CLOCKS_PER_SEC;

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

    std::cout << "\nImport time = " << importTime << " sec" << std::endl;

    std::cout << "Files imported" << endl
              << endl;
    std::cout << "----------------------------------------------" << endl
              << endl;

    float temps2;
    clock_t d1;
    clock_t d2;

    d1 = clock();

    // Initialize random number generator

    if (seed == 0) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      auto seedValue = currentTime.time_since_epoch().count();
      seed = static_cast<unsigned int>(seedValue);
    }
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    vector<string> lines;
    // compute hyperspace
    std::cout << "Creation of hyperspace..." << std::endl;

    std::vector<std::vector<double>> matHypLocus;

    if (params->isStringSet(WEIGHTS_FILE)) {
      if (nbDimlpNets > 1) {
        std::cout << "\nParameters of hyperLocus :\n"
                  << std::endl;
        std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
        std::cout << "- Interval : [-" << hiKnot << "," << hiKnot << "]" << std::endl
                  << std::endl;
        std::cout << "Computation of all hyperlocus" << std::endl;
      }
      for (string wf : weightsFiles) {
        std::vector<std::vector<double>> hypLocus;
        if (nbDimlpNets > 1) {
          hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot, false); // Get hyperlocus
        } else {
          hypLocus = calcHypLocus(wf, nbQuantLevels, hiKnot); // Get hyperlocus
        }

        matHypLocus.insert(matHypLocus.end(), hypLocus.begin(), hypLocus.end()); // Concatenate hypLocus to matHypLocus
      }
      if (nbDimlpNets > 1) {
        std::cout << "All hyperlocus created" << std::endl;
      }
    } else {
      if (params->isStringSet(ATTRIBUTES_FILE)) {
        matHypLocus = calcHypLocus(inputRulesFile, nbAttributes, attributeNames);
      } else {
        matHypLocus = calcHypLocus(inputRulesFile, nbAttributes);
      }
    }

    FidexNameSpace::Hyperspace hyperspace(matHypLocus); // Initialize hyperbox and get hyperplans

    const size_t nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributes != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributes) + ".");
    }

    std::cout << "Hyperspace created" << endl
              << endl;

    // Stats
    double meanFidelity = 0;
    double meanConfidence = 0;
    double meanCovSize = 0;
    double meanNbAntecedentsPerRule = 0;
    double meanAccuracy = 0;

    int nbIt;
    // We compute rule for each test sample
    for (int currentSample = 0; currentSample < nbTestSamples; currentSample++) {
      int counterFailed = 0; // Number of times we failed to find a rule with maximal fidexlity when minNbCover is 1
      int currentMinNbCover = minNbCover;
      lines.push_back("Rule for sample " + std::to_string(currentSample) + " :\n");

      do {
        hyperspace.getHyperbox()->resetDiscriminativeHyperplans(); // Reinitialize discriminativeHyperplans for next sample

        if (nbTestSamples > 1 && currentMinNbCover == minNbCover) {
          std::cout << "Computation of rule for sample " << currentSample << " : " << endl
                    << endl;
        }

        if (nbTestSamples == 1 && currentMinNbCover == minNbCover) {
          std::cout << "Searching for discriminating hyperplans..." << std::endl;
        }

        // Compute initial covering
        vector<int> coveredSamples(nbTrainSamples);                         // Samples covered by the hyperbox
        std::iota(std::begin(coveredSamples), std::end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

        // Store covering and compute initial fidelty
        hyperspace.getHyperbox()->setCoveredSamples(coveredSamples);
        hyperspace.getHyperbox()->computeFidelity(mainSamplesPreds[currentSample], trainPreds); // Compute fidelity of initial hyperbox

        if (nbTestSamples == 1 && currentMinNbCover == minNbCover) {
          std::cout << "Initial fidelity : " << hyperspace.getHyperbox()->getFidelity() << endl;
        }

        nbIt = 0;

        while (hyperspace.getHyperbox()->getFidelity() < minFidelity && nbIt < maxIterations) { // While fidelity of our hyperbox is not high enough
          // std::cout << "New iteration : " << nbIt << std::endl;
          std::unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());    // best hyperbox to choose for next step
          std::unique_ptr<Hyperbox> currentHyperbox(new Hyperbox()); // best hyperbox to choose for next step
          double mainSampleValue;
          int attribut;
          int dimension;
          int indexBestHyp = -1;
          int bestDimension = -1;
          int minHyp = -1; // Index of first hyperplan without any change of the best hyperplan
          int maxHyp = -1;
          // Randomize dimensions
          vector<int> dimensions(nbIn);
          std::iota(std::begin(dimensions), std::end(dimensions), 0); // Vector from 0 to nbIn-1
          std::shuffle(std::begin(dimensions), std::end(dimensions), gen);
          vector<int> currentCovSamp;

          for (int d = 0; d < nbIn; d++) { // Loop on all dimensions
            if (bestHyperbox->getFidelity() >= minFidelity) {
              break;
            }
            dimension = dimensions[d];
            attribut = dimension % nbAttributes;
            mainSampleValue = mainSamplesValues[currentSample][attribut];
            // Test if we dropout this dimension

            if (hasdd && dis(gen) < dropoutDim) {
              continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
            }

            bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

            size_t nbHyp = hyperspace.getHyperLocus()[dimension].size();
            if (nbHyp == 0) {
              continue; // No data on this dimension
            }
            // std::cout << std::endl;
            // std::cout << "NEW DIM" << d << std::endl;
            for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)
              // std::cout << "\rNEW hyp: " << k+1 << "/" << nbHyp << std::flush;
              //  Test if we dropout this hyperplan
              if (hasdh && dis(gen) < dropoutHyp) {
                continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
              }

              double hypValue = hyperspace.getHyperLocus()[dimension][k];
              bool mainSampleGreater = hypValue <= mainSampleValue;

              // Check if main sample value is on the right of the hyperplan
              currentHyperbox->computeCoveredSamples(hyperspace.getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
              currentHyperbox->computeFidelity(mainSamplesPreds[currentSample], trainPreds);
              // Compute fidelity
              // If the fidelity is better or is same with better covering but not if covering size is lower than minNbCover
              if (currentHyperbox->getCoveredSamples().size() >= currentMinNbCover && (currentHyperbox->getFidelity() > bestHyperbox->getFidelity() || (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() > bestHyperbox->getCoveredSamples().size()))) {
                bestHyperbox->setFidelity(currentHyperbox->getFidelity()); // Update best hyperbox
                bestHyperbox->setCoveredSamples(currentHyperbox->getCoveredSamples());
                indexBestHyp = k;
                minHyp = k; // New best
                maxHyp = -1;
                maxHypBlocked = false; // We can increase maxHyp if next is the same
                bestDimension = dimension;
              } else if (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() == bestHyperbox->getCoveredSamples().size()) {
                if (!maxHypBlocked) {
                  maxHyp = k; // Index of last (for now) hyperplan which is equal to the best.
                }

              } else {
                maxHypBlocked = true; // we can't increase maxHyp anymore for this best hyperplan
              }

              if (bestHyperbox->getFidelity() >= minFidelity) {
                break;
              }
            }
          }

          // Modication of our hyperbox with the best at this iteration and modify discriminative hyperplans
          if (indexBestHyp != -1 && bestDimension != -1) { // If we found any good dimension with good hyperplan (with enough covering)
            if (maxHyp != -1) {
              indexBestHyp = (maxHyp + minHyp) / 2;
            }
            // Rule is not added if fidelity and covering size did not increase
            if (bestHyperbox->getFidelity() > hyperspace.getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == hyperspace.getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > hyperspace.getHyperbox()->getCoveredSamples().size())) {
              // std::cout << "Found new best" << std::endl;
              // std::cout << "Fidelity : " << bestHyperbox->getFidelity() << std::endl;
              // std::cout << "Cov size :" << bestHyperbox->getCoveredSamples().size() << endl;
              hyperspace.getHyperbox()->setFidelity(bestHyperbox->getFidelity());
              hyperspace.getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
              hyperspace.getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
            }
          }
          nbIt += 1;
        }

        meanFidelity += hyperspace.getHyperbox()->getFidelity();
        std::cout << "Final fidelity : " << hyperspace.getHyperbox()->getFidelity() << endl;
        if (hyperspace.getHyperbox()->getFidelity() < minFidelity) {
          if (minCoverStrategy) {
            if (currentMinNbCover >= 2) {
              currentMinNbCover -= 1; // If we didnt found a rule with desired covering, we check with a lower covering
              std::cout << "Fidelity is too low. Restarting fidex with a minimum covering of " << currentMinNbCover << std::endl;
            } else {
              counterFailed += 1;
              std::cout << "Fidelity is too low. Restarting fidex with a minimum covering of " << currentMinNbCover << std::endl;
            }
            if (counterFailed >= maxFailedAttempts) {
              std::cout << "WARNING Fidelity is too low after trying " << std::to_string(maxFailedAttempts) << "times with a minimum covering of 1! You may want to try again." << std::endl;
              if (hasdd || hasdh) {
                std::cout << "Try to not use dropout." << std::endl;
              }
            }
          } else {
            std::cout << "WARNING Fidelity is too low! You may want to try again." << std::endl;
            std::cout << "If you can't find a rule with the wanted fidelity, try a lowest minimal covering or a lower fidelity" << std::endl;
            std::cout << "You can also try to use the min cover strategy (--covering_strategy)" << std::endl;
            std::cout << "If this is not enough, put the min covering to 1 and do not use dropout." << std::endl;
          }
        } else {
          std::cout << endl;
        }

      } while (hyperspace.getHyperbox()->getFidelity() < minFidelity && minCoverStrategy && counterFailed < maxFailedAttempts); // Restart Fidex if we use the strategy and fidelity is not high enough

      if (nbTestSamples == 1) {
        std::cout << "Discriminating hyperplans generated." << endl
                  << endl;
      }
      // Rule extraction
      if (nbTestSamples == 1) {
        std::cout << "Rule extraction" << endl
                  << endl;
      }

      // Compute rule accuracy and confidence
      double ruleAccuracy;
      if (hasTrueClasses) {
        bool mainSampleCorrect = mainSamplesPreds[currentSample] == mainSamplesTrueClass[currentSample];
        ruleAccuracy = hyperspace.computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses, mainSampleCorrect); // Percentage of correct model prediction on samples covered by the rule
      } else {
        ruleAccuracy = hyperspace.computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses); // Percentage of correct model prediction on samples covered by the rule
      }

      meanAccuracy += ruleAccuracy;
      std::cout << "Rule accuracy : " << ruleAccuracy << endl
                << endl;

      int currentSamplePred = mainSamplesPreds[currentSample];
      double currentSamplePredValue = mainSamplesOutputValuesPredictions[currentSample][currentSamplePred];
      double ruleConfidence;
      ruleConfidence = hyperspace.computeRuleConfidence(trainOutputValuesPredictions, currentSamplePred, currentSamplePredValue); // Mean output value of prediction of class chosen by the rule for the covered samples
      meanConfidence += ruleConfidence;
      std::cout << "Rule confidence : " << ruleConfidence << endl
                << endl;

      meanCovSize += static_cast<double>(hyperspace.getHyperbox()->getCoveredSamples().size());
      meanNbAntecedentsPerRule += static_cast<double>(hyperspace.getHyperbox()->getDiscriminativeHyperplans().size());
      // Extract rules
      if (params->isDoubleVectorSet(MUS)) {
        hyperspace.ruleExtraction(mainSamplesValues[currentSample], currentSamplePred, ruleAccuracy, ruleConfidence, lines, params->isStringSet(ATTRIBUTES_FILE), attributeNames, hasClassNames, classNames, mus, sigmas, normalizationIndices);
      } else {
        hyperspace.ruleExtraction(mainSamplesValues[currentSample], currentSamplePred, ruleAccuracy, ruleConfidence, lines, params->isStringSet(ATTRIBUTES_FILE), attributeNames, hasClassNames, classNames);
      }

      if (hyperspace.getHyperbox()->getCoveredSamples().size() < minNbCover) {
        std::cout << "The minimum covering of " << minNbCover << " is not achieved." << std::endl;
      }
      std::cout << "Result found after " << nbIt << " iterations." << std::endl;

      std::cout << "-------------------------------------------------" << std::endl;

      if (nbTestSamples > 1) {
        lines.emplace_back("-------------------------------------------------\n");
      }
    }

    // Stats
    meanFidelity /= static_cast<double>(nbTestSamples);
    meanConfidence /= static_cast<double>(nbTestSamples);
    meanCovSize /= static_cast<double>(nbTestSamples);
    meanNbAntecedentsPerRule /= static_cast<double>(nbTestSamples);
    meanAccuracy /= static_cast<double>(nbTestSamples);

    if (params->isStringSet(STATS_FILE)) {
      ofstream outputStatsFile(params->getString(STATS_FILE));
      if (outputStatsFile.is_open()) {
        outputStatsFile << "Statistics with a test set of " << nbTestSamples << " samples :\n"
                        << std::endl;
        outputStatsFile << "The mean covering size per rule is : " << meanCovSize << "" << std::endl;
        outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntecedentsPerRule << "" << std::endl;
        outputStatsFile << "The mean rule fidelity rate is : " << meanFidelity << "" << std::endl;
        outputStatsFile << "The mean rule accuracy is : " << meanAccuracy << "" << std::endl;
        outputStatsFile << "The mean rule confidence is : " << meanConfidence << "" << std::endl;
        outputStatsFile.close();
      } else {
        throw CannotOpenFileError("Error : Couldn't open stats extraction file " + params->getString(STATS_FILE) + ".");
      }
    }

    ofstream outputFile(ruleFile);
    if (outputFile.is_open()) {
      for (const auto &line : lines) {
        outputFile << line << "" << std::endl;
      }
      outputFile.close();
    } else {
      throw CannotOpenFileError("Error : Couldn't open rule extraction file " + ruleFile + ".");
    }

    d2 = clock();
    temps2 = (float)(d2 - d1) / CLOCKS_PER_SEC;

    std::cout << "\nTime without data import = " << temps2 << " sec" << std::endl;

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << endl;
    return -1;
  }

  return 0;
}

/*

  Exemple pour lancer le code :

  ./fidex --train_data_file datanorm --train_pred_file dimlp.out --train_class_file dataclass2 --test_data_file testSampleDataCombine --nb_attributes 16 --nb_classes 2 --weights_file dimlp.wts --nb_quant_levels 50 --rules_outfile rule.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../fidex/datafiles

  ./fidex --train_data_file datanorm --train_pred_file dimlp.out --train_class_file dataclass2 --test_data_file testData.txt --nb_attributes 16 --nb_classes 2 --test_pred_file testPred.txt --test_class_file testClass.txt --weights_file dimlp.wts --nb_quant_levels 50 --rules_outfile rule.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../fidex/datafiles
  ./fidex --train_data_file isoletTrainData.txt --train_pred_file isoletTrainPredV2.out --train_class_file isoletTrainClass.txt --test_data_file isoletTestData.txt --nb_attributes 16 --nb_classes 2 --test_pred_file isoletTestPredV2.out --test_class_file isoletTestClass.txt --weights_file isoletV2.wts --nb_quant_levels 50 --rules_outfile ruleFidex.txt --stats_file stats --max_iterations 100 --min_covering 25 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../dimlp/datafiles/isoletDataset
  ./fidex --train_data_file Train/X_train.txt --train_pred_file Train/pred_trainV2.out --train_class_file Train/y_train.txt --test_data_file Test/X_test.txt --nb_attributes 16 --nb_classes 2 --test_pred_file Test/pred_testV2.out --test_class_file Test/y_test.txt --weights_file HAPTV2.wts --nb_quant_levels 50 --rules_outfile ruleFidexV2.txt --stats_file stats --max_iterations 100 --min_covering 2 --dropout_dim 0.5 --dropout_hyp 0.5 --root_folder ../dimlp/datafiles/HAPTDataset


  #include <profileapi.h>

  LARGE_INTEGER frequency;        - ticks per second
  LARGE_INTEGER t1, t2;           - ticks
  double elapsedTime;

  get ticks per second
  QueryPerformanceFrequency(&frequency);

  start timer
  QueryPerformanceCounter(&t1);

  do something
  ...

  stop timer
  QueryPerformanceCounter(&j2WB2);

  compute and print the elapsed time in millisec
  elapsedTimeWB2 = (j2WB2.QuadPart - j1WB2.QuadPart) * 1000.0 / frequencyWB2.QuadPart;
  std::printf("\npart 2 : Full execution time = %f sec\n", elapsedTimeWB2);

*/
