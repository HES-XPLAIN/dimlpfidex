#ifndef ANTECEDANT_H
#define ANTECEDANT_H
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
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
inline ostream &operator<<(ostream &stream, const Antecedant &antecedant) {
  return stream << " " << antecedant.getAttribute() << " " << antecedant.getInequality() << " " << antecedant.getValue() << " ";
}

#endif
