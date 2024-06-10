#ifndef STRINGI_H
#define STRINGI_H

#include <memory>

///////////////////////////////////////////////////////////////////

/**
 * @brief A class to represent a singly linked list of integers.
 *
 * The StringInt class manages a singly linked list where each node contains an integer value.
 * It provides functions to manipulate and query the list.
 */
class StringInt {

  /**
   * @brief Structure representing a single element in the linked list.
   */
  struct Elem {
    int Val;                    ///< Integer value stored in the node.
    std::shared_ptr<Elem> Next; ///< Pointer to the next node in the list.
  };

  int NbEl = 0; ///< Number of elements in the list.

  std::shared_ptr<Elem> First;   ///< Pointer to the first element in the list.
  std::shared_ptr<Elem> Last;    ///< Pointer to the last element in the list.
  std::shared_ptr<Elem> PtrList; ///< Pointer to the current element in the list.
  //----------------------------------------------------------------

public:
  /**
   * @brief Get the number of elements in the list.
   *
   * @return int The number of elements in the list.
   */
  int GetNbEl() const { return NbEl; }

  /**
   * @brief Set the current element pointer to the first element in the list.
   */
  void GoToBeg() { PtrList = First; }

  /**
   * @brief Move the current element pointer to the next element in the list.
   */
  void GoToNext() { PtrList = PtrList->Next; }

  /**
   * @brief Get the value of the current element.
   *
   * @return int The value of the current element.
   */
  int GetVal() const { return PtrList->Val; }

  /**
   * @brief Set the value of the current element.
   *
   * @param val The new value to set.
   */
  void SetVal(int val) const { PtrList->Val = val; }

  /**
   * @brief Insert a new element with the given value at the end of the list.
   */
  void Insert(int val);

  /**
   * @brief Find the index of the maximum value in the list.
   */
  int FindIndMax();

  /**
   * @brief Find the index of the minimum value in the list.
   */
  int FindIndMin();

  /**
   * @brief Delete the value at the specified index and set it to a new value.
   */
  void DelVal(int indPrune, int newSet);

  /**
   * @brief Delete all elements in the list.
   */
  void Del() { NbEl = 0; };

  ~StringInt() = default;
  StringInt() = default;
};

///////////////////////////////////////////////////////////////////

#endif // STRINGI_H
