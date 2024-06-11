#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedent.h"
#include "checkFun.h"
#include "errorHandler.h"
#include "stringI.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Enumeration of parameter codes used to identify different parameters.
 */
enum ParameterCode { // to add a new parameter, just add a new parameter code BEFORE "_NB_PARAMETERS"
  TRAIN_DATA_FILE,
  TRAIN_PRED_FILE,
  TRAIN_PRED_OUTFILE,
  TRAIN_CLASS_FILE,
  TEST_DATA_FILE,
  TEST_PRED_FILE,
  TEST_PRED_OUTFILE,
  TEST_CLASS_FILE,
  VALID_DATA_FILE,
  VALID_CLASS_FILE,
  VALID_PRED_OUTFILE,
  RULES_FILE,
  RULES_OUTFILE,
  GLOBAL_RULES_OUTFILE,
  GLOBAL_RULES_FILE,
  EXPLANATION_FILE,
  CONSOLE_FILE,
  ROOT_FOLDER,
  ATTRIBUTES_FILE,
  WEIGHTS_FILE,
  WEIGHTS_OUTFILE,
  HID_FILE,
  STATS_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  NB_DIMLP_NETS,
  NB_EX_PER_NET,
  NB_QUANT_LEVELS,
  NB_FIDEX_RULES,
  HEURISTIC,
  MAX_ITERATIONS,
  MIN_COVERING,
  COVERING_STRATEGY,
  MAX_FAILED_ATTEMPTS,
  NB_THREADS,
  POSITIVE_CLASS_INDEX,
  SEED,
  DECISION_THRESHOLD,
  HI_KNOT,
  DROPOUT_HYP,
  DROPOUT_DIM,
  MIN_FIDELITY,
  LOWEST_MIN_FIDELITY,
  LEARNING_RATE,
  MOMENTUM,
  FLAT,
  ERROR_THRESH,
  ACC_THRESH,
  ABS_ERROR_THRESH,
  NB_EPOCHS,
  NB_EPOCHS_ERROR,
  WITH_RULE_EXTRACTION,
  NORMALIZATION_FILE,
  MUS,
  SIGMAS,
  NORMALIZATION_INDICES,
  WITH_FIDEX,
  WITH_MINIMAL_VERSION,
  FIRST_HIDDEN_LAYER,
  HIDDEN_LAYERS,
  HIDDEN_LAYERS_OUTFILE,
  HIDDEN_LAYERS_FILE,
  _NB_PARAMETERS // internal use only, do not consider it as a usable parameter
};

/**
 * @brief Maps parameter names to their corresponding ParameterCode values.
 */
static const std::unordered_map<std::string, ParameterCode> parameterNames = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"train_pred_file", TRAIN_PRED_FILE},
    {"train_pred_outfile", TRAIN_PRED_OUTFILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"test_pred_outfile", TEST_PRED_OUTFILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"valid_data_file", VALID_DATA_FILE},
    {"valid_class_file", VALID_CLASS_FILE},
    {"valid_pred_outfile", VALID_PRED_OUTFILE},
    {"rules_file", RULES_FILE},
    {"rules_outfile", RULES_OUTFILE},
    {"global_rules_outfile", GLOBAL_RULES_OUTFILE},
    {"global_rules_file", GLOBAL_RULES_FILE},
    {"explanation_file", EXPLANATION_FILE},
    {"console_file", CONSOLE_FILE},
    {"root_folder", ROOT_FOLDER},
    {"attributes_file", ATTRIBUTES_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"weights_outfile", WEIGHTS_OUTFILE},
    {"hid_file", HID_FILE},
    {"stats_file", STATS_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"nb_dimlp_nets", NB_DIMLP_NETS},
    {"nb_ex_per_net", NB_EX_PER_NET},
    {"nb_quant_levels", NB_QUANT_LEVELS},
    {"nb_fidex_rules", NB_FIDEX_RULES},
    {"heuristic", HEURISTIC},
    {"max_iterations", MAX_ITERATIONS},
    {"min_covering", MIN_COVERING},
    {"learning_rate", LEARNING_RATE},
    {"momentum", MOMENTUM},
    {"flat", FLAT},
    {"error_thresh", ERROR_THRESH},
    {"acc_thresh", ACC_THRESH},
    {"abs_error_thresh", ABS_ERROR_THRESH},
    {"nb_epochs", NB_EPOCHS},
    {"nb_epochs_error", NB_EPOCHS_ERROR},
    {"with_rule_extraction", WITH_RULE_EXTRACTION},
    {"covering_strategy", COVERING_STRATEGY},
    {"max_failed_attempts", MAX_FAILED_ATTEMPTS},
    {"nb_threads", NB_THREADS},
    {"positive_class_index", POSITIVE_CLASS_INDEX},
    {"seed", SEED},
    {"decision_threshold", DECISION_THRESHOLD},
    {"hi_knot", HI_KNOT},
    {"dropout_hyp", DROPOUT_HYP},
    {"dropout_dim", DROPOUT_DIM},
    {"min_fidelity", MIN_FIDELITY},
    {"lowest_min_fidelity", LOWEST_MIN_FIDELITY},
    {"normalization_file", NORMALIZATION_FILE},
    {"mus", MUS},
    {"sigmas", SIGMAS},
    {"normalization_indices", NORMALIZATION_INDICES},
    {"with_fidex", WITH_FIDEX},
    {"with_minimal_version", WITH_MINIMAL_VERSION},
    {"first_hidden_layer", FIRST_HIDDEN_LAYER},
    {"hidden_layers", HIDDEN_LAYERS},
    {"hidden_layers_outfile", HIDDEN_LAYERS_OUTFILE},
    {"hidden_layers_file", HIDDEN_LAYERS_FILE}};

/**
 * @brief The Parameters class manages configuration parameters for each program.
 *
 * This class handles the parsing, validation, and storage of parameters passed via
 * command-line arguments or a JSON configuration file. It supports various data types
 * and provides utility functions to ensure parameters are correctly set and valid.
 */
class Parameters {
private:
  std::map<ParameterCode, int> _intParams;                          ///< Integer parameters.
  std::map<ParameterCode, float> _floatParams;                      ///< Float parameters.
  std::map<ParameterCode, double> _doubleParams;                    ///< Double parameters.
  std::map<ParameterCode, bool> _boolParams;                        ///< Boolean parameters.
  std::map<ParameterCode, std::vector<double>> _doubleVectorParams; ///< Vector of double parameters.
  std::map<ParameterCode, std::vector<int>> _intVectorParams;       ///< Vector of integer parameters.
  std::map<ParameterCode, std::string> _stringParams;               ///< String parameters.
  std::vector<std::string> _weightFiles;                            ///< Weight files.

  // private parser
  /**
   * @brief Parses a given parameter name & its value.
   */
  void parseArg(const std::string &param, const std::string &arg, const std::vector<ParameterCode> &validParams);

  // path checker

  /**
   * @brief Ensures that every file/path is valid.
   */
  void checkFilesIntegrity();

  /**
   * @brief Checks if the specified path exists.
   */
  void checkPath(ParameterCode id, const std::string &path) const;

  /**
   * @brief Completes the path of a given parameter with the ROOT_FOLDER.
   */
  void completePath(ParameterCode id);

  //////////////////////////////////////////////////////

  // throwables

  /**
   * @brief Throws an exception for an invalid data type.
   */
  [[noreturn]] void throwInvalidDataTypeException(ParameterCode id, const std::string &wrongValue, const std::string &typeName) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".");
  }

  /**
   * @brief Throws an exception when an argument is already set.
   */
  [[noreturn]] void throwAlreadySetArgumentException(ParameterCode id, const std::string &value) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + value + "\" is already set, cannot override it.");
  }

  /**
   * @brief Throws an exception when an argument is not found.
   */
  [[noreturn]] void throwArgumentNotFoundException(ParameterCode id) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(id) + ") requested was not found, try to rerun including it.");
  }

  /**
   * @brief Throws an exception for an invalid file or directory.
   */
  [[noreturn]] void throwInvalidFileOrDirectory(ParameterCode id, const std::string &wrongValue) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid path. The directory or file specified could not be found.");
  }

  /**
   * @brief Throws an exception for an invalid parameter.
   */
  [[noreturn]] void throwInvalidParameter(ParameterCode wrongValue) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(wrongValue) + ") is not a valid parameter for this execution.");
  }

public:
  // constructor
  Parameters() = default;

  /**
   * @brief Construct a new Parameters object containing all arguments passed by CLI.
   */
  explicit Parameters(const std::vector<std::string> &args, const std::vector<ParameterCode> &validParams);

  /**
   * @brief Construct a new Parameters object containing all arguments passed by JSON config file.
   */
  explicit Parameters(const std::string &jsonfile, const std::vector<ParameterCode> &validParams);

  //////////////////////////////////////////////////////

  // setters handle formatting from string argument

  /**
   * @brief Sets an integer parameter from a string value.
   */
  void setInt(ParameterCode id, const std::string &value);

  /**
   * @brief Sets an integer parameter.
   */
  void setInt(ParameterCode id, int value);

  /**
   * @brief Sets a float parameter from a string value.
   */
  void setFloat(ParameterCode id, const std::string &value);

  /**
   * @brief Sets a float parameter.
   */
  void setFloat(ParameterCode id, float value);

  /**
   * @brief Sets a double parameter from a string value.
   */
  void setDouble(ParameterCode id, const std::string &value);

  /**
   * @brief Sets a double parameter.
   */
  void setDouble(ParameterCode id, double value);

  /**
   * @brief Sets a boolean parameter from a string value.
   */
  void setBool(ParameterCode id, std::string value);

  /**
   * @brief Sets a boolean parameter.
   */
  void setBool(ParameterCode id, bool value);

  /**
   * @brief Sets a double vector parameter from a string value.
   */
  void setDoubleVector(ParameterCode id, const std::string &value);

  /**
   * @brief Sets a double vector parameter.
   */
  void setDoubleVector(ParameterCode id, const std::vector<double> &value);

  /**
   * @brief Sets an integer vector parameter from a string value.
   */
  void setIntVector(ParameterCode id, const std::string &value);

  /**
   * @brief Sets an integer vector parameter.
   */
  void setIntVector(ParameterCode id, const std::vector<int> &value);

  /**
   * @brief Sets a string parameter.
   */
  void setString(ParameterCode id, const std::string &value);

  //////////////////////////////////////////////////////

  // default setter if value not set

  /**
   * @brief Sets a default integer value for the given parameter code if not already set.
   */
  void setDefaultInt(ParameterCode id, int value);

  /**
   * @brief Sets a default float value for the given parameter code if not already set.
   */
  void setDefaultFloat(ParameterCode id, float value);

  /**
   * @brief Sets a default double value for the given parameter code if not already set.
   */
  void setDefaultDouble(ParameterCode id, double value);

  /**
   * @brief Sets a default boolean value for the given parameter code if not already set.
   */
  void setDefaultBool(ParameterCode id, bool value);

  /**
   * @brief Sets a default double vector value for the given parameter code if not already set.
   */
  void setDefaultDoubleVector(ParameterCode id, const std::string &defaultValue);

  /**
   * @brief Sets a default integer vector value for the given parameter code if not already set.
   */
  void setDefaultIntVector(ParameterCode id, const std::string &defaultValue);

  /**
   * @brief Sets a default string value for the given parameter code if not already set.
   */
  void setDefaultString(ParameterCode id, const std::string &defaultValue, bool withRoot = false);

  //////////////////////////////////////////////////////

  // getters

  /**
   * @brief Gets the integer value for the given parameter code.
   */
  int getInt(ParameterCode id);

  /**
   * @brief Gets the float value for the given parameter code.
   */
  float getFloat(ParameterCode id);

  /**
   * @brief Gets the double value for the given parameter code.
   */
  double getDouble(ParameterCode id);

  /**
   * @brief Gets the boolean value for the given parameter code.
   */
  bool getBool(ParameterCode id);

  /**
   * @brief Gets the vector of double values for the given parameter code.
   */
  std::vector<double> getDoubleVector(ParameterCode id);

  /**
   * @brief Gets the vector of integer values for the given parameter code.
   */
  std::vector<int> getIntVector(ParameterCode id);

  /**
   * @brief Gets the string value for the given parameter code.
   */
  std::string getString(ParameterCode id);

  /**
   * @brief Gets the architecture of hidden layers.
   */
  StringInt getArch();

  /**
   * @brief Gets the indices of the architecture of hidden layers.
   */
  StringInt getArchInd();

  //////////////////////////////////////////////////////

  /**
   * @brief Gets all integer parameters.
   *
   * @return A map of all integer parameters.
   */
  std::map<ParameterCode, int> getAllInts() const { return _intParams; }

  /**
   * @brief Gets all float parameters.
   *
   * @return A map of all float parameters.
   */
  std::map<ParameterCode, float> getAllFloats() const { return _floatParams; }

  /**
   * @brief Gets all double parameters.
   *
   * @return A map of all double parameters.
   */
  std::map<ParameterCode, double> getAllDoubles() const { return _doubleParams; }

  /**
   * @brief Gets all boolean parameters.
   *
   * @return A map of all boolean parameters.
   */
  std::map<ParameterCode, bool> getAllBools() const { return _boolParams; }

  /**
   * @brief Gets all double vector parameters.
   *
   * @return A map of all double vector parameters.
   */
  std::map<ParameterCode, std::vector<double>> getAllDoubleVectors() const { return _doubleVectorParams; }

  /**
   * @brief Gets all integer vector parameters.
   *
   * @return A map of all integer vector parameters.
   */
  std::map<ParameterCode, std::vector<int>> getAllIntVectors() const { return _intVectorParams; }

  /**
   * @brief Gets all string parameters.
   *
   * @return A map of all string parameters.
   */
  std::map<ParameterCode, std::string> getAllStrings() const { return _stringParams; }

  //////////////////////////////////////////////////////

  /**
   * @brief Checks if an integer parameter is set.
   */
  bool isIntSet(ParameterCode id);

  /**
   * @brief Checks if a float parameter is set.
   */
  bool isFloatSet(ParameterCode id);

  /**
   * @brief Checks if a double parameter is set.
   */
  bool isDoubleSet(ParameterCode id);

  /**
   * @brief Checks if a boolean parameter is set.
   */
  bool isBoolSet(ParameterCode id);

  /**
   * @brief Checks if a double vector parameter is set.
   */
  bool isDoubleVectorSet(ParameterCode id);

  /**
   * @brief Checks if an integer vector parameter is set.
   */
  bool isIntVectorSet(ParameterCode id);

  /**
   * @brief Checks if a string parameter is set.
   */
  bool isStringSet(ParameterCode id) const;

  //////////////////////////////////////////////////////

  // special operations

  /**
   * @brief Gets the name of the parameter associated with the given parameter code.
   */
  static std::string getParameterName(ParameterCode id);

  /**
   * @brief Handles every aspect of parameters that represents files.
   */
  void sanitizePath(ParameterCode id, bool shouldFileExist);

  /**
   * @brief Write the configuration of hidden layers to a file.
   */
  void writeHiddenLayersFile();

  /**
   * @brief Reads, checks, and stores the hidden layers configuration file.
   */
  void readHiddenLayersFile(StringInt &arch, StringInt &archInd);

  /**
   * @brief Converts a vector of any type to a string representation.
   *
   * @tparam T The type of elements in the vector.
   * @param vec The vector to convert.
   * @return std::string The string representation of the vector.
   */
  template <typename T>
  std::string vectorToString(const std::vector<T> &vec) const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      ss << vec[i];
      if (i < vec.size() - 1) {
        ss << ", ";
      }
    }
    ss << "]";
    return ss.str();
  }

  //////////////////////////////////////////////////////

  // assertions

  /**
   * @brief Asserts that a string parameter is set.
   */
  void assertStringExists(ParameterCode id) const;

  /**
   * @brief Asserts that an integer parameter is set.
   */
  void assertIntExists(ParameterCode id);

  /**
   * @brief Asserts that a float parameter is set.
   */
  void assertFloatExists(ParameterCode id);

  /**
   * @brief Asserts that a double parameter is set.
   */
  void assertDoubleExists(ParameterCode id);

  /**
   * @brief Asserts that a boolean parameter is set.
   */
  void assertBoolExists(ParameterCode id);

  /**
   * @brief Asserts that a double vector parameter is set.
   */
  void assertDoubleVectorExists(ParameterCode id);

  /**
   * @brief Asserts that an integer vector parameter is set.
   */
  void assertIntVectorExists(ParameterCode id);

  //////////////////////////////////////////////////////

  // check common parameters

  /**
   * @brief Checks the attribute and class counts for validity.
   */
  void checkAttributeAndClassCounts();

  /**
   * @brief Checks common parameters for validity.
   */
  void checkParametersCommon();

  /**
   * @brief Checks Fidex-specific parameters for validity.
   */
  void checkParametersFidex();

  /**
   * @brief Checks decision threshold and positive class index parameters for validity.
   */
  void checkParametersDecisionThreshold();

  /**
   * @brief Checks Dimlp training specific parameters for validity.
   */
  void checkParametersDimlpTrn();

  /**
   * @brief Checks normalization parameters for validity.
   */
  void checkParametersNormalization();

  //////////////////////////////////////////////////////

  // set common default parameters

  /**
   * @brief Sets the default number of quantization levels.
   */
  void setDefaultNbQuantLevels();

  /**
   * @brief Sets the default values of Fidex parameters.
   */
  void setDefaultFidex();

  /**
   * @brief Sets the default values of decision threshold and positive class index parameters.
   */
  void setDefaultDecisionThreshold();

  /**
   * @brief Sets the default values of Dimlp training parameters.
   */
  void setDefaultDimlpTrn();
};

/**
 * @brief Overloads the stream insertion operator for printing each parameter of Parameters object.
 *
 * @param stream The output stream.
 * @param p The Parameters object to print.
 * @return std::ostream& The output stream with the Parameters information.
 */
inline std::ostream &operator<<(std::ostream &stream, const Parameters &p) {
  int pad = 100;
  stream << "Parameters list:" << std::endl;

  for (auto const &x : p.getAllStrings()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << x.second << std::endl;
  }

  for (auto const &x : p.getAllInts()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << std::to_string(x.second) << std::endl;
  }

  for (auto const &x : p.getAllFloats()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << std::to_string(x.second) << std::endl;
  }

  for (auto const &x : p.getAllDoubles()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << std::to_string(x.second) << std::endl;
  }

  for (auto const &x : p.getAllBools()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << std::to_string(x.second) << std::endl;
  }

  for (auto const &x : p.getAllIntVectors()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << p.vectorToString(x.second) << std::endl;
  }

  for (auto const &x : p.getAllDoubleVectors()) {
    stream << " - " << Parameters::getParameterName(x.first) << std::setw(pad - static_cast<int>(Parameters::getParameterName(x.first).size())) << p.vectorToString(x.second) << std::endl;
  }

  stream << "End of Parameters list." << std::endl
         << std::endl;

  return stream;
}

#endif // PARAMETERS_H
