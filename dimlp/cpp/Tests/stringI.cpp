using namespace std;
#include "stringI.h"

///////////////////////////////////////////////////////////////////

void StringInt::Insert(int val)

{
  if (NbEl == 0) {
    First = new Elem;
    First->Val = val;
    First->Next = 0;

    Last = First;
  }

  else {
    Elem *old = Last;

    Last = new Elem;
    Last->Val = val;
    Last->Next = 0;

    old->Next = Last;
  }

  NbEl++;
}

///////////////////////////////////////////////////////////////////

int StringInt::FindIndMax()

{
  float max, newVal;
  int e, indMax;

  if (NbEl == 1)
    return 0;

  GoToBeg();
  max = GetVal();
  indMax = 0;

  for (e = 1, GoToNext(); e < NbEl; e++, GoToNext()) {
    newVal = GetVal();

    if (newVal > max) {
      max = newVal;
      indMax = e;
    }
  }

  if (max < 0)
    return -1;

  return indMax;
}

///////////////////////////////////////////////////////////////////

int StringInt::FindIndMin()

{
  float min, newVal;
  int e, indMin;

  if (NbEl == 1)
    return 0;

  GoToBeg();
  min = GetVal();
  indMin = 0;

  for (e = 1, GoToNext(); e < NbEl; e++, GoToNext()) {
    newVal = GetVal();

    if (newVal < min) {
      min = newVal;
      indMin = e;
    }
  }

  return indMin;
}

///////////////////////////////////////////////////////////////////

void StringInt::DelVal(int indPrune, int newSet)

{
  int i;

  for (i = 0, GoToBeg(); i < indPrune; i++, GoToNext())
    ;

  SetVal(newSet);
}

///////////////////////////////////////////////////////////////////

void StringInt::Del()

{
  int e;

  PtrList = First;

  for (e = 0; e < NbEl; e++) {
    PtrList = PtrList->Next;
    delete First;
    First = PtrList;
  }

  NbEl = 0;
}

///////////////////////////////////////////////////////////////////
