#ifndef VECTWRV_H
#define VECTWRV_H

#include "../../../common/cpp/src/stringI.h"

#include <memory>
#include <vector>

/**
 * @brief A class for handling a vector with random values and operations on it.
 */
class VectWithRandVal

{
  int NbElAll;           ///< Total number of elements.
  std::vector<int> Busy; ///< Vector indicating if an element is used.

public:
  /**
   * @brief Fills the vector with unique indices.
   */
  std::shared_ptr<StringInt> FillWithUniqueInd(int nbElSub);

  /**
   * @brief Appends elements from one list to another.
   */
  void Append(StringInt *originalList, StringInt *toAppend);

  /**
   * @brief Constructor to initialize the vector with a given number of elements.
   */
  explicit VectWithRandVal(int nbElAll);

  /**
   * @brief Default destructor.
   */
  ~VectWithRandVal() = default;
};

#endif // VECTWRV_H
