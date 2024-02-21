#ifndef ANTE_H
#define ANTE_H

#ifndef RULE
#include "dimlpRule.h"
#endif
////////////////////////////////////////////////////////////////////////

class Ante : public DimlpRule {

  //----------------------------------------------------------------

public:
  int GetVarAnte() const { return DimlpRule::GetVar(); }
  float GetValAnte() const { return DimlpRule::GetVal(); }
  char GetRelAnte() const { return DimlpRule::GetRel(); }

  void DelAnte() { DimlpRule::Del(); }

  Ante(int var, float val, char rel);
};

#endif
