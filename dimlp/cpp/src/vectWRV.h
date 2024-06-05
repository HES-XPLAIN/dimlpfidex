#ifndef VECTWRV_H
#define VECTWRV_H

#ifndef STRINGINT
#include "../../../common/cpp/src/stringI.h"
#endif

#include <memory>
#include <vector>

class VectWithRandVal

{
  int NbElAll;
  std::vector<int> Busy;

public:
  std::shared_ptr<StringInt> FillWithUniqueInd(int nbElSub);
  void Append(StringInt *originalList, StringInt *toAppend);

  explicit VectWithRandVal(int nbElAll);
  ~VectWithRandVal() = default;
};

#endif // VECTWRV_H
