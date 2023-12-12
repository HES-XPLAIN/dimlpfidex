#include "antecedant.h"

Antecedant::Antecedant(int attribute, bool inequality, double hyperlocus) {
  setAttribute(attribute);
  setInequality(inequality);
  setHyperlocus(hyperlocus);
}
