#ifndef ANTECEDANT_H
#define ANTECEDANT_H
#include "../../../json/single_include/nlohmann/json.hpp"
#include <cmath>
#include <iostream>
#include <ostream>

using Json = nlohmann::json;

class Antecedant {
private:
  int attribute;
  bool inequality; // true if attribute >= value false if attribute < value
  double value;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Antecedant, attribute, inequality, value)

public:
  Antecedant() = default;
  Antecedant(int attribute, bool inequality, double value);

  // GETTERS
  int getAttribute() const { return attribute; }
  bool getInequality() const { return inequality; }
  double getValue() const { return value; }

  void setAttribute(int val) { attribute = val; }
  void setInequality(bool val) { inequality = val; }
  void setValue(double val) { value = val; }

  bool isEqual(Antecedant other) const;
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
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
