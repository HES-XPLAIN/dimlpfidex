#ifndef ANTE_H
#define ANTE_H

#ifndef RULE
#include "rule.h"
#endif

////////////////////////////////////////////////////////////////////////

class Ante : Rule {

  //----------------------------------------------------------------

public:
  int GetVar() { return Rule::GetVar(); }
  float GetVal() { return Rule::GetVal(); }
  char GetRel() { return Rule::GetRel(); }

  void Del() { Rule::Del(); }
  void DelAll() {
    Rule::Del();
    delete this;
  }

  Ante(int var, float val, char rel);
};

#endif
