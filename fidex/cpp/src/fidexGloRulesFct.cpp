#include "fidexGloRulesFct.h"

/**
 * @brief Displays the parameters for fidexGloRules.
 */
void showRulesParams() {
  std::cout << std::endl
            << "---------------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Warning! The files are located with respect to the root folder dimlpfidex." << std::endl;
  std::cout << "The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json." << std::endl
            << std::endl;

  std::cout << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Required parameters:" << std::endl
            << std::endl;

  printOptionDescription("--train_data_file <str>", "Path to the file containing the train portion of the dataset");
  printOptionDescription("--train_class_file <str>", "Path to the file containing the train true classes of the dataset, not mandatory if classes are specified in train data file");
  printOptionDescription("--train_pred_file <str>", "Path to the file containing predictions on the train portion of the dataset");
  printOptionDescription("--weights_file <str>", "Path to the file containing the trained weights of the model (not mandatory if a rules file is given with --rules_file)");
  printOptionDescription("--rules_file <str>", "Path to the file containing the trained rules to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)");
  printOptionDescription("--global_rules_outfile <str>", "Path to the file where the output rule(s) will be stored. If a .json extension is given, rules are saved in JSON format");
  printOptionDescription("--heuristic <int [1,3]>", "Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo. (Faster algorithms are less efficient)");
  printOptionDescription("--nb_attributes <int [1,inf[>", "Number of attributes in the dataset");
  printOptionDescription("--nb_classes <int [2,inf[>", "Number of classes in the dataset");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Optional parameters: " << std::endl
            << std::endl;

  printOptionDescription("--json_config_file <str>", "Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path");
  printOptionDescription("--root_folder <str>", "Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder");
  printOptionDescription("--attributes_file <str>", "Path to the file containing the labels of attributes and classes");
  printOptionDescription("--console_file <str>", "Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal");
  printOptionDescription("--max_iterations <int [1,inf[>", "Maximum iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images (default: 10)");
  printOptionDescription("--min_covering <int [1,inf[>", "Minimum number of samples covered by the generated rules (default: 2)");
  printOptionDescription("--covering_strategy <bool>", "Whether to use the covering strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)");
  printOptionDescription("--max_failed_attempts <int [0,inf[>", "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used (default: 30)");
  printOptionDescription("--min_fidelity <float [0,1]>", "Minimal rule fidelity accepted when generating a rule (default: 1.0)");
  printOptionDescription("--lowest_min_fidelity <float [0,1]>", "Minimal min_fidelity to which we agree to go down during the covering_strategy (default: 0.75)");
  printOptionDescription("--dropout_dim <float [0,1]>", "Probability of dropping a dimension during rule extraction (default: 0.0)");
  printOptionDescription("--dropout_hyp <float [0,1]>", "Probability of dropping a hyperplane during rule extraction (default: 0.0)");
  printOptionDescription("--decision_threshold <float [0,1]>", "The decision threshold used for predictions, you need to specify the index of the positive class if you want to use it");
  printOptionDescription("--positive_class_index <int [0,nb_classes-1]>", "Index of the positive class for the usage of a decision threshold, index starts at 0");
  printOptionDescription("--nb_quant_levels <int [3,inf[>", "Number of stairs in the staircase activation function (default: 50)");
  printOptionDescription("--normalization_file <str>", "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified");
  printOptionDescription("--mus <list<float ]-inf,inf[>>", "Mean or median of each attribute index to be denormalized in the rules");
  printOptionDescription("--sigmas <list<float ]-inf,inf[>>", "Standard deviation of each attribute index to be denormalized in the rules");
  printOptionDescription("--normalization_indices <list<int [0,nb_attributes-1]>>", "Attribute indices to be denormalized in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])");
  printOptionDescription("--nb_threads <int [1,nb_cores]>", "Number of threads used for computing the algorithm, 1=sequential execution (default: 1)");
  printOptionDescription("--seed <int [0,inf[>", "Seed for random number generation, 0=random. Anything else than 0 is an arbitrary seed that can be reused to obtain the same randomly generated sequence and therefore getting same results (default: 0)");

  std::cout << std::endl
            << "----------------------------" << std::endl
            << std::endl;
  std::cout << "Execution example :" << std::endl
            << std::endl;
  std::cout << "fidex.fidexGloRules(\"--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --weights_file weights.wts --nb_attributes 16 --nb_classes 2 --heuristic 1 --global_rules_outfile globalRules.rls --root_folder dimlp/datafiles\")" << std::endl
            << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl
            << std::endl;
}

/**
 * @brief Executes the Fidex algorithm on all provided samples.
 *
 *    This function works in the following way:
 *
 *    - Initializes the number of threads chosen by the user and splits evenly all samples to be covered between them.
 *    - Computes Fidex until a rule is created or until the max failed attempts limit is reached.
 * \par
 *      -> First attempt to generate a rule with a covering greater or equal to 'min_covering' and a fidelity greater or equal to 'min_fidelity'.
 * \par
 *      -> If the attempt fails and the 'covering_strategy' is on, Fidex is computed to find a rule with the maximum possible minimal covering that can be lower than 'min_covering'.
 * \par
 *      -> If all attempts fail, the targeted fidelity is gradually lowered until it succeeds or 'lowest_min_fidelity' is reached.
 * \par
 *      -> Each failed attempt at the lowest minimal fidelity is counted.
 * \par
 *      -> If the max failed attempts limit is reached, the sample is removed from the not covered samples vector as it's considered as unreachable.
 *    - The computed rule is added to the list of rules.
 *
 * @param rules Empty vector of rules to be filled once the function has finished.
 * @param notCoveredSamples Vector with all samples to be covered or ignored if the algorithm isn't able to find one in "maxFailedAttempts" attempts with a minimal covering of 1.
 * @param trainDataset Class containing all usable train data to compute the algorithm.
 * @param p Class containing all user-defined parameters that influence the program execution.
 * @param hyperlocus 2D vector of doubles representing all the possible hyperplanes used to compute the Fidex algorithm.
 */
void generateRules(std::vector<Rule> &rules, std::vector<int> &notCoveredSamples, DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus) {
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  int nbDatas = trainDataset.getNbSamples();
  int minCovering = p.getInt(MIN_COVERING);
  int nbThreadsUsed = p.getInt(NB_THREADS);

  std::vector<int> threadProgress(nbThreadsUsed, 0);

#pragma omp parallel num_threads(nbThreadsUsed)
  {
    Rule rule;
    double t1;
    double t2;
    int cnt = 0;
    bool ruleCreated;
    int localNbProblems = 0;
    std::vector<Rule> localRules;
    std::vector<int>::iterator it;
    int localNbRulesNotFound = 0;
    Hyperspace hyperspace(hyperlocus);
    int threadId = omp_get_thread_num();
    auto fidex = Fidex(trainDataset, p, hyperspace, false);

    std::string consoleFile = "";
    if (p.isStringSet(CONSOLE_FILE)) {
      consoleFile = p.getString(CONSOLE_FILE);
    }

#pragma omp critical
    {
      std::cout << "Threadpop #" << threadId << " initialized, please wait for it to be done." << std::endl;
    }

    t1 = omp_get_wtime();

    auto startTime = std::chrono::steady_clock::now();
#pragma omp for
    for (int idSample = 0; idSample < nbDatas; idSample++) {
      std::vector<int> &trainPreds = trainDataset.getPredictions();
      std::vector<std::vector<double>> &trainData = trainDataset.getDatas();
      std::vector<double> &mainSampleValues = trainData[idSample];
      int mainSamplePred = trainPreds[idSample];
      cnt += 1;

      ruleCreated = fidex.launchFidex(rule, mainSampleValues, mainSamplePred, -1);

      if (!ruleCreated) {
        localNbRulesNotFound += 1;
#pragma omp critical
        {
          // ignore samples that cannot be covered after "maxFailedAttempts" attempts with a minimum covering of 1
          it = find(notCoveredSamples.begin(), notCoveredSamples.end(), idSample);
          if (it != notCoveredSamples.end()) {
            notCoveredSamples.erase(it);
          }
        }
      } else {
        localRules.push_back(rule);
      }

      if (rule.getCoveringSize() < minCovering) {
        localNbProblems += 1;
      }

      // Estimate execution time

      if (idSample == 0 || idSample == 9 || idSample == 99 || idSample == 999 || idSample == 9999) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - startTime;
        double avgTimePerSample = elapsed.count() / (idSample + 1);
        double estimatedTotalTime = (avgTimePerSample * nbDatas) / nbThreadsUsed;

        int hours = static_cast<int>(estimatedTotalTime) / 3600;
        int minutes = (static_cast<int>(estimatedTotalTime) % 3600) / 60;
        int seconds = static_cast<int>(estimatedTotalTime) % 60;

#pragma omp critical
        {
          std::cout << "Estimated total execution time after " << idSample + 1 << " sample(s): "
                    << hours << " hours, " << minutes << " minutes, " << seconds << " seconds." << std::endl;
        }
      }

      // Update thread progress
      if ((cnt % (std::max(1, (nbDatas / nbThreadsUsed) / 100))) == 0 && !p.isStringSet(CONSOLE_FILE)) {
#pragma omp atomic write
        threadProgress[threadId] = (cnt * 100) / (nbDatas / nbThreadsUsed);

#pragma omp critical
        {
          int minProgress = *std::min_element(threadProgress.begin(), threadProgress.end());
          std::cout << "Overall progress: " << minProgress << "%\r";
          std::cout.flush();
        }
      }
    }

    t2 = omp_get_wtime();

    // gathering local data to main process
#pragma omp for ordered
    for (int i = 0; i < nbThreadsUsed; i++) {
#pragma omp ordered
      {
        if (i == threadId) {
          std::cout << "Thread #" << threadId << " ended " << cnt << " iterations in " << (t2 - t1) << " seconds." << std::endl;
          rules.insert(rules.end(), localRules.begin(), localRules.end());
          nbProblems += localNbProblems;
          nbRulesNotFound += localNbRulesNotFound;
        }
      }
    }
  } // end of parallel section

  std::cout << std::endl
            << rules.size() << " rules created." << std::endl
            << "Number of samples with lower covering than " << minCovering << " is " << nbProblems << std::endl
            << "Number of rules not found is " << nbRulesNotFound << std::endl
            << "Fidex rules computed" << std::endl;
}

/**
 * @brief Generates a list of rules covering all training samples using the best and slowest heuristic.
 *
 * Generates rules for each training sample and filters the list of generated rules to remove duplicates and keep the rules that best fit and cover all training samples.
 * This is performed in the following way:
 *
 *  - Rules are generated for each sample.
 *  - Duplicated rules are removed.
 *  - While not all samples are covered by the rules, it checks every rule's covered samples vector and checks how many of the not covered ones are present in it.
 *  - If the rule covers some samples that are not considered covered yet and covers more than the previous best rule, it's saved as the
 *    best rule to be added to the filtered ones.
 *  - When all the rules are checked, the best one is removed from the rules to check, it is saved in the chosen rules vector and the samples
 *    it covers are removed from the ones that are considered not covered.
 *
 * @param trainDataset Class containing all usable data to compute the algorithm.
 * @param p Class containing all user-defined parameters that influence the program execution.
 * @param hyperlocus 2D vector of doubles representing all the possible hyperplanes used to compute the Fidex algorithm.
 * @return A vector containing the computed rules.
 */
std::vector<Rule> heuristic_1(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus) {
  std::vector<Rule> rules;
  std::vector<Rule> chosenRules;
  int nbDatas = trainDataset.getNbSamples();
  std::vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1
  generateRules(rules, notCoveredSamples, trainDataset, p, hyperlocus);

  std::cout << "Computing global ruleset..." << std::endl;

  // remove duplicates
  rules.erase(unique(rules.begin(), rules.end()), rules.end());

  // While there are some not covered samples
  std::vector<int>::iterator ite;
  std::vector<int> currentRuleSamples;

  while (!notCoveredSamples.empty()) {
    Rule bestRule;
    int bestRuleIndex = 0;
    int bestCovering = INT_MAX;
    std::vector<int> remainingSamples;
    std::vector<int> difference(notCoveredSamples.size());

    for (int i = 0; i < rules.size(); i++) {
      currentRuleSamples = rules[i].getCoveredSamples();

      ite = set_difference(notCoveredSamples.begin(),
                           notCoveredSamples.end(),
                           currentRuleSamples.begin(),
                           currentRuleSamples.end(),
                           difference.begin()); // vectors have to be sorted

      difference.resize(ite - difference.begin());

      if (difference.size() < bestCovering) {
        bestRuleIndex = i;
        bestRule = rules[i];
        remainingSamples = difference;
        bestCovering = static_cast<int>(difference.size());
      }
    }

    chosenRules.push_back(bestRule); // add best rule with maximum covering
    notCoveredSamples = remainingSamples;
    rules.erase(rules.begin() + bestRuleIndex); // Remove this rule
  }

  std::cout << chosenRules.size() << " rules selected." << std::endl;

  return chosenRules;
}

/**
 * @brief Generates a list of rules covering all training samples using a faster but less optimal heuristic.
 *
 * Generates rules for each training sample and filters the list of generated rules to remove duplicates and keep the rules that fit and cover all training samples.
 * This is a faster version of the first heuristic 'heuristic_1', the only difference is that it doesn't check every generated rule before choosing the best one.
 * After removing duplicates, it directly selects a rule if one or more not already covered samples are covered by the rule, until all samples are covered.
 *
 * @param trainDataset Class containing all usable data to compute the algorithm.
 * @param p Class containing all user-defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles representing all the possible hyperplanes used to compute the Fidex algorithm.
 * @return A vector containing the computed rules.
 */
std::vector<Rule> heuristic_2(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus) {
  std::vector<Rule> rules;
  std::vector<Rule> chosenRules;
  int nbDatas = trainDataset.getNbSamples();
  std::vector<int> notCoveredSamples(nbDatas);
  iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1

  // getting rules and not covered samples
  generateRules(rules, notCoveredSamples, trainDataset, p, hyperlocus);

  // sort rules by covering
  sort(rules.begin(), rules.end(), [](const Rule &r1, const Rule &r2) {
    return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
  });

  // remove duplicates
  rules.erase(unique(rules.begin(), rules.end()), rules.end());

  // While there are some not covered samples
  int i = 0;
  std::vector<int>::iterator ite;
  std::vector<int> currentRuleSamples;

  while (!notCoveredSamples.empty()) {
    std::vector<int> difference(notCoveredSamples.size());
    currentRuleSamples = rules[i].getCoveredSamples();

    // Delete covered samples
    ite = set_difference(notCoveredSamples.begin(),
                         notCoveredSamples.end(),
                         currentRuleSamples.begin(),
                         currentRuleSamples.end(),
                         difference.begin()); // vectors have to be sorted

    difference.resize(ite - difference.begin());

    // If the rule covers a new sample
    if (difference.size() < notCoveredSamples.size()) {
      chosenRules.push_back(rules[i]); // add best rule with maximum covering
    }
    notCoveredSamples = difference;
    i += 1;
  }

  std::cout << chosenRules.size() << " rules selected." << std::endl;

  return chosenRules;
}

/**
 * @brief Generates a list of rules covering all training samples. This is the fastest heuristic but cannot be parallelized.
 *
 * Fastest heuristic that selects a random sample in the not covered ones, computes its rule, adds it to the computed rules, and removes each sample covered by this rule.
 * When all samples are covered, it removes duplicates.
 *
 * @param trainDataset Class containing all usable data to compute the algorithm.
 * @param p Class containing all user-defined parameters that influences the program execution.
 * @param hyperlocus 2D vector of doubles representing all the possible hyperplanes used to compute the Fidex algorithm.
 * @return A vector containing the computed rules.
 */
std::vector<Rule> heuristic_3(DataSetFid &trainDataset, Parameters &p, const std::vector<std::vector<double>> &hyperlocus) {
  Rule rule;
  int idSample;
  bool ruleCreated;
  int nbProblems = 0;
  int nbRulesNotFound = 0;
  std::vector<Rule> chosenRules;
  int seed = p.getInt(SEED);
  std::vector<int> chosenRuleSamples;
  Hyperspace hyperspace(hyperlocus);
  int minNbCover = p.getInt(MIN_COVERING);
  auto nbDatas = static_cast<int>(trainDataset.getDatas().size());
  std::vector<int> notCoveredSamples(nbDatas);
  auto fidex = Fidex(trainDataset, p, hyperspace, false);
  std::string consoleFile = "";
  if (p.isStringSet(CONSOLE_FILE)) {
    consoleFile = p.getString(CONSOLE_FILE);
  }

  if (seed == 0) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto seedValue = currentTime.time_since_epoch().count();
    seed = static_cast<int>(seedValue);
  }

  std::mt19937 gen(seed);

  iota(begin(notCoveredSamples), end(notCoveredSamples), 0);      // Vector from 0 to nbDatas-1
  shuffle(begin(notCoveredSamples), end(notCoveredSamples), gen); //  Sort data randomly

  std::cout << "Computing rules..." << std::endl
            << std::endl;

  // While there are some not covered samples
  while (!notCoveredSamples.empty()) {

    if (int(nbDatas / 100) != 0 && (nbDatas - notCoveredSamples.size()) % int(nbDatas / 100) == 0 && consoleFile.empty()) {
      std::cout << "Processing: " << int((double(nbDatas - notCoveredSamples.size()) / nbDatas) * 100) << "%\r";
      std::cout.flush();
    }

    idSample = notCoveredSamples[0];
    std::vector<int> &trainPreds = trainDataset.getPredictions();
    std::vector<std::vector<double>> &trainData = trainDataset.getDatas();
    std::vector<double> &mainSampleValues = trainData[idSample];
    int mainSamplePred = trainPreds[idSample];

    ruleCreated = fidex.launchFidex(rule, mainSampleValues, mainSamplePred, -1);

    if (!ruleCreated) {
      nbRulesNotFound += 1;
    }
    if (rule.getCoveringSize() < minNbCover) {
      nbProblems += 1;
    }

    if (ruleCreated) {
      chosenRules.push_back(rule); // We add the new rule
      chosenRuleSamples = rule.getCoveredSamples();

      // Remove samples covered by this rule
      notCoveredSamples.erase(
          remove_if(notCoveredSamples.begin(), notCoveredSamples.end(), [&chosenRuleSamples](int sample) {
            return find(chosenRuleSamples.begin(), chosenRuleSamples.end(), sample) != chosenRuleSamples.end();
            // find index of coveredSamples which is "sample" ("sample" is element of notCoveredSamples), find returns last if "sample" not found
            // -> Removes "sample" if it appears on coveredSamples (found before the end of coveredSamples)
          }),
          notCoveredSamples.end());
    } else {
      notCoveredSamples.erase(notCoveredSamples.begin());
    }
  }

  std::cout << std::endl;

  // remove duplicates
  chosenRules.erase(unique(chosenRules.begin(), chosenRules.end()), chosenRules.end());

  std::cout << "Number of samples with lower covering than " << minNbCover << " : " << nbProblems << std::endl
            << "Number of rules not found : " << nbRulesNotFound << std::endl
            << chosenRules.size() << " rules selected." << std::endl;

  return chosenRules;
}

/**
 * @brief Sets default hyperparameters and checks the logic and validity of the parameters of fidexGloRules.
 *
 * @param p Reference to the Parameters object containing all hyperparameters.
 */
void checkRulesParametersLogicValues(Parameters &p) {
  // setting default values
  p.setDefaultNbQuantLevels();
  p.setDefaultDecisionThreshold();
  p.setDefaultFidex();
  p.setDefaultInt(NB_THREADS, 1);

  // this sections check if values comply with program logic

  // asserting mandatory parameters
  p.assertIntExists(NB_ATTRIBUTES);
  p.assertIntExists(NB_CLASSES);
  p.assertStringExists(TRAIN_DATA_FILE);
  p.assertStringExists(TRAIN_PRED_FILE);
  p.assertStringExists(GLOBAL_RULES_OUTFILE);
  p.assertIntExists(HEURISTIC);

  // verifying logic between parameters, values range and so on...
  p.checkParametersCommon();
  p.checkParametersFidex();
  p.checkParametersDecisionThreshold();
  p.checkParametersNormalization();

  if (p.getInt(NB_THREADS) < 1 || p.getInt(NB_THREADS) > omp_get_max_threads()) {
    throw CommandArgumentException("Error : Number of threads must be between 1 and the number of CPU cores on your machine (which is " + std::to_string(omp_get_max_threads()) + ")");
  }

  if (!(p.getInt(HEURISTIC) > 0 && p.getInt(HEURISTIC) < 4)) {
    throw CommandArgumentException("Error : Heuristic must be 1, 2, or 3.");
  }
}

/**
 * @brief Executes the FidexGloRules algorithm with specified parameters to extract a ruleset from a dataset of training samples.
 *
 * FidexGloRules computes a rule for each training sample by calling Fidex. Fidex is based on the training samples and hyperlocus
 * and directed by the given parameters, including dropout and the maximum number of iterations allowed.
 * It works by identifying hyperplanes in the feature space that discriminate between different
 * classes of samples and constructing a rule for the training sample based on these hyperplanes covering this sample
 * and as many other training samples as possible. Then, a heuristic is used to remove the duplicated and unnecessary rules.
 *
 * The %Fidex algorithm is computed until a rule is created or until the max failed attempts limit is reached.<br>
 * - First attempt to generate a rule with a covering greater or equal to 'min_covering' and a fidelity greater or equal to 'min_fidelity'.<br>
 * - If the attempt fails and the 'covering_strategy' is on, Fidex is computed to find a rule with the maximum possible minimal covering that can be lower than 'min_covering'.<br>
 * - If all attempts fail, the targeted fidelity is gradually lowered until it succeeds or 'lowest_min_fidelity' is reached.<br>
 * - Each failed attempt at the lowest minimal fidelity is counted.<br>
 * - If the max failed attempts limit is reached, then the rule couldn't be computed for this sample.
 *
 * Notes:
 * - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
 * - It's mandatory to specify the number of attributes and classes in the data, as well as the train dataset.
 * - True train class labels must be provided, either within the data file or separately through a class file.
 * - Train predictions are also mandatory.
 * - A heuristic needs to be chosen; the first one should always be chosen if the execution time permits it.
 * - The weights file or rules_file (when training with decision trees) obtained from the model training must be provided.
 * - The path of the file containing the computed rules must be provided. It can be generated as a JSON if a JSON extension is specified.
 * - Normalization parameters can be specified to denormalize the rules if data were normalized beforehand.
 * - Parameters can be defined directly via the command line or through a JSON configuration file.
 * - Providing no command-line arguments or using <tt>-h/-\-help</tt> displays usage instructions, detailing both required and optional parameters for user guidance.
 *
 * Outputs:
 * - global_rules_outfile : File containing the obtained global explanation rule for train set. It's generated as a JSON if a JSON extension is specified.
 * - console_file : If specified, contains the console output.
 *
 * File formats:
 * - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Only attributes (floats).
 *   2. Attributes (floats) followed by an integer class ID.
 *   3. Attributes (floats) followed by one-hot encoded class.
 * - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons, or commas. Supported formats:
 *   1. Integer class ID.
 *   2. One-hot encoded class.
 * - **Prediction files**: These files should contain one line per data sample, each line consisting of a series of numerical values separated
 *   by a space, a comma (CSV), a semicolon (;), or a tab representing the prediction scores for each class.
 * - **Weights file**: This file should be obtained by training with Dimlp, SVM, MLP, or a CNN from dimlpfidex because an additional special Dimlp layer is needed.
 *   If the training was made with more than one network, each network is separated by a "Network <id>" marker.
 *   The first row represents bias values of the Dimlp layer and the second row are values of the weight matrix between the previous layer and the Dimlp layer.
 *   Each value is separated by a space. As an example, if the layers are of size 4, the biases are: b1 b2 b3 b4 and the weights are w1 w2 w3 w4.
 * - **Rule file**: This file should be obtained directly by training with Random Forests or Gradient Boosting from dimlpfidex because rules need to be extracted from the trees.
 * - **Attributes file**: Each line corresponds to one attribute, each attribute must be specified. Classes can be specified
 *   after the attributes but are not mandatory. Each attribute or class must be in one word without spaces (you can use _ to replace a space).
 *   The order is important as the first attribute/class name will represent the first attribute/class in the dataset.
 * - **Normalization file**: Each line contains the mean/median and standard deviation for an attribute.<br>
 *   Format: '2 : original mean: 0.8307, original std: 0.0425'<br>
 *   Attribute indices (index 2 here) can be replaced with attribute names, then an attribute file is required.
 *
 * Example of how to call the function:
 * @par
 * <tt>from dimlpfidex import fidex</tt>
 * @par
 * <tt>fidex.fidexGloRules('-\-train_data_file datanormTrain.txt -\-train_pred_file predTrain.out -\-train_class_file dataclass2Train.txt -\-weights_file weights.wts -\-nb_attributes 16 -\-nb_classes 2 -\-heuristic 1 -\-global_rules_outfile globalRules.rls -\-root_folder dimlp/datafiles')</tt>
 *
 * @param command A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input. This includes file paths, Fidex parameters, and options for output.
 * @return Returns 0 for successful execution, -1 for errors encountered during the process.
 */
int fidexGloRules(const std::string &command) {
  // Save buffer where we output results
  std::ofstream ofs;
  std::streambuf *cout_buff = std::cout.rdbuf(); // Save old buf

  try {
    // Parsing the command
    std::vector<std::string> commandList = {"fidexGloRules"};
    std::string s;
    std::stringstream ss(command);

    while (ss >> s) {
      commandList.push_back(s);
    }

    size_t nbParam = commandList.size();
    if (nbParam < 2 || commandList[1] == "-h" || commandList[1] == "--help") {
      showRulesParams();
      return -1;
    }

    // Import parameters
    std::unique_ptr<Parameters> params;
    std::vector<ParameterCode> validParams = {TRAIN_DATA_FILE, TRAIN_PRED_FILE, TRAIN_CLASS_FILE, WEIGHTS_FILE, RULES_FILE, GLOBAL_RULES_OUTFILE,
                                              HEURISTIC, NB_ATTRIBUTES, NB_CLASSES, ROOT_FOLDER, ATTRIBUTES_FILE, CONSOLE_FILE,
                                              MAX_ITERATIONS, MIN_COVERING, DROPOUT_DIM, DROPOUT_HYP, MAX_FAILED_ATTEMPTS, NB_QUANT_LEVELS,
                                              DECISION_THRESHOLD, POSITIVE_CLASS_INDEX, NORMALIZATION_FILE, MUS, SIGMAS, NORMALIZATION_INDICES,
                                              NB_THREADS, COVERING_STRATEGY, MIN_FIDELITY, LOWEST_MIN_FIDELITY, SEED};
    if (commandList[1].compare("--json_config_file") == 0) {
      if (commandList.size() < 3) {
        throw CommandArgumentException("JSON config file name/path is missing");
      } else if (commandList.size() > 3) {
        throw CommandArgumentException("Option " + commandList[1] + " has to be the only option in the command if specified.");
      }
      try {
        params = std::unique_ptr<Parameters>(new Parameters(commandList[2], validParams));
      } catch (const std::out_of_range &) {
        throw CommandArgumentException("Some value inside your JSON config file '" + commandList[2] + "' is out of range.\n(Probably due to a too large or too tiny numeric value).");
      } catch (const std::exception &e) {
        std::string msg(e.what());
        throw CommandArgumentException("Unknown JSON config file error: " + msg);
      }
    } else {
      // Read parameters from CLI
      params = std::unique_ptr<Parameters>(new Parameters(commandList, validParams));
    }

    // getting all program arguments from CLI
    checkRulesParametersLogicValues(*params);

    // Get console results to file
    if (params->isStringSet(CONSOLE_FILE)) {
      ofs.open(params->getString(CONSOLE_FILE));
      std::cout.rdbuf(ofs.rdbuf()); // redirect cout to file
    }

    // Show chosen parameters
    std::cout << *params;

    // Import files
    std::cout << "Importing files..." << std::endl;

    std::unique_ptr<DataSetFid> trainDatas;
    if (!params->isStringSet(TRAIN_CLASS_FILE)) {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE),
                                      params->getString(TRAIN_PRED_FILE),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(POSITIVE_CLASS_INDEX)));

      if (!trainDatas->getHasClasses()) {
        throw CommandArgumentException("The train true classes file has to be given with option --train_class_file or classes have to be given in the train data file.");
      }
    } else {
      trainDatas.reset(new DataSetFid("trainDatas from FidexGloRules",
                                      params->getString(TRAIN_DATA_FILE),
                                      params->getString(TRAIN_PRED_FILE),
                                      params->getInt(NB_ATTRIBUTES),
                                      params->getInt(NB_CLASSES),
                                      params->getFloat(DECISION_THRESHOLD),
                                      params->getInt(POSITIVE_CLASS_INDEX),
                                      params->getString(TRAIN_CLASS_FILE)));
    }

    int nbDatas = trainDatas->getNbSamples();

    if (params->getInt(MIN_COVERING) > nbDatas) {
      throw CommandArgumentException("Error : invalid type for parameter --min_covering, strictly positive integer smaller or equal than the number of train data samples requested.");
    }

    // Get attributes
    std::vector<std::string> attributeNames;
    std::vector<std::string> classNames;
    bool hasClassNames = false;

    if (params->isStringSet(ATTRIBUTES_FILE)) {
      trainDatas->setAttributes(params->getString(ATTRIBUTES_FILE), params->getInt(NB_ATTRIBUTES), params->getInt(NB_CLASSES));
      attributeNames = trainDatas->getAttributeNames();
      hasClassNames = trainDatas->getHasClassNames();
      if (hasClassNames) {
        classNames = trainDatas->getClassNames();
      }
    }

    std::vector<int> normalizationIndices;
    std::vector<double> mus;
    std::vector<double> sigmas;

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

    std::cout << "Files imported" << std::endl
              << std::endl;

    // compute hyperspace

    std::cout << "Creation of hyperspace..." << std::endl;

    std::vector<std::vector<double>> matHypLocus;
    std::string weightsFile;
    if (params->isStringSet(WEIGHTS_FILE)) {
      weightsFile = params->getString(WEIGHTS_FILE);
    }
    std::string attributesFile;
    if (params->isStringSet(ATTRIBUTES_FILE)) {
      attributesFile = params->getString(ATTRIBUTES_FILE);
    }
    std::string inputRulesFile;
    if (params->isStringSet(RULES_FILE)) {
      inputRulesFile = params->getString(RULES_FILE);
    }
    int nbQuantLevels = params->getInt(NB_QUANT_LEVELS);
    int nbAttributes = params->getInt(NB_ATTRIBUTES);
    float hiKnot = params->getFloat(HI_KNOT);

    if (params->isStringSet(WEIGHTS_FILE)) {
      matHypLocus = calcHypLocus(weightsFile, nbQuantLevels, hiKnot); // Get hyperlocus
    } else {
      matHypLocus = calcHypLocus(inputRulesFile, *trainDatas);
    }

    // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    auto nbIn = static_cast<int>(matHypLocus.size());

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributes != 0) {
      throw InternalError("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributes - " + std::to_string(nbAttributes));
    }

    std::cout << "Hyperspace created." << std::endl
              << std::endl;

    // Samples not yet covered by any rules
    std::vector<int> notCoveredSamples(nbDatas);
    iota(begin(notCoveredSamples), end(notCoveredSamples), 0); // Vector from 0 to nbDatas-1
    std::vector<Rule> chosenRules;                             // antecedents, cover vector, class, rule accuracy, rule confidence

    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // Initialize random number generator

    const auto start = std::chrono::high_resolution_clock::now();
    int heuristic = params->getInt(HEURISTIC);
    auto trainDataset = *trainDatas.get();
    auto parameters = *params.get();
    std::vector<Rule> generatedRules;

    std::cout << "Computing fidex rules..." << std::endl
              << std::endl;

    switch (heuristic) {
    case 1:
      generatedRules = heuristic_1(trainDataset, parameters, matHypLocus);
      break;

    case 2:
      generatedRules = heuristic_2(trainDataset, parameters, matHypLocus);
      break;

    case 3:
      generatedRules = heuristic_3(trainDataset, parameters, matHypLocus);
      break;

    default:
      std::cout << "Heuristic not recognised." << std::endl;
      return -1;
      break;
    }

    std::cout << "Global ruleset Computed." << std::endl;

    const auto end_h = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> diff_h = end_h - start;

    std::cout << "\nheuristic #" << heuristic << " ended in "
              << diff_h.count() << " sec\n"
              << std::endl;

    std::cout << "Rules extraction..."
              << std::endl;

    sort(generatedRules.begin(), generatedRules.end(), [](const Rule &r1, const Rule &r2) {
      return r1.getCoveredSamples().size() > r2.getCoveredSamples().size();
    });

    std::tuple<double, double> stats = writeRulesFile(params->getString(GLOBAL_RULES_OUTFILE),
                                                      generatedRules,
                                                      attributeNames,
                                                      classNames,
                                                      params->getFloat(DECISION_THRESHOLD),
                                                      params->getInt(POSITIVE_CLASS_INDEX));

    std::cout << "Mean covering size per rule : " << std::get<0>(stats) << std::endl;
    std::cout << "Mean number of antecedents per rule : " << std::get<1>(stats) << std::endl;

    const auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "\nFull execution time = " << diff.count() << " sec" << std::endl;

    std::cout.rdbuf(cout_buff); // reset to standard output again

  } catch (const ErrorHandler &e) {
    std::cout.rdbuf(cout_buff); // reset to standard output again
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
