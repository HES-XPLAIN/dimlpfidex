using namespace std;
#include "stringI.h"

////////////////////////////////////////////////////////////////////////

int Compare(const void* x, const void* y)

{  float* a = (float*) x;
   float* b = (float*) y;

   if (*a > *b) return 1;

   if (*a < *b) return -1;

   return 0;
}

////////////////////////////////////////////////////////////////////////

int AreSameClass(StringInt* listPat, int* classPatNet)

{  int  p;
   int  nbPat    = listPat->GetNbEl();

   listPat->GoToBeg();
   int  cl = classPatNet[listPat->GetVal()];

   for (p=1, listPat->GoToNext(); p<nbPat; p++, listPat->GoToNext())
       if (cl != classPatNet[listPat->GetVal()]) break;

   if (p == nbPat) return 1;

   return 0;
}

////////////////////////////////////////////////////////////////////////
