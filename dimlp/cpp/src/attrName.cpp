#include "attrName.h"
using namespace std;

///////////////////////////////////////////////////////////////////

int AttrName::FirstLect() const

{
  filebuf buf;
  std::string str;
  int k;

  if (buf.open(FileAttr, ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open file of attribute names " + std::string(FileAttr));
  }

  istream inFile(&buf);

  for (k = 0;; k++) {
    if (inFile >> str)
      ;
    else
      break;
  }

  if (k == NbAttr + NbClasses || k == NbAttr)
    return 1;

  cout << FileAttr;
  cout << ": Problem with number of attributes or number of classes.\n"
       << std::endl;
  return 0;
}

///////////////////////////////////////////////////////////////////

void AttrName::SecondLect()

{
  filebuf buf;

  if (buf.open(FileAttr, ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open file of attribute names " + std::string(FileAttr));
  }

  istream inFile(&buf);

  VarNames.clear();
  string varName;
  int k = 0;
  while (k < NbAttr) {
    inFile >> varName;
    VarNames.push_back(varName);
    k++;
  }

  ClassNames.clear();
  string className;
  bool hasClasses = false;
  while (inFile >> className) {
    hasClasses = true;
    ClassNames.push_back(className);
  }
  if (!hasClasses) {
    for (int i = 0; i < NbClasses; i++) {
      ClassNames.push_back(std::to_string(i + 1));
    }
  }
}

///////////////////////////////////////////////////////////////////

int AttrName::ReadAttr()

{
  if (FirstLect())
    SecondLect();
  else {
    FileAttr = nullptr;
    return 0;
  }
  return 1;
}

///////////////////////////////////////////////////////////////////

AttrName::AttrName(const char *fileAttr, int nbAttr, int nbClasses) : NbAttr(nbAttr), NbClasses(nbClasses), FileAttr(fileAttr)

{
}

///////////////////////////////////////////////////////////////////
