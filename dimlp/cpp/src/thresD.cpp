using namespace std;
#include "thresD.h"

///////////////////////////////////////////////////////////////////

void ThresDescr::ResetAllCountPatDiscr()

{  int v;

   for (v=0; v<NbVar; v++)
       Descr[v]->ResetCountPatDiscr();
}

///////////////////////////////////////////////////////////////////

void ThresDescr::Del()

{  int v;

   for (v=0; v<NbVar; v++)
        (Descr[v])->OneVarThresDescr::Del();

}

///////////////////////////////////////////////////////////////////

ThresDescr::ThresDescr(int nbVar)

{  int v;

   NbVar      = nbVar;
   Descr = new OneVarThresDescr*[NbVar];

   for (v=0; v<NbVar; v++)
       Descr[v] = new OneVarThresDescr;
}

///////////////////////////////////////////////////////////////////

