#ifndef DATASET_H
#define DATASET_H

#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/stringI.h"
#include "dimlpRule.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

///////////////////////////////////////////////////////////////////

class DataSet {
  int NbEx = 0;
  int NbAttr = 0;

  float **Set = nullptr;

  // ------------------------------------------------------------------

  int FirstLecture(const std::string &nameFile) const;
  void SecondLecture(const std::string &nameFile);

  void SetNbAttr(int nbAttr) { NbAttr = nbAttr; }
  void InsertExample(const std::vector<float> &example, int index);

  // ------------------------------------------------------------------

public:
  DataSet() = default;
  explicit DataSet(int nbEx);
  DataSet(const std::string &nameFile, int nbAttr);
  DataSet(const std::string &nameFile, int nbIn, int nbOut);
  DataSet(DataSet &bigData, StringInt *listPat);
  DataSet(DataSet &master, const int *indPat, int nbEx);
  DataSet(DataSet &data1, DataSet &data2);

  void Del();

  float *GetExample(int index) { return *(Set + index); }
  int GetNbEx() const { return NbEx; }
  int GetNbAttr() const { return NbAttr; }

  std::shared_ptr<StringInt> Select(std::shared_ptr<DimlpRule> r);
  std::shared_ptr<StringInt> Select(std::shared_ptr<DimlpRule> r, std::shared_ptr<StringInt> subSet);

  void ExtractDataAndTarget(DataSet &data1, int nbAttr1,
                            DataSet &data2, int nbAttr2) const;
};

///////////////////////////////////////////////////////////////////

#endif
