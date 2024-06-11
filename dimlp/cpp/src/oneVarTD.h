#ifndef ONEVARTD_H
#define ONEVARTD_H

#include "../../../common/cpp/src/stringI.h"
#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief Class to describe thresholds for a single variable.
 *
 * This class maintains a list of thresholds and associated pattern indices.
 * Thresholds are specific values used to discretize a continuous variable,
 * and patterns represent the instances of data that fall within these thresholds.
 */
class OneVarThresDescr {
  /**
   * @brief Structure to store threshold details.
   */
  struct Elem {
    float Thres;                        ///< Threshold value.
    std::unique_ptr<StringInt> ListPat; ///< List of pattern indices.
    int CountPatDiscr;                  ///< Count of patterns for discretization.
    std::shared_ptr<Elem> Next;         ///< Pointer to the next element.
  };

  int NbThres = 0;               ///< Number of thresholds.
  std::shared_ptr<Elem> First;   ///< Pointer to the first element.
  std::shared_ptr<Elem> PtrList; ///< Pointer to traverse the list.

  //----------------------------------------------------------------
  /**
   * @brief Creates a new element.
   */
  std::shared_ptr<Elem> CreateElem(float thres, int indPat);

  //----------------------------------------------------------------

public:
  /**
   * @brief Gets the number of thresholds.
   * @return The number of thresholds.
   */
  int GetNbThres() const { return NbThres; }

  /**
   * @brief Resets the pointer to the beginning of the list.
   */
  void GoToBeg() { PtrList = First; }

  /**
   * @brief Advances the pointer to the next element in the list.
   */
  void GoToNext() { PtrList = PtrList->Next; }

  /**
   * @brief Gets the current threshold value.
   * @return The current threshold value.
   */
  float GetThres() const { return PtrList->Thres; }

  /**
   * @brief Gets the pointer to the list of patterns.
   * @return The pointer to the list of patterns.
   */
  StringInt *GetPtrPat() { return PtrList->ListPat.get(); }

  /**
   * @brief Gets the count of patterns for discretization.
   * @return The count of patterns for discretization.
   */
  int GetCountPatDiscr() const { return PtrList->CountPatDiscr; }

  /**
   * @brief Increments the count of patterns for discretization.
   */
  void IncCountPatDiscr() const { (PtrList->CountPatDiscr)++; }

  /**
   * @brief Sets the count of patterns for discretization.
   * @param val The value to set.
   */
  void SetCountPat(int val) const { PtrList->CountPatDiscr = val; }

  /**
   * @brief Resets the count of patterns for discretization for all elements.
   */
  void ResetCountPatDiscr();

  /**
   * @brief Inserts a new threshold and pattern index.
   */
  void Insert(float thres, int indPat);

  /**
   * @brief Deletes all elements in the list.
   */
  void Del();

  /**
   * @brief Constructor for OneVarThresDescr.
   */
  OneVarThresDescr() = default;
};

///////////////////////////////////////////////////////////////////

#endif // ONEVARTD_H
