#include "stringI.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Insert a new element with the given value at the end of the list.
 *
 * @param val The value to insert.
 */
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

/**
 * @brief Find the index of the maximum value in the list.
 *
 * @return The index of the maximum value.
 */
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

/**
 * @brief Find the index of the minimum value in the list.
 *
 * @return The index of the minimum value.
 */
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

/**
 * @brief Delete the value at the specified index and set it to a new value.
 *
 * @param indPrune The index of the value to delete.
 * @param newSet The new value to set.
 */
void StringInt::DelVal(int indPrune, int newSet)

{
  int i;

  for (i = 0, GoToBeg(); i < indPrune; i++, GoToNext())
    ;

  SetVal(newSet);
}

///////////////////////////////////////////////////////////////////
