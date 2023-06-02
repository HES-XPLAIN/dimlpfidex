#ifndef ANTE_H
#define ANTE_H

#ifndef RULE
#include "rule.h"
#endif
////////////////////////////////////////////////////////////////////////

class Ante : public Rule {

  //----------------------------------------------------------------

public:
  int GetVarAnte() const { return Rule::GetVar(); }
  float GetValAnte() const { return Rule::GetVal(); }
  char GetRelAnte() const { return Rule::GetRel(); }

  void DelAnte() { Rule::Del(); }

  Ante(int var, float val, char rel);
};

#endif
