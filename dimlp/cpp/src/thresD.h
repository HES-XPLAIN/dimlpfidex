#ifndef THRESD_H
#define THRESD_H

#include "oneVarTD.h"
#include <vector>

///////////////////////////////////////////////////////////////////

/**
 * @brief Class to manage discretization thresholds for multiple variables.
 *
 * This class uses `OneVarThresDescr` to store and manage thresholds for each variable.
 */
class ThresDescr {

  std::vector<std::shared_ptr<OneVarThresDescr>> Descr; ///< Vector of threshold descriptors for each variable.
  int NbVar;                                            ///< Number of variables.

  //----------------------------------------------------------------

public:
  /**
   * @brief Retrieves the threshold descriptor for a given variable.
   *
   * @param var Index of the variable.
   * @return Pointer to the threshold descriptor of the variable.
   */
  OneVarThresDescr *GetDescr(int var) { return Descr[var].get(); }

  /**
   * @brief Inserts a threshold and pattern index for a given variable.
   *
   * @param var Index of the variable.
   * @param thres Threshold value.
   * @param indPat Pattern index.
   */
  void Insert(int var, float thres, int indPat) { (Descr[var])->OneVarThresDescr::Insert(thres, indPat); }

  /**
   * @brief Resets the discretization pattern count for all variables.
   */
  void ResetAllCountPatDiscr();

  /**
   * @brief Deletes all thresholds and patterns for all variables.
   */
  void Del();

  /**
   * @brief Constructor for the `ThresDescr` class.
   */
  explicit ThresDescr(int nbVar);
};

///////////////////////////////////////////////////////////////////

#endif // THRESD_H
