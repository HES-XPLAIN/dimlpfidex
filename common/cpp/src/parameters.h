#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedant.h"
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

// to add a new parameter, just add a new parameter code BEFORE "_NB_PARAMETERS"
enum ParameterCode {
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

class Parameters {
private:
  std::map<ParameterCode, int> _intParams;
  std::map<ParameterCode, float> _floatParams;
  std::map<ParameterCode, double> _doubleParams;
  std::map<ParameterCode, bool> _boolParams;
  std::map<ParameterCode, std::vector<double>> _doubleVectorParams;
  std::map<ParameterCode, std::vector<int>> _intVectorParams;
  std::map<ParameterCode, std::string> _stringParams;
  std::vector<std::string> _weightFiles;

  // private parser
  void parseArg(const std::string &param, const std::string &arg, const std::vector<ParameterCode> &validParams);

  // path checker
  void checkFilesIntegrity();
  void checkPath(ParameterCode id, const std::string &path) const;
  void completePath(ParameterCode id);

  // throwables
  [[noreturn]] void throwInvalidDataTypeException(ParameterCode id, const std::string &wrongValue, const std::string &typeName) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".");
  }

  [[noreturn]] void throwAlreadySetArgumentException(ParameterCode id, const std::string &value) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + value + "\" is already set, cannot override it.");
  }

  [[noreturn]] void throwArgumentNotFoundException(ParameterCode id) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(id) + ") requested was not found, try to rerun including it.");
  }

  [[noreturn]] void throwInvalidFileOrDirectory(ParameterCode id, const std::string &wrongValue) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid path. The directory or file specified could not be found.");
  }

  [[noreturn]] void throwInvalidParameter(ParameterCode wrongValue) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(wrongValue) + ") is not a valid parameter for this execution.");
  }

public:
  // constructor
  Parameters() = default;
  explicit Parameters(const std::vector<std::string> &args, const std::vector<ParameterCode> &validParams);
  explicit Parameters(const std::string &jsonfile, const std::vector<ParameterCode> &validParams);

  // setters handle formatting from string argument
  void setInt(ParameterCode id, const std::string &value);
  void setInt(ParameterCode id, int value);
  void setFloat(ParameterCode id, const std::string &value);
  void setFloat(ParameterCode id, float value);
  void setDouble(ParameterCode id, const std::string &value);
  void setDouble(ParameterCode id, double value);
  void setBool(ParameterCode id, std::string value);
  void setBool(ParameterCode id, bool value);
  void setDoubleVector(ParameterCode id, const std::string &value);
  void setDoubleVector(ParameterCode id, const std::vector<double> &value);
  void setIntVector(ParameterCode id, const std::string &value);
  void setIntVector(ParameterCode id, const std::vector<int> &value);
  void setString(ParameterCode id, const std::string &value);

  // default setter if value not set
  void setDefaultInt(ParameterCode id, int value);
  void setDefaultFloat(ParameterCode id, float value);
  void setDefaultDouble(ParameterCode id, double value);
  void setDefaultBool(ParameterCode id, bool value);
  void setDefaultDoubleVector(ParameterCode id, const std::string &defaultValue);
  void setDefaultIntVector(ParameterCode id, const std::string &defaultValue);
  void setDefaultString(ParameterCode id, const std::string &defaultValue, bool withRoot = false);

  // getters
  int getInt(ParameterCode id);
  float getFloat(ParameterCode id);
  double getDouble(ParameterCode id);
  bool getBool(ParameterCode id);
  std::vector<double> getDoubleVector(ParameterCode id);
  std::vector<int> getIntVector(ParameterCode id);
  std::string getString(ParameterCode id);
  StringInt getArch();
  StringInt getArchInd();

  std::map<ParameterCode, int> getAllInts() const { return _intParams; }
  std::map<ParameterCode, float> getAllFloats() const { return _floatParams; }
  std::map<ParameterCode, double> getAllDoubles() const { return _doubleParams; }
  std::map<ParameterCode, bool> getAllBools() const { return _boolParams; }
  std::map<ParameterCode, std::vector<double>> getAllDoubleVectors() const { return _doubleVectorParams; }
  std::map<ParameterCode, std::vector<int>> getAllIntVectors() const { return _intVectorParams; }
  std::map<ParameterCode, std::string> getAllStrings() const { return _stringParams; }

  bool isIntSet(ParameterCode id);
  bool isFloatSet(ParameterCode id);
  bool isDoubleSet(ParameterCode id);
  bool isBoolSet(ParameterCode id);
  bool isDoubleVectorSet(ParameterCode id);
  bool isIntVectorSet(ParameterCode id);
  bool isStringSet(ParameterCode id) const;

  // special operations
  static std::string getParameterName(ParameterCode id);
  void sanitizePath(ParameterCode id, bool shouldFileExist);
  void writeHiddenLayersFile();
  void readHiddenLayersFile(StringInt &arch, StringInt &archInd);

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

  // assertions
  void assertIntExists(ParameterCode id);
  void assertFloatExists(ParameterCode id);
  void assertDoubleExists(ParameterCode id);
  void assertBoolExists(ParameterCode id);
  void assertDoubleVectorExists(ParameterCode id);
  void assertIntVectorExists(ParameterCode id);
  void assertStringExists(ParameterCode id) const;

  // check common parameters
  void checkAttributeAndClassCounts();
  void checkParametersCommon();
  void checkParametersFidex();
  void checkParametersDecisionThreshold();
  void checkParametersDimlpTrn();
  void checkParametersNormalization();

  // set common default parameters
  void setDefaultNbQuantLevels();
  void setDefaultFidex();
  void setDefaultDecisionThreshold();
  void setDefaultDimlpTrn();
};

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
