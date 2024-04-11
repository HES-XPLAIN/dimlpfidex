#include "antecedant.h"

/**
 * @details Initializes the Antecedant object with the provided attribute ID, inequality flag, and comparison value.
 */
Antecedant::Antecedant(int attribute, bool inequality, double value) {
  setAttribute(attribute);
  setInequality(inequality);
  setValue(value);
}

/**
 * @details Compares this antecedant with another for equality. Two antecedants are considered equal if they
 * have the same attribute ID, the same inequality condition, and a comparison value that differs by less
 * than a specified epsilon (10^-6).
 */
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
