#ifndef ATTRNAME_H
#define ATTRNAME_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/errorHandler.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

/**
 * @brief The AttrName class handles the reading and storage of attribute and class names from a file.
 */
class AttrName {
  int NbAttr;    ///< Number of attributes.
  int NbClasses; ///< Number of classes.

  std::string FileAttr;                ///< File containing attribute and class names.
  std::vector<std::string> VarNames;   ///< List of attribute names.
  std::vector<std::string> ClassNames; ///< List of class names.

  /**
   * @brief Performs the first read to count the number of attributes and classes.
   */
  int FirstLect() const;

  /**
   * @brief Performs the second read to actually store the attribute and class names.
   */
  void SecondLect();

public:
  /**
   * @brief Reads the attribute and class names from the file.
   */
  int ReadAttr();

  /**
   * @brief Checks if the attribute file is set.
   *
   * @return 1 if the file is set, 0 otherwise.
   */
  int IsFileAttr() const { return (FileAttr.empty() ? 0 : 1); }

  /**
   * @brief Gets the list of attribute names.
   *
   * @return A vector of attribute names.
   */
  std::vector<std::string> GetListAttr() const { return VarNames; }

  /**
   * @brief Gets the list of class names.
   *
   * @return A vector of class names.
   */
  std::vector<std::string> GetListClasses() const { return ClassNames; }

  /**
   * @brief Constructs an AttrName object.
   */
  AttrName(const std::string &fileAttr, int nbAttr, int nbClasses);

  /**
   * @brief Default constructor.
   */
  AttrName() = default;
};

#endif // ATTRNAME_H
