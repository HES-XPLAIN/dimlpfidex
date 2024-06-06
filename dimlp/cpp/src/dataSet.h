#ifndef DATASET_H
#define DATASET_H

#include "../../../common/cpp/src/checkFun.h"
#include "../../../common/cpp/src/errorHandler.h"
#include "../../../common/cpp/src/stringI.h"
#include "dimlpRule.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

///////////////////////////////////////////////////////////////////

/**
 * @brief Represents a dataset for the Dimlp algorithm.
 *
 * This class handles reading datasets from files, storing them, and providing methods to manipulate and access the data.
 */
class DataSet {
  int NbEx = 0;   ///< Number of examples in the dataset.
  int NbAttr = 0; ///< Number of attributes in the dataset.

  float **Set = nullptr; ///< Pointer to the array of dataset examples.

  // ------------------------------------------------------------------

  /**
   * @brief First pass to read the dataset file and count the number of examples.
   */
  int FirstLecture(const std::string &nameFile) const;

  /**
   * @brief Second pass to read the dataset file and populate the dataset.
   */
  void SecondLecture(const std::string &nameFile);

  /**
   * @brief Set the number of attributes.
   *
   * @param nbAttr Number of attributes.
   */
  void SetNbAttr(int nbAttr) { NbAttr = nbAttr; }

  /**
   * @brief Insert an example into the dataset.
   */
  void InsertExample(const std::vector<float> &example, int index);

  // ------------------------------------------------------------------

public:
  /**
   * @brief Default constructor.
   */
  DataSet() = default;

  /**
   * @brief Construct a DataSet with a specified number of examples.
   */
  explicit DataSet(int nbEx);

  /**
   * @brief Construct a DataSet from a file with a specified number of attributes.
   */
  DataSet(const std::string &nameFile, int nbAttr);

  /**
   * @brief Construct a DataSet from a file with a specified number of attributes and classes.
   */
  DataSet(const std::string &nameFile, int nbIn, int nbOut);

  /**
   * @brief Construct a DataSet from a larger dataset and a list of pattern indices.
   */
  DataSet(DataSet &bigData, StringInt *listPat);

  /**
   * @brief Construct a DataSet from a master dataset and an array of pattern indices.
   */
  DataSet(DataSet &master, const int *indPat, int nbEx);

  /**
   * @brief Construct a DataSet by merging two datasets.
   */
  DataSet(DataSet &data1, DataSet &data2);

  /**
   * @brief Delete the dataset and free memory.
   */
  void Del();

  /**
   * @brief Get an example from the dataset.
   *
   * @param index Index of the example.
   * @return Pointer to the example.
   */
  float *GetExample(int index) { return *(Set + index); }

  /**
   * @brief Get the number of examples.
   *
   * @return Number of examples.
   */
  int GetNbEx() const { return NbEx; }

  /**
   * @brief Get the number of attributes.
   *
   * @return Number of attributes.
   */
  int GetNbAttr() const { return NbAttr; }

  /**
   * @brief Select examples from the dataset based on a given rule.
   */
  std::shared_ptr<StringInt> Select(std::shared_ptr<DimlpRule> r);

  /**
   * @brief Select examples from a subset of the dataset based on a given rule.
   */
  std::shared_ptr<StringInt> Select(std::shared_ptr<DimlpRule> r, std::shared_ptr<StringInt> subSet);

  /**
   * @brief Extract data and target (class) attributes from the current dataset.
   */
  void ExtractDataAndTarget(DataSet &data1, int nbAttr1,
                            DataSet &data2, int nbAttr2) const;
};

///////////////////////////////////////////////////////////////////

#endif // DATASET_H
