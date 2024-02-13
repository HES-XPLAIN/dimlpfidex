#ifndef STRINGI_H
#define STRINGI_H

#include <memory>

///////////////////////////////////////////////////////////////////

class StringInt {
  struct Elem {
    int Val;
    std::shared_ptr<Elem> Next;
  };

  int NbEl = 0;

  std::shared_ptr<Elem> First;
  std::shared_ptr<Elem> Last;
  std::shared_ptr<Elem> PtrList;

  //----------------------------------------------------------------

public:
  int GetNbEl() const { return NbEl; }
  void GoToBeg() { PtrList = First; }
  void GoToNext() { PtrList = PtrList->Next; }
  int GetVal() const { return PtrList->Val; }
  void SetVal(int val) const { PtrList->Val = val; }

  void Insert(int val);
  int FindIndMax();
  int FindIndMin();
  void DelVal(int indPrune, int newSet);

  void Del() { NbEl = 0; };

  ~StringInt() = default;
  StringInt() = default;
};

///////////////////////////////////////////////////////////////////

#endif
