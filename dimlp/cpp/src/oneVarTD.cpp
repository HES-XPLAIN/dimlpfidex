using namespace std;
#include "oneVarTD.h"

///////////////////////////////////////////////////////////////////

OneVarThresDescr::Elem* OneVarThresDescr::CreateElem(float thres, int indPat)

{  OneVarThresDescr::Elem* e = new OneVarThresDescr::Elem;

   e->Thres         = thres;
   e->Next          = 0;
   e->CountPatDiscr = 0;
   e->ListPat       = new StringInt; 

   (e->ListPat)->Insert(indPat);

   NbThres++;

   return e;
}

///////////////////////////////////////////////////////////////////

void OneVarThresDescr::ResetCountPatDiscr()

{  int t;

   for (t=0, PtrList=First; t<NbThres; t++, PtrList=PtrList->Next)
       PtrList->CountPatDiscr = 0;
}

///////////////////////////////////////////////////////////////////

void OneVarThresDescr::Insert(float thres, int indPat)

{  int e;
   OneVarThresDescr::Elem* last;

   if (NbThres == 0)
      First = CreateElem(thres, indPat);

   else
   {
      for (e=0, PtrList=First; e<NbThres; e++, PtrList=PtrList->Next)
      {
          if (PtrList->Thres == thres)
          {
             (PtrList->ListPat)->Insert(indPat);
             break;
          }

          last = PtrList;
      }

      if (e == NbThres)
         last->Next = CreateElem(thres, indPat);
   }
}

///////////////////////////////////////////////////////////////////

void OneVarThresDescr::Del()

{  int e;

   PtrList = First;

   for (e=0; e<NbThres; e++)
   {
       PtrList = PtrList->Next;
       (First->ListPat)->Del();
       delete First;
       First = PtrList;
   }
   
   NbThres = 0;
}


///////////////////////////////////////////////////////////////////
