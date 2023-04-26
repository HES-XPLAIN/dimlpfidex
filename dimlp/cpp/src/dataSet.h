#ifndef DATASET_H
#define DATASET_H

using namespace std;
#include "rule.h"
#include "stringI.h"

///////////////////////////////////////////////////////////////////

class DataSet {
  int NbEx;
  int NbAttr;

  float **Set;

  // ------------------------------------------------------------------

  int FirstLecture(char nameFile[]);
  void SecondLecture(char nameFile[]);

  void SetNbAttr(int nbAttr) { NbAttr = nbAttr; }
  void InsertExample(float example[], int index);

  // ------------------------------------------------------------------

public:
  DataSet();
  DataSet(int nbEx);
  DataSet(char nameFile[], int nbAttr);
  DataSet(DataSet &bigData, StringInt *listPat);
  DataSet(DataSet &master, int *indPat, int nbEx);
  DataSet(DataSet &data1, DataSet &data2);

  void Del();
  void LightDel() { delete Set; }

  float *GetExample(int index) { return *(Set + index); }
  int GetNbEx() { return NbEx; }
  int GetNbAttr() { return NbAttr; }

  StringInt *Select(Rule *r);
  StringInt *Select(Rule *r, StringInt *subSet);

  void ExtractDataAndTarget(DataSet &data1, int nbAttr1, DataSet &data2,
                            int nbAttr2);
};

///////////////////////////////////////////////////////////////////

#endif
