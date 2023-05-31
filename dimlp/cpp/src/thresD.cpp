#include "thresD.h"

///////////////////////////////////////////////////////////////////

void ThresDescr::ResetAllCountPatDiscr()

{
  for (int v = 0; v < NbVar; v++)
    Descr[v]->ResetCountPatDiscr();
}

///////////////////////////////////////////////////////////////////

void ThresDescr::Del()

{
  for (int v = 0; v < NbVar; v++)
    (Descr[v])->OneVarThresDescr::Del();
}

///////////////////////////////////////////////////////////////////

ThresDescr::ThresDescr(int nbVar) : Descr(nbVar), NbVar(nbVar)

{
  for (int v = 0; v < NbVar; v++)
    Descr[v] = std::make_shared<OneVarThresDescr>();
}

///////////////////////////////////////////////////////////////////
