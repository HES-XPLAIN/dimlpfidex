#ifndef RULE_H
#define RULE_H

using namespace std;
////////////////////////////////////////////////////////////////////////

class Rule {
  struct Antecedent {
    int Var;
    float Val;
    char Rel;
    Antecedent *Next;
  };

  int NbAnt;

  Antecedent *FirstAnt;
  Antecedent *LastAnt;
  Antecedent *PtrAnt;
  Antecedent *Memory;

  //----------------------------------------------------------------

public:
  int GetNbAnt() { return NbAnt; }
  void GoToBeg() { PtrAnt = FirstAnt; }
  void GoToNext() { PtrAnt = PtrAnt->Next; }
  int GetVar() { return PtrAnt->Var; }
  float GetVal() { return PtrAnt->Val; }
  char GetRel() { return PtrAnt->Rel; }
  void RemAnt() { PtrAnt->Var = -1; }
  void SetAnt(int var) { PtrAnt->Var = var; }
  void SetThres(float val) { PtrAnt->Val = val; }
  int IsAntDeleted() { return ((PtrAnt->Var == -1) ? 1 : 0); }
  void SavePtrAnt() { Memory = PtrAnt; }
  void PrevPtrAnt() { PtrAnt = Memory; }

  int GetNbAntWithout();
  void DeleteRule();
  void Insert(int var, float val, char rel);
  Rule *Copy(Rule *r);

  void Del();
  void DelAll() {
    Del();
    delete this;
  }

  Rule() { NbAnt = 0; }
  ~Rule() { Del(); }
};

#endif
