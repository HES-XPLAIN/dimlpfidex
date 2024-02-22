#include "fidexAlgo.h"

Fidex::Fidex(DataSetFid &trainDataset, Parameters &parameters, Hyperspace &hyperspace, bool usingTestSamples) : _trainDataset(&trainDataset), _parameters(&parameters), _hyperspace(&hyperspace), _usingTestSamples(usingTestSamples) {
  int seed = parameters.getInt(SEED);

  if (seed == 0) {
    auto currentTime = high_resolution_clock::now();
    auto seedValue = currentTime.time_since_epoch().count();
    _rnd.seed(seedValue);
  } else {
    _rnd.seed(seed);
  }
}

bool Fidex::compute(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, double minFidelity, int minNbCover, int mainSampleClass) {

  specs.nbIt = 0;

  bool showInitialFidelity = getShowInitialFidelity();
  double mainSamplePredValue = getMainSamplePredValue();

  Hyperspace *hyperspace = _hyperspace;
  int nbAttributes = _trainDataset->getNbAttributes();
  vector<int> &trainPreds = _trainDataset->getPredictions();
  vector<int> &trainTrueClass = _trainDataset->getClasses();
  vector<vector<double>> &trainData = _trainDataset->getDatas();
  vector<vector<double>> &trainOutputValuesPredictions = _trainDataset->getOutputValuesPredictions();
  auto nbInputs = static_cast<int>(hyperspace->getHyperLocus().size());
  int maxIterations = _parameters->getInt(MAX_ITERATIONS);
  double dropoutDim = _parameters->getFloat(DROPOUT_DIM);
  double dropoutHyp = _parameters->getFloat(DROPOUT_HYP);
  bool hasdd = dropoutDim > 0.001;
  bool hasdh = dropoutHyp > 0.001;
  bool hasTrueClasses = true;
  if (mainSampleClass == -1) {
    hasTrueClasses = false;
  }

  vector<int> normalizationIndices;
  vector<double> mus;
  vector<double> sigmas;

  if (_parameters->isIntVectorSet(NORMALIZATION_INDICES)) {
    normalizationIndices = _parameters->getIntVector(NORMALIZATION_INDICES);
  }
  if (_parameters->isDoubleVectorSet(MUS)) {
    mus = _parameters->getDoubleVector(MUS);
  }
  if (_parameters->isDoubleVectorSet(SIGMAS)) {
    sigmas = _parameters->getDoubleVector(SIGMAS);
  }
  if (_parameters->isDoubleVectorSet(MUS) && !(_parameters->isIntVectorSet(NORMALIZATION_INDICES) && _parameters->isDoubleVectorSet(SIGMAS))) {
    throw InternalError("Error during computation of Fidex: mus are specified but sigmas or normalization indices are not specified.");
  }

  if (mainSamplePredValue == -1.0 && _usingTestSamples) {
    throw InternalError("Error during computation of Fidex: Execution with a test sample but no sample prediction value has been given.");
  }

  uniform_real_distribution<double> dis(0.0, 1.0);

  // Compute initial covering
  vector<int> coveredSamples(trainData.size());        // Samples covered by the hyperbox
  iota(begin(coveredSamples), end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

  // Store covering and compute initial fidelty
  hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
  hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox
  hyperspace->getHyperbox()->resetDiscriminativeHyperplans();             // We reset hyperbox discriminativeHyperplans

  if (_usingTestSamples && showInitialFidelity) {
    std::cout << "Initial fidelity : " << hyperspace->getHyperbox()->getFidelity() << endl;
  }

  int nbIt = 0;

  while (hyperspace->getHyperbox()->getFidelity() < minFidelity && nbIt < maxIterations) { // While fidelity of our hyperbox is not high enough
    unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());                                     // best hyperbox to choose for next step
    unique_ptr<Hyperbox> currentHyperbox(new Hyperbox());
    double mainSampleValue;
    int attribut;
    int dimension;
    int indexBestHyp = -1;
    int bestDimension = -1;
    int minHyp = -1; // Index of first hyperplan without any change of the best hyperplan
    int maxHyp = -1;
    // Randomize dimensions
    vector<int> dimensions(nbInputs);
    iota(begin(dimensions), end(dimensions), 0); // Vector from 0 to nbIn-1
    shuffle(begin(dimensions), end(dimensions), _rnd);

    vector<int> currentCovSamp;
    for (int d = 0; d < nbInputs; d++) { // Loop on all dimensions
      if (bestHyperbox->getFidelity() >= minFidelity) {
        break;
      }

      dimension = dimensions[d];
      attribut = dimension % nbAttributes;
      mainSampleValue = mainSampleValues[attribut];

      // Test if we dropout this dimension
      if (hasdd && dis(_rnd) < dropoutDim) {
        continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
      }
      bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

      size_t nbHyp = hyperspace->getHyperLocus()[dimension].size();
      if (nbHyp == 0) {
        continue; // No data on this dimension
      }

      for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)
        // Test if we dropout this hyperplan
        if (hasdh && dis(_rnd) < dropoutHyp) {
          continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
        }

        double hypValue = hyperspace->getHyperLocus()[dimension][k];
        bool mainSampleGreater = hypValue <= mainSampleValue;                                                                                     // Check if main sample value is on the right of the hyperplan
        currentHyperbox->computeCoveredSamples(hyperspace->getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
        currentHyperbox->computeFidelity(mainSamplePred, trainPreds);                                                                             // Compute fidelity

        // If the fidelity is better or is same with better covering but not if covering size is lower than minNbCover
        if (currentHyperbox->getCoveredSamples().size() >= minNbCover && (currentHyperbox->getFidelity() > bestHyperbox->getFidelity() || (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() > bestHyperbox->getCoveredSamples().size()))) {

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
          maxHypBlocked = true; // we can't increase maxHyp anymmore for this best hyperplan
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
      // antecedant is not added if fidelity and covering size did not increase
      if (bestHyperbox->getFidelity() > hyperspace->getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == hyperspace->getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > hyperspace->getHyperbox()->getCoveredSamples().size())) {
        hyperspace->getHyperbox()->setFidelity(bestHyperbox->getFidelity());
        hyperspace->getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
        hyperspace->getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
      }
    }
    nbIt += 1;
  }

  // Compute rule accuracy and confidence
  double ruleAccuracy;
  if (hasTrueClasses && _usingTestSamples) {
    bool mainSampleCorrect = mainSamplePred == mainSampleClass;
    ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses, mainSampleCorrect); // Percentage of correct model prediction on samples covered by the rule
  } else {
    ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses); // Percentage of correct model prediction on samples covered by the rule
  }

  double ruleConfidence;
  ruleConfidence = hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred, mainSamplePredValue); // Mean output value of prediction of class chosen by the rule for the covered samples

  // Extract rules
  if (_parameters->isDoubleVectorSet(MUS)) {
    rule = hyperspace->ruleExtraction(mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence, mus, sigmas, normalizationIndices);
  } else {
    rule = hyperspace->ruleExtraction(mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence);
  }

  // Reset execution specs
  specs.showInitialFidelity = false;
  setNbIt(nbIt);

  if (hyperspace->getHyperbox()->getFidelity() < minFidelity) {
    return false;
  }

  return true;
}

/**
 * @brief Attempts to compute a rule with Fidex algorithm based on given parameters and updates the rule object if successful.
 *
 * @param rule Reference to the Rule object to be potentially updated by the computation.
 * @param mainSampleValues A vector of double values representing the main sample values.
 * @param mainSamplePred An integer representing the predicted class of the main sample.
 * @param minFidelity A float representing the minimum fidelity threshold for rule creation.
 * @param minNbCover An integer representing the minimum number of samples a rule must cover.
 * @param mainSampleClass An integer representing the class of the main sample.
 * @param verbose A boolean flag for detailed verbose output.
 * @param detailedVerbose A boolean flag for detailed verbose output. Default is false.
 * @param foundRule A boolean indicating whether a rule was found in a previous attempt. Default is false.
 * @return true If a rule meeting the criteria is successfully computed.
 * @return false If no rule meeting the criteria can be computed.
 */
bool Fidex::tryComputeFidex(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, bool verbose, bool detailedVerbose, bool foundRule) {
  if (detailedVerbose && verbose) {
    if (foundRule) {
      std::cout << "A rule has been found. ";
    } else {
      std::cout << "Fidelity is too low. ";
    }
    std::cout << "Restarting fidex with a minimum covering of " << minNbCover << " and a minimum accepted fidelity of " << minFidelity << "." << std::endl;
  }

  bool ruleCreated = compute(rule, mainSampleValues, mainSamplePred, minFidelity, minNbCover, mainSampleClass);
  if (verbose) {
    std::cout << "Final fidelity : " << rule.getFidelity() << endl;
  }
  return ruleCreated;
}

/**
 * @brief Performs a dichotomic (binary) search to find a rule with the best covering that meets the minimum fidelity criteria.
 *
 * @param bestRule Reference to the Rule object to store the best rule found during the search.
 * @param mainSampleValues A vector of double values representing the main sample values.
 * @param mainSamplePred An integer representing the predicted class of the main sample.
 * @param minFidelity A float representing the minimum fidelity threshold for rule creation.
 * @param mainSampleClass An integer representing the class of the main sample.
 * @param left The starting point of the search range.
 * @param right The ending point of the search range.
 * @param verbose A boolean flag for detailed verbose output.
 * @return The best covering found that meets the minimum fidelity criteria. Returns -1 if no such covering is found.
 */
int Fidex::dichotomicSearch(Rule &bestRule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int mainSampleClass, int left, int right, bool verbose) {
  int bestCovering = -1;
  int currentMinNbCover = right + 1;
  bool foundRule = false;
  while (currentMinNbCover != ceil((right + left) / 2.0) && left <= right) {
    currentMinNbCover = static_cast<int>(ceil((right + left) / 2.0));
    Rule tempRule;
    if (tryComputeFidex(tempRule, mainSampleValues, mainSamplePred, minFidelity, currentMinNbCover, mainSampleClass, verbose, true, foundRule)) {
      bestCovering = currentMinNbCover;
      bestRule = tempRule;
      left = currentMinNbCover + 1;
      foundRule = true;
    } else {
      right = currentMinNbCover - 1;
      foundRule = false;
    }
  }
  return bestCovering;
}

/**
 * @brief Attempts to compute a rule multiple times up to a maximum number of failed attempts, adjusting fidelity if necessary.
 *
 * @param rule Reference to the Rule object to be potentially updated by the computation.
 * @param mainSampleValues A vector of double values representing the main sample values.
 * @param mainSamplePred An integer representing the predicted class of the main sample.
 * @param minFidelity A reference to a float representing the current minimum fidelity threshold for rule creation. May be adjusted.
 * @param minNbCover An integer representing the minimum number of samples a rule must cover.
 * @param mainSampleClass An integer representing the class of the main sample.
 * @param verbose A boolean flag for detailed verbose output.
 * @return true If a rule meeting the criteria is successfully computed within the maximum number of attempts.
 * @return false If no rule meeting the criteria can be computed within the maximum number of attempts.
 */
bool Fidex::retryComputeFidex(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, float minFidelity, int minNbCover, int mainSampleClass, bool verbose) {
  int counterFailed = 0; // Number of times we failed to find a rule with maximal fidexlity when minNbCover is 1
  int maxFailedAttempts = _parameters->getInt(MAX_FAILED_ATTEMPTS);
  bool ruleCreated = false;
  bool hasDropout = _parameters->getFloat(DROPOUT_DIM) > 0.001 || _parameters->getFloat(DROPOUT_HYP) > 0.001;
  do {
    ruleCreated = tryComputeFidex(rule, mainSampleValues, mainSamplePred, minFidelity, minNbCover, mainSampleClass, verbose, true);
    if (!ruleCreated) {
      counterFailed += 1;
    }
    if (counterFailed >= maxFailedAttempts && verbose) {
      std::cout << "\nWARNING Fidelity is too low after trying " << std::to_string(maxFailedAttempts) << " times with a minimum covering of " << minNbCover << " and a minimum accepted fidelity of " << minFidelity << "! You may want to try again." << std::endl;
      if (hasDropout) {
        std::cout << "Try to not use dropout." << std::endl;
      }
    }
  } while (!ruleCreated && counterFailed < maxFailedAttempts);

  return ruleCreated;
}

/**
 * @brief Launches the Fidex algorithm with specified parameters to attempt creating a rule that meets given minimum covering and minimum fidelity criteria.
 *
 * @param rule Reference to the Rule object to be potentially updated by the computation.
 * @param mainSampleValues A vector of double values representing the main sample values.
 * @param mainSamplePred An integer representing the predicted class of the main sample.
 * @param mainSampleClass An integer representing the class of the main sample.
 * @param verbose A boolean flag for detailed verbose output. Default is false.
 *  @return true If a rule meeting the criteria is successfully computed.
 * @return false If no rule meeting the criteria can be computed.
 */
bool Fidex::launchFidex(Rule &rule, vector<double> &mainSampleValues, int mainSamplePred, int mainSampleClass, bool verbose) {

  int minNbCover = _parameters->getInt(MIN_COVERING);
  float minFidelity = _parameters->getFloat(MIN_FIDELITY);
  bool ruleCreated = false;
  bool foundRule = true;

  if (verbose) {
    setShowInitialFidelity(true);
  }

  // Try to find a rule
  if (!tryComputeFidex(rule, mainSampleValues, mainSamplePred, minFidelity, minNbCover, mainSampleClass, verbose)) {
    // If no rule is found

    // With covering strategy
    if (_parameters->getBool(COVERING_STRATEGY)) {
      int right = minNbCover - 1;
      int bestCovering = -1;
      Rule bestRule;

      // Dichotomic search to find a rule with best covering
      if (right > 0) {
        bestCovering = dichotomicSearch(bestRule, mainSampleValues, mainSamplePred, minFidelity, mainSampleClass, 1, right, verbose);
      }

      // Coundn't find a rule with minimal fidelity 1, we search for a lower minimal fidelity
      if (bestCovering == -1) {
        float currentMinFidelity = minFidelity;
        while (!ruleCreated && currentMinFidelity > _parameters->getFloat(LOWEST_MIN_FIDELITY)) {
          currentMinFidelity -= 0.05f;
          ruleCreated = tryComputeFidex(rule, mainSampleValues, mainSamplePred, currentMinFidelity, 1, mainSampleClass, verbose, true);
        }
        // Coundn't find a rule, we retry maxFailedAttempts times
        if (!ruleCreated) {
          foundRule = retryComputeFidex(rule, mainSampleValues, mainSamplePred, currentMinFidelity, 1, mainSampleClass, verbose);
        }

      } else { // If we found a correct rule during dichotomic search
        rule = bestRule;
      }
      if (verbose) {
        std::cout << endl;
      }
      // Without covering strategy
    } else if (verbose) {
      std::cout << "\nWARNING Fidelity is too low! You may want to try again." << std::endl;
      std::cout << "If you can't find a rule with the wanted fidelity, try a lowest minimal covering or a lower fidelity" << std::endl;
      std::cout << "You can also try to use the min cover strategy (--covering_strategy)" << std::endl;
      std::cout << "If this is not enough, put the min covering to 1 and do not use dropout.\n"
                << std::endl;
      foundRule = false;
    }
  }
  if (!foundRule) {
    return false;
  }
  return true;
}
