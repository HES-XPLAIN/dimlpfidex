#ifndef ATTRNAME_H
#define ATTRNAME_H

#include "writeErr.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

class AttrName {
  int NbAttr;
  int NbClasses;

  std::string FileAttr;
  std::vector<std::string> VarNames;
  std::vector<std::string> ClassNames;

  int FirstLect() const;
  void SecondLect();

public:
  int ReadAttr();
  int IsFileAttr() const { return (FileAttr.empty() ? 0 : 1); }
  std::vector<std::string> GetListAttr() const { return VarNames; }
  std::vector<std::string> GetListClasses() const { return ClassNames; }

  AttrName(const char *fileAttr, int nbAttr, int nbClasses);
  AttrName() = default;
};

#endif
