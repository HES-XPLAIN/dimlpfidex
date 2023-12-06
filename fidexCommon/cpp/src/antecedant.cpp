#include "antecedant.h"

/**
 * @brief Construct a new Antecedant:: Antecedant object
 *
 * @param attribute attribute's integer ID equivalent.
 * @param inequality if true, the inequality is "<=", else it is ">".
 * @param hyperlocus
 */
Antecedant::Antecedant(int attribute, bool inequality, double hyperlocus) {
  setAttribute(attribute);
  setInequality(inequality);
  setHyperlocus(hyperlocus);
}
