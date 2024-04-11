#ifndef ANTECEDANT_H
#define ANTECEDANT_H
#include "../../../json/single_include/nlohmann/json.hpp"
#include <cmath>
#include <iostream>
#include <ostream>

using Json = nlohmann::json;

/**
 * @brief Represents an antecedent condition in a rule-based system.
 *
 * This class encapsulates an attribute along with a condition (inequality)
 * that determines whether a given sample satisfies the antecedent. The condition
 * is defined in terms of greater than or less than/equal to a specified value.
 */
class Antecedant {
private:
  int attribute;   ///< Integer ID representing the attribute.
  bool inequality; ///< True if the condition is '>=', False if '<'.
  double value;    ///< The comparison value for the condition.

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Antecedant, attribute, inequality, value)

public:
  Antecedant() = default;
  /**
   * @brief Constructs a new Antecedant object with specified parameters.
   *
   * @param attribute Integer ID of the attribute.
   * @param inequality True for '>=', false for '<'.
   * @param value The value to compare against.
   */
  Antecedant(int attribute, bool inequality, double value);

  // Getters
  int getAttribute() const { return attribute; }
  bool getInequality() const { return inequality; }
  double getValue() const { return value; }

  // Setters
  void setAttribute(int val) { attribute = val; }
  void setInequality(bool val) { inequality = val; }
  void setValue(double val) { value = val; }

  /**
   * @brief Checks equality with another Antecedant object.
   * @param other The other Antecedant to compare with.
   * @return True if both Antecedant objects are equal, False otherwise.
   */
  bool isEqual(Antecedant other) const;
};

// Operator overloads
inline std::ostream &operator<<(std::ostream &stream, const Antecedant &antecedant) {
  return stream << " "
                << antecedant.getAttribute() << " "
                << antecedant.getInequality() << " "
                << antecedant.getValue() << " ";
}

inline bool operator==(const Antecedant &a1, const Antecedant &a2) {
  return a1.isEqual(a2);
}

inline bool operator!=(const Antecedant &a1, const Antecedant &a2) {
  return !a1.isEqual(a2);
}

#endif
