#include "dataSet.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Insert an example into the dataset.
 *
 * @param example Vector of attribute values.
 * @param index Index at which to insert the example.
 */
void DataSet::InsertExample(const std::vector<float> &example, int index) {
  float **newEx = Set + index;

  *newEx = new float[NbAttr];

  for (int i = 0; i < NbAttr; i++)
    (*newEx)[i] = example[i];
}

///////////////////////////////////////////////////////////////////

/**
 * @brief First pass to read the dataset file and count the number of examples.
 *
 * @param nameFile Path to the dataset file.
 * @return The number of examples in the file.
 * @throws CannotOpenFileError If the file cannot be opened.
 * @throws FileContentError If there is an issue with the file content.
 */
int DataSet::FirstLecture(const std::string &nameFile) const

{
  std::filebuf buf;
  int count;
  float x;
  std::cout << "\n----------------------------------------------------------\n"
            << std::endl;
  if (buf.open(nameFile, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open input file " + nameFile);
  }

  std::istream inFile(&buf);

  std::cout << nameFile << ": "
            << "Reading ..." << std::endl;

  count = 0;
  while (inFile >> x) {
    count++;
  }

  if ((inFile.rdstate() == std::ifstream::badbit) || (inFile.rdstate() == std::ifstream::failbit)) {
    throw FileContentError("Error : File position " + std::to_string(count + 1) + ": problem in input file " + nameFile);
  }

  if (count % NbAttr != 0) {
    throw FileContentError("Error : Possible wrong number of attributes in file " + nameFile);
  }

  std::cout << nameFile << ": "
            << "Read.\n"
            << std::endl;

  return count / NbAttr;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Second pass to read the dataset file and populate the dataset.
 *
 * @param nameFile Path to the dataset file.
 */
void DataSet::SecondLecture(const std::string &nameFile) {
  std::filebuf buf;
  std::vector<float> oneExample;

  std::cout << nameFile << ": "
            << "Creating dataset structures ..." << std::endl;

  buf.open(nameFile, std::ios_base::in);
  std::istream inFile(&buf);

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

/**
 * @brief Delete the dataset and free memory.
 */
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

/**
 * @brief Select examples from the dataset based on a given rule.
 *
 * @param r Pointer to the rule.
 * @return Pointer to a StringInt object containing the selected example indices.
 */
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

/**
 * @brief Select examples from a subset of the dataset based on a given rule.
 *
 * @param r Pointer to the rule.
 * @param subSet Pointer to a StringInt object containing the subset of example indices.
 * @return Pointer to a StringInt object containing the selected example indices.
 */
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

/**
 * @brief Construct a DataSet with a specified number of examples.
 *
 * @param nbEx Number of examples.
 */
DataSet::DataSet(int nbEx) : NbEx(nbEx)

{
  Set = new float *[NbEx];
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Construct a DataSet from a file with a specified number of attributes.
 *
 * @param nameFile Path to the dataset file.
 * @param nbAttr Number of attributes.
 */
DataSet::DataSet(const std::string &nameFile, int nbAttr) : NbAttr(nbAttr)

{
  NbEx = FirstLecture(nameFile);
  Set = new float *[NbEx];

  std::cout << "Number of patterns in file " << nameFile << ": ";
  std::cout << NbEx << std::endl;

  SecondLecture(nameFile);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Construct a DataSet from a file with a specified number of attributes and classes.
 *
 * @param nameFile Path to the data file.
 * @param nbIn Number of input attributes (attributes).
 * @param nbOut Number of output attributes (class labels).
 * @throws FileContentError If there is a problem with the file format or content.
 */
DataSet::DataSet(const std::string &nameFile, int nbIn, int nbOut) {
  std::filebuf buf;
  std::vector<double> lineValues;

  std::cout << "\n----------------------------------------------------------\n"
            << std::endl;
  if (buf.open(nameFile, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Error : Cannot open input file " + nameFile);
  }

  std::istream inFile(&buf);
  std::cout << nameFile << ": Reading and creating dataset structures ..." << std::endl;

  std::string line;
  std::vector<std::vector<double>> tempSet; // Use a temporary vector to store data

  int lineSize = -1;

  while (getline(inFile, line)) {

    lineValues = parseFileLine(line, nameFile);
    auto currentLineSize = static_cast<int>(lineValues.size());

    if (lineSize != -1 && currentLineSize != lineSize) {
      throw FileContentError("Error : Inconsistent line lengths in file " + nameFile + ", line : " + line + ".");
    }

    // Apply specific checks based on currentLineSize
    if (currentLineSize == 1) {
      if (NbAttr == 0) {
        NbAttr = nbOut;
        lineSize = currentLineSize;
      }
      if (lineValues.back() != std::floor(lineValues[0])) {
        throw FileContentError("Error : Class ID (" + std::to_string(lineValues.back()) + ") is not an integer in file " + nameFile + ".");
      }
      auto classID = static_cast<int>(lineValues[0]);
      if (classID < 0 || classID >= nbOut) {
        throw FileContentError("Error : Class ID (" + std::to_string(classID) + ") out of range in file " + nameFile + ".");
      }
    } else if (currentLineSize == nbIn + 1) {
      if (NbAttr == 0) {
        NbAttr = nbIn + nbOut;
        lineSize = currentLineSize;
      }
      if (lineValues.back() != std::floor(lineValues.back())) {
        throw FileContentError("Error : Class ID (" + std::to_string(lineValues.back()) + ") is not an integer in file " + nameFile + ".");
      }
      auto classID = static_cast<int>(lineValues.back());
      if (classID < 0 || classID >= nbOut) {
        throw FileContentError("Error : Class ID (" + std::to_string(classID) + ") out of range in file " + nameFile + ".");
      }
    } else if (currentLineSize == nbOut || currentLineSize == nbIn + nbOut) {
      if (NbAttr == 0) {
        NbAttr = currentLineSize;
        lineSize = currentLineSize;
      }
      auto oneHotCount = static_cast<int>(std::count(lineValues.end() - nbOut, lineValues.end(), 1.0f));
      if (oneHotCount != 1 || std::count_if(lineValues.end() - nbOut, lineValues.end(), [](double val) { return val != 0.0 && val != 1.0; }) > 0) {
        throw FileContentError("Error : Invalid one-hot encoding in file " + nameFile + ", line : " + line + ".");
      }
    } else if (currentLineSize == nbIn) {
      if (NbAttr == 0) {
        NbAttr = currentLineSize;
        lineSize = currentLineSize;
      }
    } else {
      throw FileContentError("Error : Invalid line length in file " + nameFile + ", line : " + line + ".");
    }

    // Convert class ID format to one-hot format if necessary
    if (lineValues.size() == 1 || lineValues.size() == nbIn + 1) {
      std::vector<double> oneHot(nbOut, 0.0f);
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

  std::cout << nameFile << ": Read.\n"
            << std::endl;

  // Create the final Set based on the temporary vector
  NbEx = static_cast<int>(tempSet.size());
  Set = new float *[NbEx];

  for (int i = 0; i < NbEx; ++i) {
    Set[i] = new float[NbAttr];
    std::transform(tempSet[i].begin(), tempSet[i].end(), Set[i], [](double val) { return static_cast<float>(val); });
  }

  std::cout << "Number of patterns in file " << nameFile << ": ";
  std::cout << NbEx << std::endl;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Construct a DataSet from a larger dataset and a list of pattern indices.
 *
 * @param bigData Larger dataset.
 * @param listPat List of pattern indices.
 */
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

/**
 * @brief Construct a DataSet by merging two datasets.
 *
 * @param data1 First dataset.
 * @param data2 Second dataset.
 */
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

/**
 * @brief Construct a DataSet from a master dataset and an array of pattern indices.
 *
 * @param master Master dataset.
 * @param indPat Array of pattern indices.
 * @param nbEx Number of examples.
 */
DataSet::DataSet(DataSet &master, const int *indPat, int nbEx) : NbEx(nbEx), NbAttr(master.GetNbAttr()), Set(new float *[nbEx])

{
  for (int i = 0; i < nbEx; i++)
    Set[i] = master.GetExample(indPat[i]);
}

////////////////////////////////////////////////////////////////////////

/**
 * @brief Extract data and target (class) attributes from the current dataset.
 *
 * @param data1 Dataset for the attributes.
 * @param nbAttr1 Number of attributes.
 * @param data2 Dataset for the targets (classes).
 * @param nbAttr2 Number of targets (classes).
 * @throws FileContentError If the class data is missing or invalid.
 */
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
