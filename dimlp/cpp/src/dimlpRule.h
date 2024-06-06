#ifndef DIMLPRULE_H
#define DIMLPRULE_H

#include <memory>

////////////////////////////////////////////////////////////////////////

/**
 * @brief The DimlpRule class represents a rule with antecedents used in the Dimlp algorithm.
 *
 * This class allows the creation, manipulation, and traversal of a rule's antecedents.
 * An antecedent consists of a variable, a value, and a relational operator.
 */
class DimlpRule {

  /**
   * @brief The Antecedent struct represents a single antecedent in the rule.
   */
  struct Antecedent {
    int Var;                          ///< The variable index.
    float Val;                        ///< The threshold value.
    char Rel;                         ///< The relational operator.
    std::shared_ptr<Antecedent> Next; ///< Pointer to the next antecedent in the rule.
  };

  int NbAnt = 0; ///< Number of antecedents in the rule.

  std::shared_ptr<Antecedent> FirstAnt; ///< Pointer to the first antecedent.
  std::shared_ptr<Antecedent> LastAnt;  ///< Pointer to the last antecedent.
  std::shared_ptr<Antecedent> PtrAnt;   ///< Pointer to the current antecedent for traversal.
  std::shared_ptr<Antecedent> Memory;   ///< Pointer to a saved antecedent for traversal.

  //----------------------------------------------------------------

public:
  /**
   * @brief Get the number of antecedents.
   * @return The number of antecedents.
   */
  int GetNbAnt() const { return NbAnt; }

  /**
   * @brief Set the traversal pointer to the beginning of the antecedent list.
   */
  void GoToBeg() { PtrAnt = FirstAnt; }

  /**
   * @brief Move the traversal pointer to the next antecedent.
   */
  void GoToNext() { PtrAnt = PtrAnt->Next; }

  /**
   * @brief Get the variable of the current antecedent.
   * @return The variable index.
   */
  int GetVar() const { return PtrAnt->Var; }

  /**
   * @brief Get the value of the current antecedent.
   * @return The threshold value.
   */
  float GetVal() const { return PtrAnt->Val; }

  /**
   * @brief Get the relational operator of the current antecedent.
   * @return The relational operator.
   */
  char GetRel() const { return PtrAnt->Rel; }

  /**
   * @brief Mark the current antecedent as removed.
   */
  void RemAnt() const { PtrAnt->Var = -1; }

  /**
   * @brief Set the variable of the current antecedent.
   * @param var The variable index.
   */
  void SetAnt(int var) const { PtrAnt->Var = var; }

  /**
   * @brief Set the value of the current antecedent.
   * @param val The threshold value.
   */
  void SetThres(float val) const { PtrAnt->Val = val; }

  /**
   * @brief Check if the current antecedent is marked as removed.
   * @return 1 if the antecedent is removed, otherwise 0.
   */
  int IsAntDeleted() const { return ((PtrAnt->Var == -1) ? 1 : 0); }

  /**
   * @brief Save the current traversal pointer.
   */
  void SavePtrAnt() { Memory = PtrAnt; }

  /**
   * @brief Restore the traversal pointer to the saved position.
   */
  void PrevPtrAnt() { PtrAnt = Memory; }

  /**
   * @brief Get the number of non-deleted antecedents in the rule.
   */
  int GetNbAntWithout();

  /**
   * @brief Mark all antecedents in the rule as removed.
   */
  void DeleteRule();

  /**
   * @brief Insert a new antecedent into the rule.
   */
  void Insert(int var, float val, char rel);

  /**
   * @brief Create a copy of the rule.
   */
  DimlpRule *Copy(std::shared_ptr<DimlpRule> r);

  /**
   * @brief Delete all antecedents in the rule.
   */
  void Del() {
    NbAnt = 0;
  }

  /**
   * @brief Default constructor for the DimlpRule class.
   */
  DimlpRule() = default;
};

#endif // DIMLPRULE_H
