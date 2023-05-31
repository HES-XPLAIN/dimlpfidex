#ifndef THRESD_H
#define THRESD_H

#include "oneVarTD.h"
#include <vector>

///////////////////////////////////////////////////////////////////

class ThresDescr {
  std::vector<std::shared_ptr<OneVarThresDescr>> Descr;

  int NbVar;

  //----------------------------------------------------------------

public:
  OneVarThresDescr *GetDescr(int var) { return Descr[var].get(); }

  void Insert(int var, float thres, int indPat) { (Descr[var])->OneVarThresDescr::Insert(thres, indPat); }

  void ResetAllCountPatDiscr();
  void Del();

  explicit ThresDescr(int nbVar);
};

///////////////////////////////////////////////////////////////////

#endif
