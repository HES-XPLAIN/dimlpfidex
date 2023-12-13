#include "antecedant.h"

Antecedant::Antecedant(int attribute, bool inequality, double value) {
  setAttribute(attribute);
  setInequality(inequality);
  setHyperlocus(value);
}
