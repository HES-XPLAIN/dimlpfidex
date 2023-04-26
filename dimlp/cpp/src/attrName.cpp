using namespace std;
#include <string.h>
#include <iostream>
#include "attrName.h"
#include <fstream>

///////////////////////////////////////////////////////////////////

int AttrName::FirstLect()

{  filebuf buf;
   char    str[StringLength];
   int     k;

   if (buf.open(FileAttr, ios_base::in) == 0)
   {
      cout << "Cannot open file of attribute names";
      cout << " (" << FileAttr << ")\n\n";
      return 0;
   }

   istream inFile(&buf);

   for (k=0; ; k++)
   {
       if (inFile >> str) ;
       else
          break;
   }

   if (k == NbAttr + NbClasses) return 1;

   cout << FileAttr;
   cout << ": Problem with number of attributes or number of classes.\n\n";

   return 0;
}

///////////////////////////////////////////////////////////////////

void AttrName::SecondLect()

{  filebuf buf;
   int     k;

   if (buf.open(FileAttr, ios_base::in) == 0);

   istream inFile(&buf);

   for (k=0; k<NbAttr; k++)
       inFile >> VarNames[k];
   
   for (k=0; k<NbClasses; k++)
       inFile >> ClassNames[k];
}

///////////////////////////////////////////////////////////////////

int AttrName::ReadAttr()

{
   if (FirstLect())
      SecondLect();
   else
   {
      FileAttr = 0;
      return 0;
   }

   return 1;
}

///////////////////////////////////////////////////////////////////

void AttrName::Del()

{  int i;

   for (i=0; i<NbAttr; i++)
       delete VarNames[i];

   for (i=0; i<NbClasses; i++)
       delete ClassNames[i];

   delete VarNames;
   delete ClassNames;
   delete FileAttr;
}

///////////////////////////////////////////////////////////////////

AttrName::AttrName(char* fileAttr, int nbAttr, int nbClasses)

{  int   i, j;
   char* ptr;

   NbAttr    = nbAttr;
   NbClasses = nbClasses;

   VarNames   = new char*[NbAttr];
   ClassNames = new char*[NbClasses];

   StringLength = 80;
   FileAttr     = new char[StringLength];

   strcpy(FileAttr, fileAttr);

   for (i=0; i<NbAttr; i++)
   {
       VarNames[i] = new char[StringLength];
       ptr         = VarNames[i];

       for (j=0; j<StringLength; j++)
           ptr[j] = '\0';
   }

   for (i=0; i<NbClasses; i++)
   {
       ClassNames[i] = new char[StringLength];
       ptr           = ClassNames[i];

       for (j=0; j<StringLength; j++)
           ptr[j] = '\0';
   }
}

///////////////////////////////////////////////////////////////////

