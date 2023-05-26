#include "dataSet.h"

using namespace std;
///////////////////////////////////////////////////////////////////

void DataSet::InsertExample(const vector<float> &example, int index)

{
  vector<float> &newEx = Set[index];
  newEx.resize(NbAttr);

  for (int i = 0; i < NbAttr; i++) {
    newEx[i] = example[i];
  }
}

///////////////////////////////////////////////////////////////////

int DataSet::FirstLecture(const char nameFile[]) const

{
  filebuf buf;
  int count;
  float x;
  cout << "\n----------------------------------------------------------\n\n";
  if (buf.open(nameFile, ios_base::in) == nullptr) {
    string errorMsg = "Cannot open input file";
    WriteError(errorMsg, nameFile);
  }

  istream inFile(&buf);

  cout << nameFile << ": "
       << "Reading ..." << endl;

  count = 0;
  while (inFile >> x) {
    count++;
  }

  if ((inFile.rdstate() == ifstream::badbit) || (inFile.rdstate() == ifstream::failbit)) {
    cerr << "File position " << count + 1 << ": ";
    string errorMsg = "problem in input file";
    WriteError(errorMsg, nameFile);
  }

  if (count % NbAttr != 0) {
    string errorMsg = "Possible wrong number of attributes in file";
    WriteError(errorMsg, nameFile);
  }

  cout << nameFile << ": "
       << "Read.\n"
       << endl;

  return count / NbAttr;
}

///////////////////////////////////////////////////////////////////

void DataSet::SecondLecture(const char nameFile[])

{
  filebuf buf;
  vector<float> oneExample(NbAttr);

  cout << nameFile << ": "
       << "Creating dataset structures ..." << endl;

  buf.open(nameFile, ios_base::in);
  istream inFile(&buf);

  for (int count = 0; count < NbEx; count++) {
    for (int attr = 0; attr < NbAttr; attr++) {
      inFile >> oneExample[attr];
    }
    InsertExample(oneExample, count);
  }

  cout << nameFile << ": "
       << "Dataset structures created." << endl;
}

///////////////////////////////////////////////////////////////////

void DataSet::Del()

{
  /*
  for (int p = 0; p < NbEx; p++)
    delete Set[p];

  delete Set;
  */
}

///////////////////////////////////////////////////////////////////

StringInt *DataSet::Select(Rule *r)

{
  int a;
  int nextPat;
  int nbAnt = r->GetNbAnt();

  auto savePat = new StringInt;

  for (int p = 0; p < NbEx; p++) {
    const vector<float> &example = Set[p];

    for (a = 0, nextPat = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
      if (r->IsAntDeleted() == 1)
        continue;

      switch (r->GetRel()) {
      case '<':
        if (example[r->GetVar()] < r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }

      case '>':
        if (example[r->GetVar()] >= r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }
      default:
        break;
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
  int a;
  int p;
  int indPat;
  int nextPat;

  int nbAnt = r->GetNbAnt();
  int nbEx = subSet->GetNbEl();
  auto savePat = new StringInt;

  for (p = 0, subSet->GoToBeg(); p < nbEx; p++, subSet->GoToNext()) {
    indPat = subSet->GetVal();
    const vector<float> &example = Set[indPat];

    for (a = 0, nextPat = 0, r->GoToBeg(); a < nbAnt; a++, r->GoToNext()) {
      if (r->IsAntDeleted() == 1)
        continue;

      switch (r->GetRel()) {
      case '<':
        if (example[r->GetVar()] < r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }

      case '>':
        if (example[r->GetVar()] >= r->GetVal())
          continue;
        else {
          nextPat = 1;
          break;
        }

      default:
        break;
      }

      if (nextPat == 1)
        break;
    }

    if (a == nbAnt)
      savePat->Insert(indPat);
  }

  return savePat;
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(int nbEx) : NbEx(nbEx)

{
  Set.resize(NbEx);
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(const char nameFile[], int nbAttr) : NbAttr(nbAttr)

{
  NbEx = FirstLecture(nameFile);
  Set.resize(NbEx);

  cout << "Number of patterns in file " << nameFile << ": ";
  cout << NbEx << endl;

  SecondLecture(nameFile);
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &bigData, StringInt *listPat) : NbEx(listPat->GetNbEl()), NbAttr(bigData.GetNbAttr())

{
  int p;

  Set.resize(NbEx);

  for (p = 0, listPat->GoToBeg(); p < NbEx; p++, listPat->GoToNext()) {
    Set[p].resize(NbAttr);
    vector<float> &subSet = Set[p];
    const vector<float> &example = bigData.GetExample(listPat->GetVal());

    for (int i = 0; i < NbAttr; i++) {
      subSet[i] = example[i];
    }
  }
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &data1, DataSet &data2) : NbEx(data1.GetNbEx() + data2.GetNbEx()), NbAttr(data1.GetNbAttr())

{
  int i;

  Set.resize(NbEx);

  for (i = 0; i < data1.GetNbEx(); i++)
    Set[i] = data1.GetExample(i);

  for (int j = 0; j < data2.GetNbEx(); i++, j++)
    Set[i] = data2.GetExample(j);

  // cout << "Created merged dataset with " << NbEx << " examples." << "\n";
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &master, int *indPat, int nbEx) : NbEx(nbEx), NbAttr(master.GetNbAttr())

{
  Set.resize(NbEx);

  for (int i = 0; i < nbEx; i++)
    Set[i] = master.GetExample(indPat[i]);
}

////////////////////////////////////////////////////////////////////////

void DataSet::ExtractDataAndTarget(
    DataSet &data1, int nbAttr1, DataSet &data2, int nbAttr2) {
  int j;
  vector<float> vecData1(nbAttr1);
  vector<float> vecData2(nbAttr2);

  data1.SetNbAttr(nbAttr1);
  data2.SetNbAttr(nbAttr2);

  for (int p = 0; p < NbEx; p++) {
    const vector<float> &example = Set[p];
    vector<float> &data1Example = vecData1;
    vector<float> &data2Example = vecData2;

    for (j = 0; j < nbAttr1; j++)
      data1Example[j] = example[j];

    data1.InsertExample(data1Example, p);

    for (j = 0; j < nbAttr2; j++)
      data2Example[j] = example[j + nbAttr1];

    data2.InsertExample(data2Example, p);
  }
}

////////////////////////////////////////////////////////////////////////
