#include "stringI.h"

///////////////////////////////////////////////////////////////////

void StringInt::Insert(int val)

{
  if (NbEl == 0) {
    First = std::make_shared<Elem>();
    First->Val = val;
    First->Next = nullptr;

    Last = First;
  }

  else {
    std::shared_ptr<Elem> old = Last;

    Last = std::make_shared<Elem>();
    Last->Val = val;
    Last->Next = nullptr;

    old->Next = Last;
  }

  NbEl++;
}

///////////////////////////////////////////////////////////////////

int StringInt::FindIndMax()

{
  float max;
  float newVal;
  int e;
  int indMax;

  if (NbEl == 1)
    return 0;

  GoToBeg();
  max = static_cast<float>(GetVal());
  indMax = 0;

  for (e = 1, GoToNext(); e < NbEl; e++, GoToNext()) {
    newVal = static_cast<float>(GetVal());

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
  float min;
  float newVal;
  int e;
  int indMin;

  if (NbEl == 1)
    return 0;

  GoToBeg();
  min = static_cast<float>(GetVal());
  indMin = 0;

  for (e = 1, GoToNext(); e < NbEl; e++, GoToNext()) {
    newVal = static_cast<float>(GetVal());

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
