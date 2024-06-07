#include "thresD.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Resets the discretization pattern count for all variables.
 */
void ThresDescr::ResetAllCountPatDiscr()

{
  for (int v = 0; v < NbVar; v++)
    Descr[v]->ResetCountPatDiscr();
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Deletes all thresholds and patterns for all variables.
 */
void ThresDescr::Del()

{
  for (int v = 0; v < NbVar; v++)
    (Descr[v])->OneVarThresDescr::Del();
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for the `ThresDescr` class.
 *
 * @param nbVar Number of variables.
 */
ThresDescr::ThresDescr(int nbVar) : Descr(nbVar), NbVar(nbVar)

{
  for (int v = 0; v < NbVar; v++)
    Descr[v] = std::make_shared<OneVarThresDescr>();
}

///////////////////////////////////////////////////////////////////
