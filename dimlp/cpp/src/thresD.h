#ifndef THRESD_H
#define THRESD_H

using namespace std;
#include "oneVarTD.h"

///////////////////////////////////////////////////////////////////

class ThresDescr
{
   OneVarThresDescr** Descr;

   int NbVar;

//----------------------------------------------------------------

public:

   OneVarThresDescr* GetDescr(int var) { return Descr[var]; }

   void Insert(int var, float thres, int indPat)
        { (Descr[var])->OneVarThresDescr::Insert(thres, indPat); }

   void ResetAllCountPatDiscr();
   void Del();

   ThresDescr(int nbVar);

};

///////////////////////////////////////////////////////////////////

#endif
