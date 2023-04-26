#ifndef VECTWRV_H
#define VECTWRV_H

using namespace std;
#ifndef STRINGINT
#include "stringI.h"
#endif

class VectWithRandVal

{
    int  NbElAll;
    int* Busy;

public:

     StringInt* FillWithUniqueInd(int nbElSub);
     void       Append(StringInt* originalList, StringInt* toAppend);

     VectWithRandVal(int nbElAll);
     ~VectWithRandVal() { delete Busy; }
};

#endif
