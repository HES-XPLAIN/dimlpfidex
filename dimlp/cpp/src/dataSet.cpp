#include "dataSet.h"
using namespace std;

///////////////////////////////////////////////////////////////////

void DataSet::InsertExample(const std::vector<float> &example, int index) {
  float **newEx = Set + index;

  *newEx = new float[NbAttr];

  for (int i = 0; i < NbAttr; i++)
    (*newEx)[i] = example[i];
}

///////////////////////////////////////////////////////////////////

int DataSet::FirstLecture(const std::string &nameFile) const

{
  filebuf buf;
  int count;
  float x;
  std::cout << "\n----------------------------------------------------------\n"
            << std::endl;
  if (buf.open(nameFile, ios_base::in) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open input file " + nameFile);
  }

  istream inFile(&buf);

  std::cout << nameFile << ": "
            << "Reading ..." << std::endl;

  count = 0;
  while (inFile >> x) {
    count++;
  }

  if ((inFile.rdstate() == ifstream::badbit) || (inFile.rdstate() == ifstream::failbit)) {
    throw FileContentError("Error : File position " + to_string(count + 1) + ": problem in input file " + nameFile);
  }

  if (count % NbAttr != 0) {
    throw FileContentError("Error : Possible wrong number of attributes in file " + nameFile);
  }

  std::cout << nameFile << ": "
            << "Read.\n"
            << endl;

  return count / NbAttr;
}

///////////////////////////////////////////////////////////////////

void DataSet::SecondLecture(const std::string &nameFile) {
  filebuf buf;
  std::vector<float> oneExample;

  std::cout << nameFile << ": "
            << "Creating dataset structures ..." << std::endl;

  buf.open(nameFile, ios_base::in);
  istream inFile(&buf);

  oneExample.resize(NbAttr);

  for (int count = 0; count < NbEx; count++) {
    for (int attr = 0; attr < NbAttr; attr++)
      inFile >> oneExample[attr];

    InsertExample(oneExample, count);
  }

  std::cout << nameFile << ": "
            << "Dataset structures created." << std::endl;
}

///////////////////////////////////////////////////////////////////

void DataSet::Del() {
  for (int p = 0; p < NbEx; p++) {
    // Use delete[] to free the memory allocated for each array of attributes
    // This corresponds to the allocation with new float[NbAttr] for each example
    delete[] Set[p];
  }

  // Free the memory for the array of pointers itself
  // This corresponds to the allocation with new float *[NbEx]
  delete[] Set;

  // Set the pointer to nullptr to avoid dangling pointers
  Set = nullptr;

  // Reset the number of examples and attributes in the dataset to 0
  NbEx = 0;
  NbAttr = 0;
}

///////////////////////////////////////////////////////////////////

std::shared_ptr<StringInt> DataSet::Select(std::shared_ptr<DimlpRule> r)

{
  int a;
  int p;
  int nextPat;
  float **ptrPat;
  int nbAnt = r->GetNbAnt();
  auto savePat = std::make_shared<StringInt>();

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

std::shared_ptr<StringInt> DataSet::Select(std::shared_ptr<DimlpRule> r, std::shared_ptr<StringInt> subSet)

{
  int a;
  int p;
  int indPat;
  int nextPat;
  float **ptrPat;

  int nbAnt = r->GetNbAnt();
  int nbEx = subSet->GetNbEl();
  auto savePat = std::make_shared<StringInt>();

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
  Set = new float *[NbEx];
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(const std::string &nameFile, int nbAttr) : NbAttr(nbAttr)

{
  NbEx = FirstLecture(nameFile);
  Set = new float *[NbEx];

  std::cout << "Number of patterns in file " << nameFile << ": ";
  std::cout << NbEx << endl;

  SecondLecture(nameFile);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Initlialize dataset. Reads the data file, determines the number of attributes per sample, and creates the dataset.
 *        Validates the format of class data based on its length and converts class IDs to one-hot encoding if necessary.
 *
 * @param nameFile Path to the data file.
 * @param nbIn Number of input attributes (attributes).
 * @param nbOut Number of output attributes (class labels).
 * @throws FileContentError If there is a problem with the file format or content.
 */
DataSet::DataSet(const std::string &nameFile, int nbIn, int nbOut) {
  filebuf buf;
  float x;
  std::vector<float> lineValues;

  std::cout << "\n----------------------------------------------------------\n"
            << std::endl;
  if (buf.open(nameFile, ios_base::in) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open input file " + nameFile);
  }

  istream inFile(&buf);
  cout << nameFile << ": Reading and creating dataset structures ..." << std::endl;

  string line;
  std::vector<std::vector<float>> tempSet; // Use a temporary vector to store data

  int lineSize = -1;

  while (getline(inFile, line)) {
    std::istringstream lineStream(line);
    lineValues.clear();
    while (lineStream >> x) {
      lineValues.push_back(x);
    }
    if (lineStream.fail() && !lineStream.eof()) {
      throw FileContentError("Error : Non number found in file " + nameFile + ".");
    }

    auto currentLineSize = static_cast<int>(lineValues.size());

    if (lineSize != -1 && currentLineSize != lineSize) {
      throw FileContentError("Error : Inconsistent line lengths in file " + nameFile + ".");
    }

    // Apply specific checks based on currentLineSize
    if (currentLineSize == 1) {
      if (NbAttr == 0) {
        NbAttr = nbOut;
        lineSize = currentLineSize;
      }
      if (lineValues.back() != std::floor(lineValues[0])) {
        throw FileContentError("Error : Class ID is not an integer in file " + nameFile + ".");
      }
      auto classID = static_cast<int>(lineValues[0]);
      if (classID < 0 || classID >= nbOut) {
        throw FileContentError("Error : Class ID out of range in file " + nameFile + ".");
      }
    } else if (currentLineSize == nbIn + 1) {
      if (NbAttr == 0) {
        NbAttr = nbIn + nbOut;
        lineSize = currentLineSize;
      }
      if (lineValues.back() != std::floor(lineValues.back())) {
        throw FileContentError("Error : Class ID is not an integer in file " + nameFile + ".");
      }
      auto classID = static_cast<int>(lineValues.back());
      if (classID < 0 || classID >= nbOut) {
        throw FileContentError("Error : Class ID out of range in file " + nameFile + ".");
      }
    } else if (currentLineSize == nbOut || currentLineSize == nbIn + nbOut) {
      if (NbAttr == 0) {
        NbAttr = currentLineSize;
        lineSize = currentLineSize;
      }
      auto oneHotCount = static_cast<int>(std::count(lineValues.end() - nbOut, lineValues.end(), 1.0f));
      if (oneHotCount != 1 || std::count_if(lineValues.end() - nbOut, lineValues.end(), [](float val) { return val != 0.0f && val != 1.0f; }) > 0) {
        throw FileContentError("Error : Invalid one-hot encoding in file " + nameFile);
      }
    } else if (currentLineSize == nbIn) {
      if (NbAttr == 0) {
        NbAttr = currentLineSize;
        lineSize = currentLineSize;
      }
    } else {
      throw FileContentError("Error : Invalid line length in file " + nameFile);
    }

    // Convert class ID format to one-hot format if necessary
    if (lineValues.size() == 1 || lineValues.size() == nbIn + 1) {
      std::vector<float> oneHot(nbOut, 0.0f);
      auto classID = static_cast<int>(lineValues.back()); // Obtain class ID
      oneHot[classID] = 1.0f;

      if (lineValues.size() == nbIn + 1) {
        lineValues.pop_back();                                             // Remove class ID
        lineValues.insert(lineValues.end(), oneHot.begin(), oneHot.end()); // Append one-hot vector
      } else {
        lineValues = oneHot; // Replace with one-hot vector
      }
    }

    tempSet.push_back(lineValues);
  }

  cout << nameFile << ": Read.\n"
       << endl;

  // Create the final Set based on the temporary vector
  NbEx = static_cast<int>(tempSet.size());
  Set = new float *[NbEx];

  for (int i = 0; i < NbEx; ++i) {
    Set[i] = new float[NbAttr];
    std::copy(tempSet[i].begin(), tempSet[i].end(), Set[i]);
  }

  cout << "Number of patterns in file " << nameFile << ": ";
  cout << NbEx << endl;
}

///////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &bigData, StringInt *listPat) : NbEx(listPat->GetNbEl()), NbAttr(bigData.GetNbAttr())

{
  int p;
  const float *ptrEx;
  float *ptrSub;

  Set = new float *[NbEx];

  for (p = 0, listPat->GoToBeg(); p < NbEx; p++, listPat->GoToNext()) {
    Set[p] = new float[NbAttr];
    ptrSub = Set[p];
    ptrEx = bigData.GetExample(listPat->GetVal());

    for (int i = 0; i < NbAttr; i++, ptrSub++, ptrEx++)
      *ptrSub = *ptrEx;
  }
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &data1, DataSet &data2) : NbEx(data1.GetNbEx() + data2.GetNbEx()), NbAttr(data1.GetNbAttr())

{
  int i;

  Set = new float *[NbEx];

  for (i = 0; i < data1.GetNbEx(); i++)
    Set[i] = data1.GetExample(i);

  for (int j = 0; j < data2.GetNbEx(); i++, j++)
    Set[i] = data2.GetExample(j);
}

////////////////////////////////////////////////////////////////////////

DataSet::DataSet(DataSet &master, const int *indPat, int nbEx) : NbEx(nbEx), NbAttr(master.GetNbAttr()), Set(new float *[nbEx])

{
  for (int i = 0; i < nbEx; i++)
    Set[i] = master.GetExample(indPat[i]);
}

////////////////////////////////////////////////////////////////////////

void DataSet::ExtractDataAndTarget(
    DataSet &data1, int nbAttr1, DataSet &data2, int nbAttr2) const {
  int j;
  const float *ptr;
  float *ptrD1;
  float *ptrD2;
  std::vector<float> vecData1(nbAttr1);
  std::vector<float> vecData2(nbAttr2);

  data1.SetNbAttr(nbAttr1);
  data2.SetNbAttr(nbAttr2);

  bool hasClass;

  for (int p = 0; p < NbEx; p++) {
    ptr = Set[p];
    ptrD1 = vecData1.data();
    ptrD2 = vecData2.data();

    for (j = 0; j < nbAttr1; j++, ptrD1++, ptr++)
      *ptrD1 = *ptr;

    data1.InsertExample(vecData1, p);

    for (j = 0; j < nbAttr2; j++, ptrD2++, ptr++)
      *ptrD2 = *ptr;

    hasClass = false;
    for (float i : vecData2) {
      if (static_cast<int>(i) != 0) {
        hasClass = true;
      }
    }
    if (!hasClass) {
      throw FileContentError("Error : No class file given, and there is no class in data file.");
    }

    data2.InsertExample(vecData2, p);
  }
}

////////////////////////////////////////////////////////////////////////
