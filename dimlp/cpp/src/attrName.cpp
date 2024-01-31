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

  throw FileContentError(std::string(FileAttr) + ": Problem with number of attributes or number of classes." + std::string(FileAttr));
  return 0;
}

///////////////////////////////////////////////////////////////////

void AttrName::SecondLect() {
  filebuf buf;

  if (buf.open(FileAttr, ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open file of attribute names " + std::string(FileAttr));
  }

  istream inFile(&buf);

  VarNames.clear();
  string varName;
  int k = 0;
  while (k < NbAttr && std::getline(inFile, varName)) {

    while (!varName.empty() && (varName.back() == '\n' || varName.back() == '\r')) { // Remove line breaks
      varName.pop_back();
    }

    if (hasSpaceBetweenWords(varName)) {
      throw FileContentError("Error in file " + std::string(FileAttr) + " : an attribute has spaces inbetween. Maybe replace it by an underscore.");
    }
    VarNames.push_back(varName);
    k++;
  }
  ClassNames.clear();
  string className;
  bool hasClasses = false;

  while (std::getline(inFile, className)) {
    if (hasSpaceBetweenWords(className)) {
      throw FileContentError("Error in file " + std::string(FileAttr) + " : a class has spaces inbetween. Maybe replace it by an underscore.");
    }
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
