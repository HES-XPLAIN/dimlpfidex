#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "checkFun.h"
#include "errorHandler.h"
#include <iostream>
#include <map>
#include <omp.h>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// to add a new parameter, just add a new parameter code
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
  MIN_FIDELITY
};

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
  void setWeightFiles(vector<string> files);
  void setRootDirectory(ParameterCode id);

  void throwInvalidDataTypeException(ParameterCode id, string wrongValue, string typeName) {
    throw CommandArgumentException("Parsing error: argument (ID " + to_string(id) + ") with value \"" + wrongValue + "\" is not a valid" + typeName + ".");
  }

  void throwAlreadySetArgumentException(ParameterCode id, string value) {
    throw CommandArgumentException("Parsing error: argument (ID " + to_string(id) + ") with value \"" + value + "\" is already set, cannot override it.");
  }

  void throwArgumentNotFoundException(ParameterCode id) {
    throw CommandArgumentException("Parameters error: argument (ID " + to_string(id) + ") requested was not found, try to rerun including it.");
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

  bool isIntSet(ParameterCode id);
  bool isFloatSet(ParameterCode id);
  bool isDoubleSet(ParameterCode id);
  bool isStringSet(ParameterCode id);

  // special operations
  void addWeightsFile(string file);
};

#endif // PARAMETERS_H
