#include "ante.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Construct a new Ante object.
 * @param var The variable of the antecedent.
 * @param val The value of the antecedent.
 * @param rel The relational operator of the antecedent.
 */
Ante::Ante(int var, float val, char rel) : DimlpRule()

{
  DimlpRule::Insert(var, val, rel);
  DimlpRule::GoToBeg();
}

///////////////////////////////////////////////////////////////////
