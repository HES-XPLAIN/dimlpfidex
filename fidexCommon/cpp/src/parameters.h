#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "../../../json/single_include/nlohmann/json.hpp"
#include "antecedant.h"
#include "checkFun.h"
#include "errorHandler.h"
#include "rule.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <omp.h>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// to add a new parameter, just add a new parameter code BEFORE "_NB_PARAMETERS"
enum ParameterCode {
  TRAIN_DATA_FILE,
  TRAIN_PRED_FILE,
  TRAIN_CLASS_FILE,
  TEST_DATA_FILE,
  TEST_PRED_FILE,
  TEST_CLASS_FILE,
  RULES_FILE,
  GLOBAL_RULES_OUTFILE,
  GLOBAL_RULES_FILE,
  EXPLANATION_FILE,
  CONSOLE_FILE,
  ROOT_FOLDER,
  ATTRIBUTES_FILE,
  WEIGHTS_FILE,
  NB_ATTRIBUTES,
  NB_CLASSES,
  NB_DIMLP_NETS,
  NB_QUANT_LEVELS,
  HEURISTIC,
  MAX_ITERATIONS,
  MIN_COVERING,
  MAX_FAILED_ATTEMPTS,
  NB_THREADS,
  POSITIVE_CLASS_INDEX,
  SEED,
  DECISION_THRESHOLD,
  HI_KNOT,
  DROPOUT_HYP,
  DROPOUT_DIM,
  MIN_FIDELITY,
  NORMALIZATION_FILE,
  MUS,
  SIGMAS,
  NORMALIZATION_INDICES,
  WITH_FIDEX,
  WITH_MINIMAL_VERSION,
  INVALID,
  _NB_PARAMETERS // internal use only, do not consider it as a usable parameter
};

static const std::unordered_map<std::string, ParameterCode> parameterNames = {
    {"train_data_file", TRAIN_DATA_FILE},
    {"train_pred_file", TRAIN_PRED_FILE},
    {"train_class_file", TRAIN_CLASS_FILE},
    {"test_data_file", TEST_DATA_FILE},
    {"test_pred_file", TEST_PRED_FILE},
    {"test_class_file", TEST_CLASS_FILE},
    {"rules_file", RULES_FILE},
    {"global_rules_outfile", GLOBAL_RULES_OUTFILE},
    {"global_rules_file", GLOBAL_RULES_FILE},
    {"explanation_file", EXPLANATION_FILE},
    {"console_file", CONSOLE_FILE},
    {"root_folder", ROOT_FOLDER},
    {"attributes_file", ATTRIBUTES_FILE},
    {"weights_file", WEIGHTS_FILE},
    {"nb_attributes", NB_ATTRIBUTES},
    {"nb_classes", NB_CLASSES},
    {"nb_dimlp_nets", NB_DIMLP_NETS},
    {"nb_quant_levels", NB_QUANT_LEVELS},
    {"heuristic", HEURISTIC},
    {"max_iterations", MAX_ITERATIONS},
    {"min_covering", MIN_COVERING},
    {"max_failed_attempts", MAX_FAILED_ATTEMPTS},
    {"nb_threads", NB_THREADS},
    {"positive_class_index", POSITIVE_CLASS_INDEX},
    {"seed", SEED},
    {"decision_threshold", DECISION_THRESHOLD},
    {"hi_knot", HI_KNOT},
    {"dropout_hyp", DROPOUT_HYP},
    {"dropout_dim", DROPOUT_DIM},
    {"min_fidelity", MIN_FIDELITY},
    {"normalization_file", NORMALIZATION_FILE},
    {"mus", MUS},
    {"sigmas", SIGMAS},
    {"normalization_indices", NORMALIZATION_INDICES},
    {"normalization_indices", WITH_FIDEX},
    {"normalization_indices", WITH_MINIMAL_VERSION}};

class Parameters {
private:
  map<ParameterCode, int> _intParams;
  map<ParameterCode, float> _floatParams;
  map<ParameterCode, double> _doubleParams;
  map<ParameterCode, bool> _boolParams;
  map<ParameterCode, vector<double>> _doubleVectorParams;
  map<ParameterCode, vector<int>> _intVectorParams;
  map<ParameterCode, string> _stringParams;
  vector<string> _weightFiles; // the only 1 special parameter

  // private parser
  void parseArg(const string &param, const string &arg);

  // setters handle formatting from string argument
  void setInt(ParameterCode id, const string &value);
  void setInt(ParameterCode id, int value);
  void setFloat(ParameterCode id, const string &value);
  void setFloat(ParameterCode id, float value);
  void setDouble(ParameterCode id, const string &value);
  void setDouble(ParameterCode id, double value);
  void setBool(ParameterCode id, string value);
  void setBool(ParameterCode id, bool value);
  void setDoubleVector(ParameterCode id, const string &value);
  void setIntVector(ParameterCode id, const string &value);
  void setString(ParameterCode id, const string &value);
  void setRootDirectory(ParameterCode id);

  void throwInvalidDataTypeException(ParameterCode id, const string &wrongValue, const string &typeName) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".");
  }

  void throwAlreadySetArgumentException(ParameterCode id, const string &value) const {
    throw CommandArgumentException("Parsing error: argument (ID " + getParameterName(id) + ") with value \"" + value + "\" is already set, cannot override it.");
  }

  void throwArgumentNotFoundException(ParameterCode id) const {
    throw CommandArgumentException("Parameters error: argument (ID " + getParameterName(id) + ") requested was not found, try to rerun including it.");
  }

public:
  // constructor
  Parameters() = default;
  Parameters(const vector<string> &args);
  Parameters(const string &jsonfile);

  // default setter if value not set
  void setDefaultInt(ParameterCode id, int value);
  void setDefaultFloat(ParameterCode id, float value);
  void setDefaultDouble(ParameterCode id, double value);
  void setDefaultBool(ParameterCode id, bool value);
  void setDefaultDoubleVector(ParameterCode id, const string &defaultValue);
  void setDefaultIntVector(ParameterCode id, const string &defaultValue);
  void setDefaultString(ParameterCode id, const string &defaultValue);

  // public setter
  void setIntVector(ParameterCode id, vector<int> value);
  void setDoubleVector(ParameterCode id, vector<double> value);

  // getters
  int getInt(ParameterCode id);
  float getFloat(ParameterCode id);
  double getDouble(ParameterCode id);
  bool getBool(ParameterCode id);
  vector<double> getDoubleVector(ParameterCode id);
  vector<int> getIntVector(ParameterCode id);
  string getString(ParameterCode id);
  vector<string> getWeightsFiles() const;

  std::string getParameterName(ParameterCode id) const;

  map<ParameterCode, int> getAllInts() const { return _intParams; }
  map<ParameterCode, float> getAllFloats() const { return _floatParams; }
  map<ParameterCode, double> getAllDoubles() const { return _doubleParams; }
  map<ParameterCode, bool> getAllBools() const { return _boolParams; }
  map<ParameterCode, vector<double>> getAllDoubleVectors() const { return _doubleVectorParams; }
  map<ParameterCode, vector<int>> getAllIntVectors() const { return _intVectorParams; }
  map<ParameterCode, string> getAllStrings() const { return _stringParams; }

  bool isIntSet(ParameterCode id);
  bool isFloatSet(ParameterCode id);
  bool isDoubleSet(ParameterCode id);
  bool isBoolSet(ParameterCode id);
  bool isDoubleVectorSet(ParameterCode id);
  bool isIntVectorSet(ParameterCode id);
  bool isStringSet(ParameterCode id) const;

  // special operations
  void setWeightsFiles();
  void addWeightsFile(string file);
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
  void assertStringExists(ParameterCode id);
};

inline ostream &operator<<(ostream &stream, const Parameters &p) {
  int pad = 100;
  stream << "Parameters list:" << endl;

  for (auto const &x : p.getAllStrings()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << x.second << endl;
  }

  for (auto const &x : p.getAllInts()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllFloats()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllDoubles()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllBools()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllIntVectors()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << p.vectorToString(x.second) << endl;
  }

  for (auto const &x : p.getAllDoubleVectors()) {
    stream << " - " << p.getParameterName(x.first) << setw(pad - p.getParameterName(x.first).size()) << p.vectorToString(x.second) << endl;
  }

  if (p.isStringSet(WEIGHTS_FILE)) {
    stream << "  WEIGHTS_FILES (list)" << endl;
    for (string f : p.getWeightsFiles()) {
      stream << "     " << f << endl;
    }
  }

  stream << "End of Parameters list." << endl
         << endl;

  return stream;
}

#endif // PARAMETERS_H
