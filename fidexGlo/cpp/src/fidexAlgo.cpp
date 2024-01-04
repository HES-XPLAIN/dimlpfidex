#include "fidexAlgo.h"

// TODO: the "seed" argument is not really useful anymore as it's contained by the Parameter object, should be removed
Fidex::Fidex(DataSetFid *dataset, Parameters *parameters, Hyperspace *hyperspace, int seed) {
  _dataset = dataset;
  _parameters = parameters;
  _hyperspace = hyperspace;

  if (seed == 0) {
    _rnd = generateRandom(seed);
  } else {
    _rnd = mt19937(seed);
  }
}

mt19937 Fidex::generateRandom(int seed) {
  auto currentTime = high_resolution_clock::now();
  auto seedValue = currentTime.time_since_epoch().count();
  mt19937 random(seedValue);

  return random;
}

bool Fidex::compute(Rule *rule, int idSample, float minFidelity) {
  int threadId = omp_get_thread_num();

  // double t_init_s = omp_get_wtime();
  int nbDatas = _dataset->getNbSamples();
  int nbAttributes = _dataset->getNbAttributes();
  vector<int> *trainPreds = _dataset->getPredictions();
  vector<int> *trainTrueClass = _dataset->getClasses();
  vector<vector<double>> *trainData = _dataset->getDatas();
  vector<double> *mainSampleValues = &(*trainData)[idSample];
  vector<vector<double>> *trainOutputValuesPredictions = _dataset->getOutputValuesPredictions();
  int mainSamplePred = (*trainPreds)[idSample];
  int nbInputs = _hyperspace->getHyperLocus().size();
  int itMax = _parameters->getInt(MAX_ITERATIONS);
  int minNbCover = _parameters->getInt(MIN_COVERING);
  float dropoutDim = _parameters->getFloat(DROPOUT_DIM);
  float dropoutHyp = _parameters->getFloat(DROPOUT_HYP);
  bool hasdd = dropoutDim > 0.001;
  bool hasdh = dropoutHyp > 0.001;
  // double t_init_e = omp_get_wtime();

  // cout << "Thread#" << threadId << " var init time cost: " << (t_init_e - t_init_s) << " seconds." << endl;

  uniform_real_distribution<double> dis(0.0, 1.0);

  // Compute initial covering
  vector<int> coveredSamples((*trainData).size());     // Samples covered by the hyperbox
  iota(begin(coveredSamples), end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

  // Store covering and compute initial fidelty
  _hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
  _hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox
  _hyperspace->getHyperbox()->resetDiscriminativeHyperplans();             // If we come from fidexGlo, we reset hyperbox discriminativeHyperplans

  int nbIt = 0;

  double t_maxit_s = omp_get_wtime();
  double sum_innerfor = 0;
  double sum_doublecmp = 0;
  while (_hyperspace->getHyperbox()->getFidelity() < minFidelity && nbIt < itMax) { // While fidelity of our hyperbox is not high enough
    unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());                              // best hyperbox to choose for next step
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
      mainSampleValue = (*mainSampleValues)[attribut];

      // Test if we dropout this dimension
      // double t_dd_s = omp_get_wtime();
      if (hasdd && isless(dis(_rnd), dropoutDim)) {
        continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
      }
      // double t_dd_e = omp_get_wtime();
      // cout << "-Thread#" << threadId << " Double to double comparison (drop_dim) time cost: " << (t_dd_e - t_dd_s) << " seconds." << endl;
      bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

      size_t nbHyp = _hyperspace->getHyperLocus()[dimension].size();
      if (nbHyp == 0) {
        continue; // No data on this dimension
      }

      double t_innerfor_s = omp_get_wtime();
      //! this is probably slow in some cases
      // cout << "nbHyp" << nbHyp << endl;
      for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)

        // Test if we dropout this hyperplan
        //! is 0.0003s a lot for double comparisons ?
        double t_dh_s = omp_get_wtime();
        if (hasdh && isless(dis(_rnd), dropoutHyp)) {
          continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
        }
        double t_dh_e = omp_get_wtime();
        sum_doublecmp += t_dh_e - t_dh_s;
        // cout << "--Thread#" << threadId << " Double to double comparison (drop_hyp) time cost: " << (t_dh_e - t_dh_s) << " seconds." << endl;

        double hypValue = _hyperspace->getHyperLocus()[dimension][k];
        bool mainSampleGreater = hypValue <= mainSampleValue;                                                                                      // Check if main sample value is on the right of the hyperplan
        currentHyperbox->computeCoveredSamples(_hyperspace->getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
        currentHyperbox->computeFidelity(mainSamplePred, trainPreds);                                                                              // Compute fidelity
        //! ok checkpoint
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

      double t_innerfor_e = omp_get_wtime();
      sum_innerfor += (t_innerfor_e - t_innerfor_s);
      // cout << "----Thread #" << threadId << " inner loop time cost: " << (t_innerfor_e - t_innerfor_s) << " seconds." << endl;
    }

    // Modication of our hyperbox with the best at this iteration and modify discriminative hyperplans
    if (indexBestHyp != -1 && bestDimension != -1) { // If we found any good dimension with good hyperplan (with enough covering)
      if (maxHyp != -1) {
        indexBestHyp = (maxHyp + minHyp) / 2;
      }
      // antecedant is not added if fidelity and covering size did not increase
      if (bestHyperbox->getFidelity() > _hyperspace->getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == _hyperspace->getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > _hyperspace->getHyperbox()->getCoveredSamples().size())) {
        _hyperspace->getHyperbox()->setFidelity(bestHyperbox->getFidelity());
        _hyperspace->getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
        _hyperspace->getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
      }
    }
    nbIt += 1;
  }

  double t_maxit_e = omp_get_wtime();
  // if ((t_maxit_e - t_maxit_s) > 0.18) {
  //   cout << "---Thread #" << threadId << " Sum of innerfor time: " << sum_innerfor << " seconds." << endl;
  //   cout << "---Thread #" << threadId << " Sum of doublecmp time: " << sum_doublecmp << " seconds." << endl;
  //   cout << "--Thread #" << threadId << " Max iteration loop (main one) time cost: " << (t_maxit_e - t_maxit_s) << " seconds. nbTt: " << nbIt << endl;
  // }
  if (_hyperspace->getHyperbox()->getFidelity() < minFidelity) {
    return false;
  }

  // Compute rule accuracy
  double ruleAccuracy;
  ruleAccuracy = _hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass); // Percentage of correct model prediction on samples covered by the rule

  // double t_ruleextract_s = omp_get_wtime();
  double ruleConfidence;
  ruleConfidence = _hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred); // Mean output value of prediction of class chosen by the rule for the covered samples
  *rule = _hyperspace->ruleExtraction(mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence);
  // double t_ruleextract_e = omp_get_wtime();
  // cout << "Thread#" << threadId << " Rule confidence & rule extraction time cost: " << (t_ruleextract_e - t_ruleextract_s) << " seconds." << endl;

  return true;
}

// bool FidexAlgo::fidex(Rule &rule,
//                       DataSetFid *dataset,
//                       Parameters *p,
//                       Hyperspace *hyperspace,
//                       int idSample,
//                       double minFidelity,
//                       mt19937 gen) const {

//   // Get diverse elements of the provided dataset
//   int nbDatas = dataset->getNbSamples();
//   int nbAttributes = dataset->getNbAttributes();
//   vector<vector<double>> *trainData = dataset->getDatas();
//   vector<int> *trainPreds = dataset->getPredictions();
//   vector<vector<double>> *trainOutputValuesPredictions = dataset->getOutputValuesPredictions();
//   vector<int> *trainTrueClass = dataset->getClasses();
//   vector<double> *mainSampleValues = &(*trainData)[idSample];
//   int mainSamplePred = (*trainPreds)[idSample];
//   int nbInputs = hyperspace->getHyperLocus().size();
//   int itMax = p->getInt(MAX_ITERATIONS);
//   int minNbCover = p->getInt(MIN_COVERING);
//   float dropoutDim = p->getFloat(DROPOUT_DIM);
//   float dropoutHyp = p->getFloat(DROPOUT_HYP);

//   // Initialize uniform distribution
//   uniform_real_distribution<double> dis(0.0, 1.0);

//   // Compute initial covering
//   vector<int> coveredSamples((*trainData).size());     // Samples covered by the hyperbox
//   iota(begin(coveredSamples), end(coveredSamples), 0); // Vector from 0 to len(coveredSamples)-1

//   // Store covering and compute initial fidelty
//   hyperspace->getHyperbox()->setCoveredSamples(coveredSamples);
//   hyperspace->getHyperbox()->computeFidelity(mainSamplePred, trainPreds); // Compute fidelity of initial hyperbox
//   // If we come from fidexGlo, we reset hyperbox discriminativeHyperplans
//   hyperspace->getHyperbox()->resetDiscriminativeHyperplans();

//   while (hyperspace->getHyperbox()->getFidelity() < minFidelity && itMax) { // While fidelity of our hyperbox is not high enough
//     unique_ptr<Hyperbox> bestHyperbox(new Hyperbox());                      // best hyperbox to choose for next step
//     unique_ptr<Hyperbox> currentHyperbox(new Hyperbox());
//     double mainSampleValue;
//     int attribut;
//     int dimension;
//     int indexBestHyp = -1;
//     int bestDimension = -1;
//     int minHyp = -1; // Index of first hyperplan without any change of the best hyperplan
//     int maxHyp = -1;
//     // Randomize dimensions
//     vector<int> dimensions(nbInputs);
//     iota(begin(dimensions), end(dimensions), 0); // Vector from 0 to nbIn-1
//     shuffle(begin(dimensions), end(dimensions), gen);

//     vector<int> currentCovSamp;
//     for (int d = 0; d < nbInputs; d++) {
//       if (bestHyperbox->getFidelity() >= minFidelity) {
//         break;
//       }

//       dimension = dimensions[d];
//       attribut = dimension % nbAttributes;
//       mainSampleValue = (*mainSampleValues)[attribut];

//       // Test if we dropout this dimension
//       if (dis(gen) < dropoutDim) {
//         continue; // Drop this dimension if below parameter ex: param=0.2 -> 20% are dropped
//       }
//       bool maxHypBlocked = true; // We assure that we can't increase maxHyp index for the current best hyperbox

//       size_t nbHyp = hyperspace->getHyperLocus()[dimension].size();
//       if (nbHyp == 0) {
//         continue; // No data on this dimension
//       }
//       for (int k = 0; k < nbHyp; k++) { // for each possible hyperplan in this dimension (there is nbSteps+1 hyperplans per dimension)

//         // Test if we dropout this hyperplan
//         if (dis(gen) < dropoutHyp) {
//           continue; // Drop this hyperplan if below parameter ex: param=0.2 -> 20% are dropped
//         }

//         double hypValue = hyperspace->getHyperLocus()[dimension][k];
//         bool mainSampleGreater = hypValue <= mainSampleValue;                                                                                     // Check if main sample value is on the right of the hyperplan
//         currentHyperbox->computeCoveredSamples(hyperspace->getHyperbox()->getCoveredSamples(), attribut, trainData, mainSampleGreater, hypValue); // Compute new cover samples
//         currentHyperbox->computeFidelity(mainSamplePred, trainPreds);                                                                             // Compute fidelity
//         // If the fidelity is better or is same with better covering but not if covering size is lower than minNbCover
//         if (currentHyperbox->getCoveredSamples().size() >= minNbCover && (currentHyperbox->getFidelity() > bestHyperbox->getFidelity() || (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() > bestHyperbox->getCoveredSamples().size()))) {
//           bestHyperbox->setFidelity(currentHyperbox->getFidelity()); // Update best hyperbox
//           bestHyperbox->setCoveredSamples(currentHyperbox->getCoveredSamples());
//           indexBestHyp = k;
//           minHyp = k; // New best
//           maxHyp = -1;
//           maxHypBlocked = false; // We can increase maxHyp if next is the same
//           bestDimension = dimension;
//         } else if (currentHyperbox->getFidelity() == bestHyperbox->getFidelity() && currentHyperbox->getCoveredSamples().size() == bestHyperbox->getCoveredSamples().size()) {
//           if (!maxHypBlocked) {
//             maxHyp = k; // Index of last (for now) hyperplan which is equal to the best.
//           }
//         } else {
//           maxHypBlocked = true; // we can't increase maxHyp anymmore for this best hyperplan
//         }

//         if (bestHyperbox->getFidelity() >= minFidelity) {
//           break;
//         }
//       }
//     }

//     // Modication of our hyperbox with the best at this iteration and modify discriminative hyperplans
//     if (indexBestHyp != -1 && bestDimension != -1) { // If we found any good dimension with good hyperplan (with enough covering)
//       if (maxHyp != -1) {
//         indexBestHyp = (maxHyp + minHyp) / 2;
//       }
//       // antecedant is not added if fidelity and covering size did not increase
//       if (bestHyperbox->getFidelity() > hyperspace->getHyperbox()->getFidelity() || (bestHyperbox->getFidelity() == hyperspace->getHyperbox()->getFidelity() && bestHyperbox->getCoveredSamples().size() > hyperspace->getHyperbox()->getCoveredSamples().size())) {
//         hyperspace->getHyperbox()->setFidelity(bestHyperbox->getFidelity());
//         hyperspace->getHyperbox()->setCoveredSamples(bestHyperbox->getCoveredSamples());
//         hyperspace->getHyperbox()->discriminateHyperplan(bestDimension, indexBestHyp);
//       }
//     }
//     itMax -= 1;
//   }

//   if (hyperspace->getHyperbox()->getFidelity() < minFidelity) {
//     return false;
//   }

//   // Compute rule accuracy
//   double ruleAccuracy;
//   ruleAccuracy = hyperspace->computeRuleAccuracy(trainPreds, trainTrueClass); // Percentage of correct model prediction on samples covered by the rule

//   double ruleConfidence;
//   ruleConfidence = hyperspace->computeRuleConfidence(trainOutputValuesPredictions, mainSamplePred); // Mean output value of prediction of class chosen by the rule for the covered samples
//   rule = hyperspace->ruleExtraction(mainSampleValues, mainSamplePred, ruleAccuracy, ruleConfidence);

//   return true;
// }

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
