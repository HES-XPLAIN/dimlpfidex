#include "fidexAlgo.h"

using namespace std;
FidexAlgo::FidexAlgo() = default;

// Different mains:

bool FidexAlgo::fidex(std::tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> &rule, vector<vector<double>> *trainData, vector<int> *trainPreds, bool hasConfidence, vector<vector<double>> *trainOutputValuesPredictions, vector<int> *trainTrueClass, vector<double> *mainSampleValues, int mainSamplePred, FidexGloNameSpace::Hyperspace *hyperspace, const int nbIn, const int nbAttributs, int itMax, int minNbCover, bool dropoutDim, double dropoutDimParam, bool dropoutHyp, double dropoutHypParam, std::mt19937 gen) const {

  // Initialize uniform distribution
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  // Compute initial covering
  vector<int> coveredSamples((*trainData).size());                    // Samples covered by the hyperbox
  std::iota(std::begin(coveredSamples), std::end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

  // Store covering and compute initial fidelty
  hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
  hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox

  // If we come from fidexGlo, we reset hyperbox discriminativeHyperplans
  hyperspace->getHyperbox()->resetDiscriminativeHyperplans();

  int nbIt = 0;

  while (hyperspace->getHyperbox()->getFidelity() != 1 && nbIt < itMax) { // While fidelity of our hyperbox is not 100%

    std::unique_ptr<Hyperbox> bestHyperbox(new Hyperbox()); // best hyperbox to choose for next step
    std::unique_ptr<Hyperbox> currentHyperbox(new Hyperbox());
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

    for (int d = 0; d < nbIn; d++) {
      if (bestHyperbox->getFidelity() == 1) {
        break;
      }
      dimension = dimensions[d];
      attribut = dimension % nbAttributs;
      mainSampleValue = (*mainSampleValues)[attribut];

      // Test if we dropout this dimension
      if (dropoutDim && dis(gen) < dropoutDimParam) {
        continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
      }
      bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

      size_t nbHyp = hyperspace->getHyperLocus()[dimension].size();
      if (nbHyp == 0) {
        continue; // No data on this dimension
      }

      for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)

        // Test if we dropout this hyperplan
        if (dropoutHyp && dis(gen) < dropoutHypParam) {
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

        if (bestHyperbox->getFidelity() == 1) {
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
      if (bestHyperbox->getFidelity() > hyperspace->getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == hyperspace->getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > hyperspace->getHyperbox()->getCoveredSamples().size())) {
        hyperspace->getHyperbox()->setFidelity(bestHyperbox->getFidelity());
        hyperspace->getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
        hyperspace->getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
      }
    }
    nbIt += 1;
  }

  if (hyperspace->getHyperbox()->getFidelity() != 1) {
    return false;
  }

  // Compute rule accuracy

  double ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass); // Percentage of correct model prediction on samples covered by the rule
  double ruleConfidence;
  if (hasConfidence) {
    ruleConfidence = hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred); // Mean output value of prediction of class chosen by the rule for the covered samples
  } else {
    ruleConfidence = -1;
  }
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
