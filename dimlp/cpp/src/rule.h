#ifndef RULE_H
#define RULE_H

#include <memory>

////////////////////////////////////////////////////////////////////////

class Rule {
  struct Antecedent {
    int Var;
    float Val;
    char Rel;
    std::shared_ptr<Antecedent> Next;
  };

  int NbAnt = 0;

  std::shared_ptr<Antecedent> FirstAnt;
  std::shared_ptr<Antecedent> LastAnt;
  std::shared_ptr<Antecedent> PtrAnt;
  std::shared_ptr<Antecedent> Memory;

  //----------------------------------------------------------------

public:
  int GetNbAnt() const { return NbAnt; }
  void GoToBeg() { PtrAnt = FirstAnt; }
  void GoToNext() { PtrAnt = PtrAnt->Next; }
  int GetVar() const { return PtrAnt->Var; }
  float GetVal() const { return PtrAnt->Val; }
  char GetRel() const { return PtrAnt->Rel; }
  void RemAnt() const { PtrAnt->Var = -1; }
  void SetAnt(int var) const { PtrAnt->Var = var; }
  void SetThres(float val) const { PtrAnt->Val = val; }
  int IsAntDeleted() const { return ((PtrAnt->Var == -1) ? 1 : 0); }
  void SavePtrAnt() { Memory = PtrAnt; }
  void PrevPtrAnt() { PtrAnt = Memory; }

  int GetNbAntWithout();
  void DeleteRule();
  void Insert(int var, float val, char rel);
  Rule *Copy(std::shared_ptr<Rule> r);

  void Del() {
    NbAnt = 0;
  }

  Rule() = default;
  ~Rule() = default;
};

#endif
