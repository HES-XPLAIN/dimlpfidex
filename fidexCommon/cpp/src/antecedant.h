#ifndef ANTECEDANT_H
#define ANTECEDANT_H
#include <cmath>
#include <ostream>

using namespace std;

class Antecedant {
private:
  int _attribute;
  bool _inequality; // true if attribute >= value false if attribute < value
  double _value;

  // PRIVATE SETTERS
  void setAttribute(int attribute) { _attribute = attribute; }
  void setInequality(bool inequality) { _inequality = inequality; }
  void setHyperlocus(double value) { _value = value; }

public:
  Antecedant() = default;
  Antecedant(int attribute, bool inequality, double value);

  // GETTERS
  int getAttribute() const { return _attribute; }
  bool getInequality() const { return _inequality; }
  double getValue() const { return _value; }

  bool isEqual(Antecedant other) const;
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
inline ostream &operator<<(ostream &stream, const Antecedant &antecedant) {
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
