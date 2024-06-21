#ifndef ANTECEDENT_H
#define ANTECEDENT_H
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
class Antecedent {
private:
  int attribute;   ///< Integer ID representing the attribute.
  bool inequality; ///< True if the condition is '>=', False if '<'.
  double value;    ///< The comparison value for the condition.

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Antecedent, attribute, inequality, value)

public:
  Antecedent() = default;
  /**
   * @brief Constructs a new Antecedent object with specified parameters.
   */
  Antecedent(int attribute, bool inequality, double value);

  // Getters

  /**
   * @brief Gets the attribute ID.
   *
   * @return int The attribute ID.
   */
  int getAttribute() const { return attribute; }

  /**
   * @brief Gets the inequality condition.
   *
   * @return bool True if the condition is '>=', False if '<'.
   */
  bool getInequality() const { return inequality; }

  /**
   * @brief Gets the comparison value.
   *
   * @return double The comparison value for the condition.
   */
  double getValue() const { return value; }

  // Setters

  /**
   * @brief Sets the attribute ID.
   *
   * @param val The new attribute ID.
   */
  void setAttribute(int val) { attribute = val; }

  /**
   * @brief Sets the inequality condition.
   *
   * @param val True for '>=', False for '<'.
   */
  void setInequality(bool val) { inequality = val; }

  /**
   * @brief Sets the comparison value.
   *
   * @param val The new comparison value.
   */
  void setValue(double val) { value = val; }

  /**
   * @brief Checks equality with another Antecedent object.
   */
  bool isEqual(Antecedent other) const;
};

// Operator overloads

/**
 * @brief Overloads the stream insertion operator for easy printing of Antecedent objects.
 *
 * @param stream The output stream.
 * @param antecedent The Antecedent object to print.
 * @return std::ostream& The output stream with the Antecedent information.
 */
inline std::ostream &operator<<(std::ostream &stream, const Antecedent &antecedent) {
  return stream << " "
                << std::to_string(antecedent.getAttribute()) << " "
                << std::to_string(antecedent.getInequality()) << " "
                << std::to_string(antecedent.getValue()) << " ";
}

/**
 * @brief Overloads the equality operator to compare two Antecedent objects.
 *
 * @param a1 The first Antecedent object.
 * @param a2 The second Antecedent object.
 * @return bool Returns true if the antecedents are identical, false otherwise.
 */
inline bool operator==(const Antecedent &a1, const Antecedent &a2) {
  return a1.isEqual(a2);
}

/**
 * @brief Overloads the inequality operator to compare two Antecedent objects.
 *
 * @param a1 The first Antecedent object.
 * @param a2 The second Antecedent object.
 * @return bool Returns true if the antecedents are not identical, false otherwise.
 */
inline bool operator!=(const Antecedent &a1, const Antecedent &a2) {
  return !a1.isEqual(a2);
}

#endif // ANTECEDENT_H
