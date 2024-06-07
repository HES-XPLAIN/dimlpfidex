#include "oneVarTD.h"
///////////////////////////////////////////////////////////////////

/**
 * @brief Creates a new element.
 *
 * @param thres The threshold value.
 * @param indPat The index of the pattern.
 * @return A shared pointer to the created element.
 */
std::shared_ptr<OneVarThresDescr::Elem> OneVarThresDescr::CreateElem(float thres, int indPat)

{
  auto e = std::make_shared<OneVarThresDescr::Elem>();

  e->Thres = thres;
  e->Next = nullptr;
  e->CountPatDiscr = 0;
  e->ListPat = std::unique_ptr<StringInt>(new StringInt);

  (e->ListPat)->Insert(indPat);

  NbThres++;

  return e;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Resets the count of patterns for discretization for all elements.
 */
void OneVarThresDescr::ResetCountPatDiscr()

{
  int t;

  for (t = 0, PtrList = First; t < NbThres; t++, PtrList = PtrList->Next)
    PtrList->CountPatDiscr = 0;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Inserts a new threshold and pattern index.
 *
 * @param thres The threshold value.
 * @param indPat The index of the pattern.
 */
void OneVarThresDescr::Insert(float thres, int indPat)

{
  int e;
  auto last = std::make_shared<OneVarThresDescr::Elem>();

  if (NbThres == 0)
    First = CreateElem(thres, indPat);

  else {
    for (e = 0, PtrList = First; e < NbThres; e++, PtrList = PtrList->Next) {
      if (PtrList->Thres == thres) {
        (PtrList->ListPat)->Insert(indPat);
        break;
      }

      last = PtrList;
    }

    if (e == NbThres)
      last->Next = CreateElem(thres, indPat);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Deletes all elements in the list.
 */
void OneVarThresDescr::Del()

{
  PtrList = First;

  for (int e = 0; e < NbThres; e++) {
    PtrList = PtrList->Next;
    (First->ListPat.get())->Del();
    First = PtrList;
  }

  NbThres = 0;
}

///////////////////////////////////////////////////////////////////
