#ifndef ANTECEDANT_H
#define ANTECEDANT_H
#include <ostream>

using namespace std;

class Antecedant {
private:
  int _attribute;
  bool _inequality;
  double _hyperlocus;

  // PRIVATE SETTERS
  void setAttribute(int attribute) { _attribute = attribute; }
  void setInequality(bool inequality) { _inequality = inequality; }
  void setHyperlocus(double hyperlocus) { _hyperlocus = hyperlocus; }

public:
  Antecedant() = default;
  Antecedant(int attribute, bool inequality, double hyperlocus);

  // GETTERS
  int getAttribute() const { return _attribute; }
  bool getInequality() const { return _inequality; }
  double gethyperlocus() const { return _hyperlocus; }
};

// OPERATOR OVERLOAD TO EASE PRINTING PURPOSES
inline ostream &operator<<(ostream &stream, const Antecedant &antecedant) {
  return stream << " " << antecedant.getAttribute() << " " << antecedant.getInequality() << " " << antecedant.gethyperlocus() << " ";
}

#endif
