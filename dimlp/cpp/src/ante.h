#ifndef ANTE_H
#define ANTE_H

#include "dimlpRule.h"
////////////////////////////////////////////////////////////////////////

/**
 * @brief The Ante class represents an antecedent in a rule, inheriting from the DimlpRule class.
 */
class Ante : public DimlpRule {

  //----------------------------------------------------------------

public:
  /**
   * @brief Get the variable of the antecedent.
   * @return The variable of the antecedent.
   */
  int GetVarAnte() const { return DimlpRule::GetVar(); }

  /**
   * @brief Get the value of the antecedent.
   * @return The value of the antecedent.
   */
  float GetValAnte() const { return DimlpRule::GetVal(); }

  /**
   * @brief Get the relational operator of the antecedent.
   * @return The relational operator of the antecedent.
   */
  char GetRelAnte() const { return DimlpRule::GetRel(); }

  /**
   * @brief Delete the antecedent.
   */
  void DelAnte() { DimlpRule::Del(); }

  /**
   * @brief Construct a new Ante object.
   */
  Ante(int var, float val, char rel);
};

#endif // ANTE_H
