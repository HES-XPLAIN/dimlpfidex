#ifndef RULEPRO_H
#define RULEPRO_H

#include "dataSet.h"
#include "thresD.h"

#include <vector>

////////////////////////////////////////////////////////////////////////

/**
 * @brief Class to manage and manipulate rules extracted from the Dimlp algorithm.
 *
 * This class provides functionalities to count, prune, and enlarge rules based on certain criteria and thresholds.
 */
class RuleProcessing

{
  int NbRules = 0; ///< Number of rules.
  int NbVar;       ///< Number of variables.
  int NbHyp;       ///< Number of hyperplanes.

  DataSet Data;                      ///< Dataset containing the data for rule processing.
  std::vector<int> ClassPatNet;      ///< Vector containing the class patterns of the network.
  std::vector<int> TabRules;         ///< Vector containing the rules.
  std::shared_ptr<ThresDescr> Descr; ///< Pointer to the threshold descriptor.

  StringInt Gain;    ///< Gain values for pruning rules.
  StringInt RuleInd; ///< Rule indices for pruning.
  StringInt AntInd;  ///< Antecedent indices for pruning.

  /**
   * @brief Structure to store a saved rule along with its carried examples and a pointer to the next saved rule.
   */
  struct Saved {
    std::shared_ptr<DimlpRule> OneRule; ///< Pointer to a rule.
    std::shared_ptr<StringInt> Carried; ///< Pointer to carried examples.
    std::shared_ptr<Saved> Next;        ///< Pointer to the next saved rule.
  };

  std::shared_ptr<Saved> First;   ///< Pointer to the first saved rule.
  std::shared_ptr<Saved> Current; ///< Pointer to the current rule.
  std::shared_ptr<Saved> Memory;  ///< Pointer to the the rule kept in memory.

  //----------------------------------------------------------------

  /**
   * @brief Finds the maximum value in the table.
   */
  int Max(const std::vector<int> &tab, int nbRules) const;

  /**
   * @brief Finds the minimum value in the table.
   */
  int Min(const std::vector<int> &tab, int nbRules) const;

  /**
   * @brief Sets the count of antecedents for each rule in the rule list.
   */
  void SetCountAntRules();

  /**
   * @brief Sets the carried field for each rule in the rule list.
   */
  void SetCarriedField();

  /**
   * @brief Deletes the carried lists for each rule in the rule list.
   */
  void DelListCar();

  /**
   * @brief Checks if all examples are carried by the rule list if one of the rule is dropped.
   */
  int CheckAllCarried(int toDrop);

  /**
   * @brief Removes the current rule from the rule list.
   */
  void RemCurrentRule() const;

  /**
   * @brief Removes a rule from the rule list if it can be safely pruned.
   */
  int GoToSavedAndRemRule(int indPrune);

  /**
   * @brief Goes to a rule and removes its antecedent.
   */
  void GoToRuleAndRemAnt(int indPrune);

  /**
   * @brief Removes several thresholds from a rule.
   */
  void RemSevThres(std::shared_ptr<DimlpRule> r) const;

  /**
   * @brief Computes the gain for pruning rules.
   */
  void ComputeGain();

  /**
   * @brief Prunes the rules by selecting the rules with minimum carried examples.
   */
  void RulePruneByMinCar();

  /**
   * @brief Prunes the rules by iteratively removing the rule with the most antecedents.
   */
  void FastRulePrune(int nbIt);

  /**
   * @brief Tries to enlarge the thresholds for the rules in the rule list.
   */
  int TryEnlargedThres();

  //----------------------------------------------------------------

public:
  /**
   * @brief Checks if a rule is empty.
   */
  int IsRuleEmpty(std::shared_ptr<DimlpRule> rule) const;

  /**
   * @brief Gets the number of rules.
   *
   * @return The number of rules.
   */
  int GetNbRules() const { return NbRules; }

  /**
   * @brief Goes to the beginning of the rule list.
   */
  void GoToBeg() { Current = First; }

  /**
   * @brief Goes to the next rule in the list.
   */
  void GoToNext() { Current = Current->Next; }

  /**
   * @brief Gets the current rule.
   *
   * @return Pointer to the current rule.
   */
  std::shared_ptr<DimlpRule> GetRule() const { return Current->OneRule; }

  /**
   * @brief Saves the current state of the rule list.
   */
  void Save() { Memory = Current; }

  /**
   * @brief Goes to the previously saved state of the rule list.
   */
  void Previous() { Current = Memory; }

  /**
   * @brief Counts the total number of antecedents in all the rules.
   */
  int CountAnt();

  /**
   * @brief Counts the number of non-empty rules.
   */
  int CountRules();

  /**
   * @brief Cleans the rule list by removing unnecessary thresholds.
   */
  void Clean();

  /**
   * @brief Mixes and prunes the rules.
   */
  void MixPrune();

  /**
   * @brief Enlarges and prunes the rules.
   */
  void EnlargeAndPrune();

  /**
   * @brief Inserts a rule into the rule list.
   */
  void Insert(std::shared_ptr<DimlpRule> r);

  /**
   * @brief Deletes all rules from the rule list.
   */
  void Del();

  /**
   * @brief Constructor for the RuleProcessing class.
   */
  RuleProcessing(
      int nbVar,
      int nbHyp,
      DataSet data,
      const std::vector<int> &classPatNet,
      std::shared_ptr<ThresDescr> descr);

  /**
   * @brief Default constructor for the RuleProcessing class.
   */
  RuleProcessing() = default;
};

#endif // RULEPRO_H
