#ifndef ONEVARTD_H
#define ONEVARTD_H

#ifndef STRINGINT
#include "stringI.h"
#endif
#include <memory>

///////////////////////////////////////////////////////////////////

class OneVarThresDescr {
  struct Elem {
    float Thres;
    std::unique_ptr<StringInt> ListPat;
    int CountPatDiscr;
    std::shared_ptr<Elem> Next;
  };

  int NbThres = 0;
  std::shared_ptr<Elem> First;
  std::shared_ptr<Elem> PtrList;

  //----------------------------------------------------------------

  std::shared_ptr<Elem> CreateElem(float thres, int indPat);

  //----------------------------------------------------------------

public:
  int GetNbThres() const { return NbThres; }
  void GoToBeg() { PtrList = First; }
  void GoToNext() { PtrList = PtrList->Next; }
  float GetThres() const { return PtrList->Thres; }
  StringInt *GetPtrPat() { return PtrList->ListPat.get(); }
  int GetCountPatDiscr() const { return PtrList->CountPatDiscr; }
  void IncCountPatDiscr() const { (PtrList->CountPatDiscr)++; }
  void SetCountPat(int val) const { PtrList->CountPatDiscr = val; }

  void ResetCountPatDiscr();
  void Insert(float thres, int indPat);
  void Del();

  OneVarThresDescr() = default;
};

///////////////////////////////////////////////////////////////////

#endif
