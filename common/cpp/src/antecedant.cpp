#include "antecedant.h"

/**
 * @brief Constructs a new Antecedant object with specified parameters.
 *
 * Initializes the Antecedant object with the provided attribute ID, inequality flag, and comparison value.
 *
 * @param attribute Integer ID of the attribute.
 * @param inequality True for '>=', false for '<'.
 * @param value The value to compare against.
 */
Antecedant::Antecedant(int attribute, bool inequality, double value) {
  setAttribute(attribute);
  setInequality(inequality);
  setValue(value);
}

/**
 * @brief Checks equality with another Antecedant object.
 *
 * Compares this antecedent with another for equality. Two antecedents are considered equal if they
 * have the same attribute ID, the same inequality condition, and a comparison value that differs by less
 * than a specified epsilon (10^-6).
 *
 * @param other The other Antecedant to compare with.
 * @return True if both Antecedant objects are equal, False otherwise.
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
