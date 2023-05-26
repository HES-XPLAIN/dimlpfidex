#ifndef ONEVARTD_H
#define ONEVARTD_H

using namespace std;
#ifndef STRINGINT
#include "stringI.h"
#endif

///////////////////////////////////////////////////////////////////

class OneVarThresDescr {
  struct Elem {
    float Thres;
    StringInt *ListPat;
    int CountPatDiscr;
    Elem *Next;
  };

  int NbThres;
  Elem *First;
  Elem *PtrList;

  //----------------------------------------------------------------

  Elem *CreateElem(float thres, int indPat);

  //----------------------------------------------------------------

public:
  int GetNbThres() { return NbThres; }
  void GoToBeg() { PtrList = First; }
  void GoToNext() { PtrList = PtrList->Next; }
  float GetThres() { return PtrList->Thres; }
  StringInt *GetPtrPat() { return PtrList->ListPat; }
  int GetCountPatDiscr() { return PtrList->CountPatDiscr; }
  void IncCountPatDiscr() { (PtrList->CountPatDiscr)++; }
  void SetCountPat(int val) { PtrList->CountPatDiscr = val; }

  void ResetCountPatDiscr();
  void Insert(float thres, int indPat);
  void Del();

  OneVarThresDescr() { NbThres = 0; }
};

///////////////////////////////////////////////////////////////////

#endif
