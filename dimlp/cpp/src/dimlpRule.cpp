#include "dimlpRule.h"

///////////////////////////////////////////////////////////////////

int DimlpRule::GetNbAntWithout()

{
  int a;
  int count;

  for (a = 0, count = 0, GoToBeg(); a < NbAnt; a++, GoToNext())
    if (IsAntDeleted() == 0)
      count++;

  return count;
}

///////////////////////////////////////////////////////////////////

void DimlpRule::DeleteRule()

{
  int a;

  for (a = 0, GoToBeg(); a < NbAnt; a++, GoToNext())
    RemAnt();
}

///////////////////////////////////////////////////////////////////

void DimlpRule::Insert(int var, float val, char rel)

{
  if (NbAnt == 0) {
    FirstAnt = std::make_shared<Antecedent>();
    // FirstAnt = new Antecedent;
    FirstAnt->Var = var;
    FirstAnt->Val = val;
    FirstAnt->Rel = rel;
    FirstAnt->Next = nullptr;

    LastAnt = FirstAnt;
  }

  else {
    std::shared_ptr<Antecedent> old = LastAnt;

    LastAnt = std::make_shared<Antecedent>();
    LastAnt->Var = var;
    LastAnt->Val = val;
    LastAnt->Rel = rel;
    LastAnt->Next = nullptr;

    old->Next = LastAnt;
  }

  NbAnt++;
}

///////////////////////////////////////////////////////////////////

DimlpRule *DimlpRule::Copy(std::shared_ptr<DimlpRule> r)

{
  int a;
  int nbAnt = r->GetNbAnt();

  for (a = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
    this->Insert(r->GetVar(), r->GetVal(), r->GetRel());
  }

  return this;
}

///////////////////////////////////////////////////////////////////
