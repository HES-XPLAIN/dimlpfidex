#include "antecedant.h"

/**
 * @brief Construct a new Antecedant:: Antecedant object
 *
 * @param attribute attribute's integer ID equivalent.
 * @param inequality if true, the inequality is "<=", else it is ">".
 * @param value
 */
Antecedant::Antecedant(int attribute, bool inequality, double value) {
  setAttribute(attribute);
  setInequality(inequality);
  setValue(value);
}

bool Antecedant::isEqual(const Antecedant other) const {
  double epsilon = 10e-6;

  if (getAttribute() != other.getAttribute())
    return false;

  if (getInequality() != other.getInequality())
    return false;

  if (fabs(getValue() != other.getValue()) > epsilon)
    return false;

  return true;
}
