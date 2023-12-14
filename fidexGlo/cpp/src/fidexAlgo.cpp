#include "fidexAlgo.h"

FidexAlgo::FidexAlgo() = default;

// Different mains:

// OPENMP: hyperspace is a shared ressource that might produce concurrency errors
bool FidexAlgo::fidex(Rule &rule,
                      DataSetFid *dataset,
                      Parameters *p,
                      vector<vector<double>> *hyperlocus,
                      int idSample,
                      mt19937 gen) const {

  // Get diverse elements of the provided dataset
  int nbDatas = dataset->getNbSamples();
  int nbAttributes = dataset->getNbAttributes();
  vector<vector<double>> *trainData = dataset->getDatas();
  vector<int> *trainPreds = dataset->getPredictions(); // bug

  // for (int i : *trainPreds) {
  //   cout << i << ", ";
  // }
  // cout << endl;

  vector<vector<double>> *trainOutputValuesPredictions = dataset->getOutputValuesPredictions();
  vector<int> *trainTrueClass = dataset->getClasses();
  vector<double> *mainSampleValues = &(*trainData)[idSample];
  int mainSamplePred = (*trainPreds)[idSample];

  int nbInputs = hyperlocus->size();
  int itMax = p->getItMax();
  int minNbCover = p->getMinNbCover();
  double minFidelity = p->getMinFidelity();
  double dropoutDim = p->getDropoutDim();
  double dropoutHyp = p->getDropoutHyp();
  Hyperspace *hyperspace = new Hyperspace(*hyperlocus);

  // Initialize uniform distribution
  uniform_real_distribution<double> dis(0.0, 1.0);

  // Compute initial covering
  vector<int> coveredSamples((*trainData).size());     // Samples covered by the hyperbox
  iota(begin(coveredSamples), end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

  // Store covering and compute initial fidelty
  hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
  hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox
  // If we come from fidexGlo, we reset hyperbox discriminativeHyperplans
  hyperspace->getHyperbox()->resetDiscriminativeHyperplans();
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
    shuffle(begin(dimensions), end(dimensions), gen);

    vector<int> currentCovSamp;
    for (int d = 0; d < nbInputs; d++) {
      if (bestHyperbox->getFidelity() >= minFidelity) {
        break;
      }

      dimension = dimensions[d];
      attribut = dimension % nbAttributes;
      mainSampleValue = (*mainSampleValues)[attribut];

      // Test if we dropout this dimension
      if (dropoutDim > 0.0f && dis(gen) < dropoutDim) {
        continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
      }
      bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

      size_t nbHyp = hyperspace->getHyperLocus()[dimension].size();
      if (nbHyp == 0) {
        continue; // No data on this dimension
      }
      for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)

        // Test if we dropout this hyperplan
        if (dropoutHyp > 0.0f && dis(gen) < dropoutHyp) {
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

  ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass); // Percentage of correct model prediction on samples covered by the rule

  double ruleConfidence;
  ruleConfidence = hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred); // Mean output value of prediction of class chosen by the rule for the covered samples
  rule = hyperspace->ruleExtraction(mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence);

  return true;
}

/*

    #include <Windows.h>

    LARGE_INTEGER frequency2;        ticks per second
    LARGE_INTEGER g1, g2;           ticks
    double elapsedTime2;

    get ticks per second
    QueryPerformanceFrequency(&frequency2);

    start timer
    QueryPerformanceCounter(&g1);

    stop timer
    QueryPerformanceCounter(&g2);

    compute and print the elapsed time in millisec
    elapsedTime2 = (g2.QuadPart - g1.QuadPart) * 1000.0 / frequency2.QuadPart;

    */
