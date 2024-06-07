#include "vectWRV.h"
#include "randFun.h"

#include <iostream>
////////////////////////////////////////////////////////////////////////

/**
 * @brief Fills the vector with unique indices.
 *
 * @param nbElSub Number of elements to select.
 * @return A shared pointer to the StringInt object containing the selected indices.
 */
std::shared_ptr<StringInt> VectWithRandVal::FillWithUniqueInd(int nbElSub)

{
  int i;
  int ind;
  auto toRet = std::make_shared<StringInt>();
  IntRandomFunction randGen(0, NbElAll - 1);

  do {
    ind = randGen.RandomInteger();

    if (Busy[ind] == 0) {
      toRet->Insert(ind);
      Busy[ind] = 1;
    }

    else {
      for (i = ind + 1; i < NbElAll; i++) {
        if (Busy[i] == 0)
          break;
      }

      if (i < NbElAll) {
        toRet->Insert(i);
        Busy[i] = 1;
      }
    }

  } while (toRet->GetNbEl() < nbElSub);

  return toRet;
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Appends elements from one list to another.
 *
 * @param originalList The original list to append to.
 * @param toAppend The list of elements to append.
 */
void VectWithRandVal::Append(StringInt *originalList, StringInt *toAppend)

{
  int i;
  int val;
  int nbApp = toAppend->GetNbEl();

  for (i = 0, toAppend->GoToBeg(); i < nbApp; i++, toAppend->GoToNext()) {
    val = toAppend->GetVal();
    Busy[val] = 1;
    originalList->Insert(val);
  }
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructor to initialize the vector with a given number of elements.
 *
 * @param nbElAll The total number of elements.
 */
VectWithRandVal::VectWithRandVal(int nbElAll) : NbElAll(nbElAll)

{
  Busy.resize(NbElAll);

  for (int p = 0; p < nbElAll; p++)
    Busy[p] = 0;
}

////////////////////////////////////////////////////////////////////////
