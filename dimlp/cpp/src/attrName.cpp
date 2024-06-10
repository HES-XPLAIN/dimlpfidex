#include "attrName.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs the first read to count the number of attributes and classes.
 *
 * @return 1 if the number of attributes and classes matches the expected values, otherwise throws an error.
 */
int AttrName::FirstLect() const

{
  std::filebuf buf;
  std::string str;
  int k;

  if (buf.open(FileAttr, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open file of attribute names " + std::string(FileAttr));
  }

  std::istream inFile(&buf);

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

/**
 * @brief Performs the second read to actually store the attribute and class names.
 */
void AttrName::SecondLect() {
  std::filebuf buf;

  if (buf.open(FileAttr, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open file of attribute names " + std::string(FileAttr));
  }

  std::istream inFile(&buf);

  VarNames.clear();
  std::string varName;
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
  std::string className;
  bool hasClasses = false;

  while (std::getline(inFile, className)) {

    while (!className.empty() && (className.back() == '\n' || className.back() == '\r')) { // Remove line breaks
      className.pop_back();
    }

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

/**
 * @brief Reads the attribute and class names from the file.
 *
 * @return 1 if successful, 0 otherwise.
 */
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

/**
 * @brief Constructs an AttrName object.
 *
 * @param fileAttr The file containing attribute and class names.
 * @param nbAttr The number of attributes.
 * @param nbClasses The number of classes.
 */
AttrName::AttrName(const std::string &fileAttr, int nbAttr, int nbClasses) : NbAttr(nbAttr), NbClasses(nbClasses), FileAttr(fileAttr)

{
}

///////////////////////////////////////////////////////////////////
