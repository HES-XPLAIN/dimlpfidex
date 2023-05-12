using namespace std;
#include "dataSet.h"
#include "writeErr.h"
#include <fstream>
#include <iostream>
///////////////////////////////////////////////////////////////////

void DataSet::InsertExample(float example[], int index)

{
  int i;
  float **newEx = Set + index;

  *newEx = new float[NbAttr];

  for (i = 0; i < NbAttr; i++)
    *(*newEx + i) = example[i];
}

///////////////////////////////////////////////////////////////////

int DataSet::FirstLecture(char nameFile[])

{
  filebuf buf;
  int count;
  float x;
  cout << "\n----------------------------------------------------------\n\n";
  if (buf.open(nameFile, ios_base::in) == 0) {
    char errorMsg[] = "Cannot open input file";
    WriteError(errorMsg, nameFile);
  }

  istream inFile(&buf);

  cout << nameFile << ": "
       << "Reading ..." << endl;

  for (count = 0; (inFile >> x); count++)
    ;

  if ((inFile.rdstate() == ifstream::badbit) || (inFile.rdstate() == ifstream::failbit)) {
    cerr << "File position " << count + 1 << ": ";
    char errorMsg[] = "problem in input file";
    WriteError(errorMsg, nameFile);
  }

  if (count % NbAttr != 0) {
    char errorMsg[] = "Possible wrong number of attributes in file";
    WriteError(errorMsg, nameFile);
  }

  cout << nameFile << ": "
       << "Read.\n"
       << endl;

  return count / NbAttr;
}

///////////////////////////////////////////////////////////////////

void DataSet::SecondLecture(char nameFile[])

{
  filebuf buf;
  float *oneExample;
  float *ptr;
  int count, attr;

  cout << nameFile << ": "
       << "Creating dataset structures ..." << endl;

  buf.open(nameFile, ios_base::in);
  istream inFile(&buf);

  oneExample = new float[NbAttr];

  for (count = 0; count < NbEx; count++) {
    ptr = oneExample;

    for (attr = 0; attr < NbAttr; attr++, ptr++)
      inFile >> *ptr;

    InsertExample(oneExample, count);
  }

  delete oneExample;

  cout << nameFile << ": "
       << "Dataset structures created." << endl;
}

///////////////////////////////////////////////////////////////////

void DataSet::Del()

{
  int p;

  for (p = 0; p < NbEx; p++)
    delete Set[p];

  delete Set;
}

///////////////////////////////////////////////////////////////////

StringInt *DataSet::Select(Rule *r)

{
  int a, p, nextPat;
  float **ptrPat;
  int nbAnt = r->GetNbAnt();

  StringInt *savePat = new StringInt;

  for (p = 0, ptrPat = Set; p < NbEx; p++, ptrPat++) {
    for (a = 0, nextPat = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
      if (r->IsAntDeleted() == 1)
        continue;

      switch (r->GetRel()) {
      case '<':
        if (*(*ptrPat + r->GetVar()) < r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }

      case '>':
        if (*(*ptrPat + r->GetVar()) >= r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }
      }

      if (nextPat == 1)
        break;
    }

    if (a == nbAnt)
      savePat->Insert(p);
  }

  return savePat;
}

////////////////////////////////////////////////////////////////////////

StringInt *DataSet::Select(Rule *r, StringInt *subSet)

{
  int a, p, indPat, nextPat;
  float **ptrPat;

  int nbAnt = r->GetNbAnt();
  int nbEx = subSet->GetNbEl();
  StringInt *savePat = new StringInt;

  for (p = 0, subSet->GoToBeg(); p < nbEx; p++, subSet->GoToNext()) {
    indPat = subSet->GetVal();
    ptrPat = Set + indPat;

    for (a = 0, nextPat = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
      if (r->IsAntDeleted() == 1)
        continue;

      switch (r->GetRel()) {
      case '<':
        if (*(*ptrPat + r->GetVar()) < r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }

      case '>':
        if (*(*ptrPat + r->GetVar()) >= r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }
      }

      if (nextPat == 1)
        break;
    }

    if (a == nbAnt)
      savePat->Insert(indPat);
  }

  return savePat;
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet()

{
  Set = 0;
  NbEx = 0;
  NbAttr = 0;
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(int nbEx)

{
  NbEx = nbEx;
  Set = new float *[NbEx];
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(char nameFile[], int nbAttr)

{
  NbAttr = nbAttr;
  NbEx = FirstLecture(nameFile);
  Set = new float *[NbEx];

  cout << "Number of patterns in file " << nameFile << ": ";
  cout << NbEx << endl;

  SecondLecture(nameFile);
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &bigData, StringInt *listPat)

{
  int i, p;
  float *ptrEx;
  float *ptrSub;

  NbEx = listPat->GetNbEl();
  NbAttr = bigData.GetNbAttr();
  Set = new float *[NbEx];

  for (p = 0, listPat->GoToBeg(); p < NbEx; p++, listPat->GoToNext()) {
    Set[p] = new float[NbAttr];
    ptrSub = Set[p];
    ptrEx = bigData.GetExample(listPat->GetVal());

    for (i = 0; i < NbAttr; i++, ptrSub++, ptrEx++)
      *ptrSub = *ptrEx;
  }
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &data1, DataSet &data2)

{
  int i, j;

  NbAttr = data1.GetNbAttr();
  NbEx = data1.GetNbEx() + data2.GetNbEx();
  Set = new float *[NbEx];

  for (i = 0; i < data1.GetNbEx(); i++)
    Set[i] = data1.GetExample(i);

  for (j = 0; j < data2.GetNbEx(); i++, j++)
    Set[i] = data2.GetExample(j);

  // InsertExample(data2.GetExample(j), i);

  // cout << "Created merged dataset with " << NbEx << " examples." << "\n";
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &master, int *indPat, int nbEx)

{
  int i;

  NbAttr = master.GetNbAttr();
  NbEx = nbEx;
  Set = new float *[nbEx];

  for (i = 0; i < nbEx; i++)
    Set[i] = master.GetExample(indPat[i]);
}

////////////////////////////////////////////////////////////////////////

void DataSet::ExtractDataAndTarget(
    DataSet &data1, int nbAttr1, DataSet &data2, int nbAttr2)

{
  int p, j;
  float *ptr;
  float *ptrD1;
  float *ptrD2;
  float *vecData1;
  float *vecData2;

  vecData1 = new float[nbAttr1];
  vecData2 = new float[nbAttr2];

  data1.SetNbAttr(nbAttr1);
  data2.SetNbAttr(nbAttr2);

  for (p = 0; p < NbEx; p++) {
    ptr = Set[p];
    ptrD1 = vecData1;
    ptrD2 = vecData2;

    for (j = 0; j < nbAttr1; j++, ptrD1++, ptr++)
      *ptrD1 = *ptr;

    data1.InsertExample(vecData1, p);

    for (j = 0; j < nbAttr2; j++, ptrD2++, ptr++)
      *ptrD2 = *ptr;

    data2.InsertExample(vecData2, p);
  }

  delete vecData1;
  delete vecData2;
}

////////////////////////////////////////////////////////////////////////
