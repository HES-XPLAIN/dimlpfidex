#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "checkFun.h"
#include "errorHandler.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <omp.h>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// to add a new parameter, just add a new parameter code BEFORE "_NB_PARAMETERS"
enum ParameterCode {
  TRAIN_DATA_FILE,
  TRAIN_DATA_PRED_FILE,
  TRAIN_DATA_TRUE_CLASS_FILE,
  INPUT_RULES_FILE,
  RULES_FILE,
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
  NB_THREADS_USED,
  INDEX_POSITIVE_CLASS,
  SEED,
  HEURISITC_INIT,
  DECISION_THRESHOLD,
  HI_KNOT,
  DROPOUT_HYP,
  DROPOUT_DIM,
  MIN_FIDELITY,
  _NB_PARAMETERS // internal use only, do not consider it as a usable parameter
};

static const vector<string> parameterNames = {
    "TRAIN_DATA_FILE",
    "TRAIN_DATA_PRED_FILE",
    "TRAIN_DATA_TRUE_CLASS_FILE",
    "INPUT_RULES_FILE",
    "RULES_FILE",
    "CONSOLE_FILE",
    "ROOT_FOLDER",
    "ATTRIBUTES_FILE",
    "WEIGHTS_FILE",
    "NB_ATTRIBUTES",
    "NB_CLASSES",
    "NB_DIMLP_NETS",
    "NB_QUANT_LEVELS",
    "HEURISTIC",
    "MAX_ITERATIONS",
    "MIN_COVERING",
    "MAX_FAILED_ATTEMPTS",
    "NB_THREADS_USED",
    "INDEX_POSITIVE_CLASS",
    "SEED",
    "HEURISITC_INIT",
    "DECISION_THRESHOLD",
    "HI_KNOT",
    "DROPOUT_HYP",
    "DROPOUT_DIM",
    "MIN_FIDELITY"};

class Parameters {
private:
  map<ParameterCode, int> _intParams;
  map<ParameterCode, float> _floatParams;
  map<ParameterCode, double> _doubleParams;
  map<ParameterCode, string> _stringParams;
  vector<string> _weightFiles; // the only 1 special parameter

  // setters handle formatting from string argument
  void setInt(ParameterCode id, string value);
  void setInt(ParameterCode id, int value);
  void setFloat(ParameterCode id, string value);
  void setFloat(ParameterCode id, float value);
  void setDouble(ParameterCode id, string value);
  void setDouble(ParameterCode id, double value);
  void setString(ParameterCode id, string value);
  void setRootDirectory(ParameterCode id);

  void throwInvalidDataTypeException(ParameterCode id, string wrongValue, string typeName) {
    throw CommandArgumentException("Parsing error: argument (ID " + parameterNames[id] + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".");
  }

  void throwAlreadySetArgumentException(ParameterCode id, string value) {
    throw CommandArgumentException("Parsing error: argument (ID " + parameterNames[id] + ") with value \"" + value + "\" is already set, cannot override it.");
  }

  void throwArgumentNotFoundException(ParameterCode id) {
    throw CommandArgumentException("Parameters error: argument (ID " + parameterNames[id] + ") requested was not found, try to rerun including it.");
  }

public:
  // constructor
  Parameters() = default;
  Parameters(vector<string> args);
  // TODO Parameters(string jsonfile);

  // default setter if value not set
  void setDefaultInt(ParameterCode id, int value);
  void setDefaultFloat(ParameterCode id, float value);
  void setDefaultDouble(ParameterCode id, double value);
  void setDefaultString(ParameterCode id, string value);

  // getters
  int getInt(ParameterCode id);
  float getFloat(ParameterCode id);
  double getDouble(ParameterCode id);
  string getString(ParameterCode id);
  vector<string> getWeightsFiles() const;

  map<ParameterCode, int> getAllInts() const { return _intParams; }
  map<ParameterCode, float> getAllFloats() const { return _floatParams; }
  map<ParameterCode, double> getAllDoubles() const { return _doubleParams; }
  map<ParameterCode, string> getAllStrings() const { return _stringParams; }

  bool isIntSet(ParameterCode id);
  bool isFloatSet(ParameterCode id);
  bool isDoubleSet(ParameterCode id);
  bool isStringSet(ParameterCode id);

  // special operations
  void setWeightsFiles();
  void addWeightsFile(string file);

  // assertions
  void assertIntExists(ParameterCode id);
  void assertFloatExists(ParameterCode id);
  void assertDoubleExists(ParameterCode id);
  void assertStringExists(ParameterCode id);
};

inline ostream &operator<<(ostream &stream, const Parameters &p) {
  int pad = 100;
  stream << "Parameters list:" << endl;

  for (auto const &x : p.getAllStrings()) {
    stream << " - " << parameterNames[x.first] << setw(pad - parameterNames[x.first].size()) << x.second << endl;
  }

  for (auto const &x : p.getAllInts()) {
    stream << " - " << parameterNames[x.first] << setw(pad - parameterNames[x.first].size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllFloats()) {
    stream << " - " << parameterNames[x.first] << setw(pad - parameterNames[x.first].size()) << to_string(x.second) << endl;
  }

  for (auto const &x : p.getAllDoubles()) {
    stream << " - " << parameterNames[x.first] << setw(pad - parameterNames[x.first].size()) << to_string(x.second) << endl;
  }

  stream << "  WEIGHTS_FILES (list)" << endl;
  for (string f : p.getWeightsFiles()) {
    stream << "     " << f << endl;
  }

  stream << "End of Parameters list." << endl
         << endl;

  return stream;
}

#endif // PARAMETERS_H
