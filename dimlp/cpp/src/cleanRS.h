#ifndef CLEANRS_H
#define CLEANRS_H

#include <iostream>

#include "bpNN.h"
#include "rulePro.h"

////////////////////////////////////////////////////////////////////////

/**
 * @brief Class CleanRuleStruct is used to simplify and manage the rules derived from the rule extraction process.
 *
 * This class provides functionalities to:
 * - Simplify the rule set using else-clauses.
 * - Prune and expand rules to ensure they are both minimal and effective.
 * - Calculate and track the accuracy and coverage of the rules on different datasets (training, testing, validation).
 * - Handle the classification and representation of rules.
 * - Ensure the fidelity of the rules, meaning that the rules correctly cover all examples.
 */
class CleanRuleStruct {
  DataSet All;        ///< All data.
  DataSet Train;      ///< Training data.
  DataSet TrainClass; ///< Training class labels.
  DataSet Test;       ///< Testing data.
  DataSet TestClass;  ///< Testing class labels.
  DataSet Valid;      ///< Validation data.
  DataSet ValidClass; ///< Validation class labels.

  int NbRules;      ///< Number of rules.
  int NbAnt;        ///< Number of antecedents.
  int NbOut;        ///< Number of output classes.
  int DefaultClass; ///< Default class for classification.

  /**
   * @brief Structure representing an antecedent of a rule.
   */
  struct AssocAnte {
    int Var;         ///< Variable index.
    float Val;       ///< Threshold value.
    char Rel;        ///< Relation ('<' or '>').
    std::string Str; ///< String representation of the antecedent.
  };

  /**
   * @brief Structure representing a rule and its associated information.
   */
  struct CleanRule {
    std::vector<AssocAnte> SevAnt; ///< List of antecedents.
    int NbAnt;                     ///< Number of antecedents.
    int Classification;            ///< Class assigned by the rule.
    std::string StrClass;          ///< String representation of the class.
    int NbAllCarried;              ///< Number of examples carried by the rule.
    int NbCarriedTrain;            ///< Number of training examples carried.
    int NbCarriedTest;             ///< Number of testing examples carried.
    int NbCarriedValid;            ///< Number of validation examples carried.
    int NbCorrectTrain;            ///< Number of correct training examples.
    int NbCorrectTest;             ///< Number of correct testing examples.
    int NbCorrectValid;            ///< Number of correct validation examples.
    int NbWrongTrain;              ///< Number of wrong training examples.
    int NbWrongTest;               ///< Number of wrong testing examples.
    int NbWrongValid;              ///< Number of wrong validation examples.
    float AccuracyTrain;           ///< Accuracy on training set.
    float AccuracyTest;            ///< Accuracy on testing set.
    float AccuracyValid;           ///< Accuracy on validation set.
    int Flag;                      ///< Flag for internal processing.
  };

  std::shared_ptr<BpNN> Bpnn;             ///< Neural network pointer.
  std::shared_ptr<RuleProcessing> Pruned; ///< Pruned rules pointer.
  float *Out;                             ///< Output of the neural network.

  std::vector<std::shared_ptr<CleanRule>> Clean; ///< Vector of clean rules.
  std::vector<int> WrongTrain;                   ///< Vector of wrong training examples.
  std::vector<int> WrongTest;                    ///< Vector of wrong testing examples.
  std::vector<int> WrongValid;                   ///< Vector of wrong validation examples.
  std::vector<int> ClAllNet;                     ///< Network class for all examples.

  std::vector<int> ClassPatNetTrain; ///< Network class for training examples.
  std::vector<int> ClassPatNetTest;  ///< Network class for testing examples.
  std::vector<int> ClassPatNetValid; ///< Network class for validation examples.

  // ------------------------------------------------------------------------

  /**
   * @brief Resets the flag for all rules to 0.
   */
  void ResetFlag() const;

  /**
   * @brief Sets the flag for all rules to 1.
   */
  void SetFlagToOne() const;

  /**
   * @brief Counts the number of rules with a specific flag.
   */
  int CountFlaggedRules(int flag) const;

  /**
   * @brief Counts the number of antecedents with a specific flag.
   */
  int CountFlaggedAnt(int flag) const;

  /**
   * @brief Counts the number of patterns carried by rules with a specific flag.
   */
  int CountCarPatByFlags(int flag) const;

  /**
   * @brief Computes the average number of patterns carried by rules with a specific flag.
   *
   * @param flag The flag to compute the average for.
   * @return The average number of patterns carried by rules with the specified flag.
   */
  float ComputeAvgCar(int flag) const;

  /**
   * @brief Computes the average number of antecedents in rules with a specific flag.
   */
  float ComputeAvgAnt(int flag) const;

  /**
   * @brief Finds the minimum variable index in the antecedents.
   */
  int FindMinOnAnt(const AssocAnte *ant, int nbAnt) const;

  /**
   * @brief Sorts the antecedents of a rule in ascending order.
   */
  void SortAnt(AssocAnte *ant, int nbAnt, int indRule) const;

  /**
   * @brief Finds the index of the rule with the maximum number of carried patterns.
   */
  int FindMaxOnRules(int start) const;

  /**
   * @brief Finds the index of the rule with the minimum number of antecedents.
   */
  int FindMinAntOnRules(int start) const;

  /**
   * @brief Sorts the rules based on the minimum number of antecedents.
   */
  void SortRules(int indMin);

  /**
   * @brief Computes the global accuracy on a dataset.
   */
  float GlobalAcc(DataSet &data, int *vecWrong, int nbEl) const;

  /**
   * @brief Computes the accuracy on a dataset.
   */
  void ComputeAcc(
      std::shared_ptr<StringInt> carried,
      int *indWrong,
      DataSet &data,
      DataSet &dataClass,
      int *nbPat,
      int *correct,
      int *wrong,
      float *acc) const;

  /**
   * @brief Sets various information fields for a given rule, including classification, coverage, and accuracy metrics.
   */
  void SetSevInfo(std::shared_ptr<DimlpRule> rule, int indClean);

  /**
   * @brief Creates vectors to track incorrectly classified examples.
   */
  void CreateWrongVect();

  /**
   * @brief Initializes and sets up the internal data structures for processing and managing rules.
   */
  void CreateStructures();

  /**
   * @brief Resets specific fields in the CleanRuleStruct.
   */
  void ResetSomeFields() const;

  /**
   * @brief Checks if a given example is covered by a specified rule.
   */
  int IsExampleCarried(DataSet &data, int index, std::shared_ptr<CleanRule> rule) const;

  /**
   * @brief Ensures 100% fidelity of the rules, meaning all examples are correctly classified by the rules.
   */
  int Fidelity100();

  /**
   * @brief Ensures 100% fidelity of the rules including the default rule.
   */
  int Fidelity100Def();

  /**
   * @brief Applies the else-clause representation to the entire dataset.
   */
  void ElseRepAll();

  /**
   * @brief Applies the else-clause representation to the training dataset.
   */
  void ElseRepTrain();

  /**
   * @brief Applies the else-clause representation to the testing dataset.
   */
  void ElseRepTest();

  /**
   * @brief Applies the else-clause representation to the validation dataset.
   */
  void ElseRepValid();

  /**
   * @brief Marks an antecedent as deleted by setting its variable index to -1.
   *
   * @param oneAnt The antecedent to be marked as deleted.
   */
  void RemAnt(AssocAnte *oneAnt) const { oneAnt->Var = -1; }

  /**
   * @brief Checks if an antecedent is marked as deleted.
   *
   * @param oneAnt The antecedent to be checked.
   * @return 1 if the antecedent is deleted, 0 otherwise.
   */
  int IsAntDeleted(const AssocAnte *oneAnt) const { return ((oneAnt->Var == -1) ? 1 : 0); }

  /**
   * @brief Randomly prunes antecedents from the rules while maintaining fidelity.
   */
  int RandomPruneAnt();

  /**
   * @brief Computes the sum of all examples covered by the rules.
   */
  int SumCarried() const;

  /**
   * @brief Removes a rule from the list of rules.
   */
  void RemRule(int r) const;

  /**
   * @brief Prunes rules to remove redundant or unnecessary antecedents.
   */
  int PruneRule();

  /**
   * @brief Expands the rules by exploring additional thresholds for antecedents.
   */
  int OrderedExpand(ThresDescr *descr);

  /**
   * @brief Checks the number of active antecedents in a rule.
   */
  int CheckAnt(int indOld) const;

  /**
   * @brief Creates a new list of clean rules, excluding deleted antecedents.
   */
  void CreateNewClean();

  /**
   * @brief Sets the default rule based on the most frequent class of remaining examples.
   */
  int SetDefRule();

  /**
   * @brief Sets the class pattern network for each dataset.
   */
  void SetClassPatNet();

  /**
   * @brief Writes the number of rules and antecedents to the output.
   */
  void WriteNumb() const;

  /**
   * @brief Converts an integer to a string.
   */
  std::string ItoA(int n) const;

  /**
   * @brief Determines the default rule based on the classification of remaining examples.
   */
  int DefDef() const;

  /**
   * @brief Computes some metrics of rules on the testing set : the accuracy, the fidelity, the accuracy when rules and network agree and the default rule activations rate.
   */
  void UnordAccWithDef(std::ostream &ruleFile);

  /**
   * @brief Computes some metrics of rules on the testing set : the accuracy, the fidelity, the accuracy when rules and network agree and the default rule activations rate (alternative method).
   */
  void UnordAccWithDef2(std::ostream &ruleFile);

  // ------------------------------------------------------------------------

public:
  /**
   * @brief Writes the rules to a file.
   */
  void WriteRules(
      int def,
      std::ostream &ruleFile,
      const std::vector<double> &mus = std::vector<double>(),
      const std::vector<double> &sigmas = std::vector<double>(),
      const std::vector<int> &normalizationIndices = std::vector<int>());

  /**
   * @brief Represents the rules using else-clauses for various datasets.
   */
  void ElseRepresentation();

  /**
   * @brief Simplifies the rules using else-clauses.
   */
  void SimplifyElse(ThresDescr *descr);

  /**
   * @brief Deletes all rules.
   */
  void Del() { NbRules = 0; };

  /**
   * @brief Sets the attribute names for the rules.
   */
  void SetAttr(std::vector<std::string> listAttr) const;

  /**
   * @brief Sets the attribute names for the rules using attribute ids.
   */
  void SetAttr();

  /**
   * @brief Sets the class names for the rules.
   */
  void SetStrClass(std::vector<std::string> listClass, int def) const;

  /**
   * @brief Sets the class names for the rules using class ids.
   */
  void SetStrClass(int def);

  /**
   * @brief Constructs a CleanRuleStruct object.
   */
  CleanRuleStruct(
      const DataSet &all,
      const DataSet &train,
      const DataSet &trainClass,
      const DataSet &valid,
      const DataSet &validClass,
      const DataSet &test,
      const DataSet &testClass,
      std::shared_ptr<RuleProcessing> processed,
      std::shared_ptr<BpNN> bpNn,
      float *out,
      int nbOut);

  ////////////////////////////////////////////////////////////////////////
};

#endif // CLEANRS_H
