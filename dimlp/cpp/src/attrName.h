#ifndef ATTRNAME_H
#define ATTRNAME_H

using namespace std;
class AttrName
{
   int    NbAttr;
   int    NbClasses;
   int    StringLength;

   char*  FileAttr;
   char** VarNames;
   char** ClassNames;


   int  FirstLect();
   void SecondLect();


public:

   int    ReadAttr();
   void   Del();
   int    IsFileAttr() { return ((FileAttr == 0) ? 0 : 1); }
   char** GetListAttr() { return VarNames; }
   char** GetListClasses() { return ClassNames; }

   AttrName(char* fileAttr, int nbAttr, int nbClasses);
   AttrName() { FileAttr = 0; }

};

#endif
