#ifndef STRINGI_H
#define STRINGI_H

using namespace std;
///////////////////////////////////////////////////////////////////

class StringInt {
  struct Elem {
    int Val;
    Elem *Next;
  };

  int NbEl;

  Elem *First;
  Elem *Last;
  Elem *PtrList;

  //----------------------------------------------------------------

public:
  int GetNbEl() { return NbEl; }
  void GoToBeg() { PtrList = First; }
  void GoToNext() { PtrList = PtrList->Next; }
  int GetVal() { return PtrList->Val; }
  void SetVal(int val) { PtrList->Val = val; }

  void Insert(int val);
  int FindIndMax();
  int FindIndMin();
  void DelVal(int indPrune, int newSet);

  void Del();
  void DelAll() {
    Del();
    delete this;
  }

  ~StringInt() { Del(); }
  StringInt() { NbEl = 0; }
};

///////////////////////////////////////////////////////////////////

#endif
