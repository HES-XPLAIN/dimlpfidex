using namespace std;
#include "rule.h"

///////////////////////////////////////////////////////////////////

int Rule::GetNbAntWithout()

{  int a, count;

   for (a=0, count=0, GoToBeg(); a<NbAnt; a++, GoToNext())
       if (IsAntDeleted() == 0) count++;

   return count;
}

///////////////////////////////////////////////////////////////////

void Rule::DeleteRule()

{  int a;

   for (a=0, GoToBeg(); a<NbAnt; a++, GoToNext())
       RemAnt();
}

///////////////////////////////////////////////////////////////////


void Rule::Insert(int var, float val, char rel)

{
   if (NbAnt == 0)
   {
      FirstAnt       = new Antecedent;
      FirstAnt->Var  = var;
      FirstAnt->Val  = val;
      FirstAnt->Rel  = rel;
      FirstAnt->Next = 0;

      LastAnt = FirstAnt;
   }

   else
   {
      Antecedent* old = LastAnt;

      LastAnt       = new Antecedent;
      LastAnt->Var  = var;
      LastAnt->Val  = val;
      LastAnt->Rel  = rel;
      LastAnt->Next = 0;

      old->Next = LastAnt;
   }

   NbAnt++;
}

///////////////////////////////////////////////////////////////////

Rule* Rule::Copy(Rule* r)

{  int a;
   int nbAnt = r->GetNbAnt();

   for (a=0, r->GoToBeg(); a<nbAnt; a++, r->GoToNext())
   {
       this->Insert(r->GetVar(), r->GetVal(), r->GetRel());
   }

   return this;
}

///////////////////////////////////////////////////////////////////

void Rule::Del()

{  int e;

   PtrAnt = FirstAnt;

   for (e=0; e<NbAnt; e++)
   {
       PtrAnt = PtrAnt->Next;
       delete FirstAnt;
       FirstAnt = PtrAnt;
   }

   NbAnt = 0;
}


///////////////////////////////////////////////////////////////////
