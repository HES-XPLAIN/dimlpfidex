#include "fidexAlgo.h"

Fidex::Fidex(DataSetFid &trainDataset, Parameters &parameters, Hyperspace &hyperspace) : _trainDataset(&trainDataset), _parameters(&parameters), _hyperspace(&hyperspace) {
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
  Hyperspace *hyperspace = _hyperspace;
  int nbAttributes = _trainDataset->getNbAttributes();
  vector<int> *trainPreds = _trainDataset->getPredictions();
  vector<int> *trainTrueClass = _trainDataset->getClasses();
  vector<vector<double>> *trainData = _trainDataset->getDatas();
  vector<vector<double>> *trainOutputValuesPredictions = _trainDataset->getOutputValuesPredictions();
  int nbInputs = hyperspace->getHyperLocus().size();
  int itMax = _parameters->getInt(MAX_ITERATIONS);
  double dropoutDim = _parameters->getFloat(DROPOUT_DIM);
  double dropoutHyp = _parameters->getFloat(DROPOUT_HYP);
  bool hasdd = dropoutDim > 0.01;
  bool hasdh = dropoutHyp > 0.01;
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

  uniform_real_distribution<double> dis(0.0, 1.0);

  // Compute initial covering
  vector<int> coveredSamples((*trainData).size());     // Samples covered by the hyperbox
  iota(begin(coveredSamples), end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

  // Store covering and compute initial fidelty
  hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
  hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox
  hyperspace->getHyperbox()->resetDiscriminativeHyperplans();             // If we come from fidexGlo, we reset hyperbox discriminativeHyperplans

  int nbIt = 0;

  while (hyperspace->getHyperbox()->getFidelity() < minFidelity && nbIt < itMax) { // While fidelity of our hyperbox is not high enough
    unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());                             // best hyperbox to choose for next step
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
    for (int d = 0; d < nbInputs; d++) {
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

  if (hyperspace->getHyperbox()->getFidelity() < minFidelity) {
    return false;
  }

  // Compute rule accuracy
  double ruleAccuracy;
  if (hasTrueClasses) {
    bool mainSampleCorrect = mainSamplePred == mainSampleClass;
    ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses, mainSampleCorrect); // Percentage of correct model prediction on samples covered by the rule
  } else {
    ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass, hasTrueClasses); // Percentage of correct model prediction on samples covered by the rule
  }

  double ruleConfidence;
  ruleConfidence = hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred); // Mean output value of prediction of class chosen by the rule for the covered samples
  if (_parameters->isDoubleVectorSet(MUS)) {
    rule = hyperspace->ruleExtraction(&mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence, &mus, &sigmas, &normalizationIndices);
  } else {
    rule = hyperspace->ruleExtraction(&mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence);
  }

  return true;
}
