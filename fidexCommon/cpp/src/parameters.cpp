#include "parameters.h"

/**
 * @brief Construct a new Parameters object containing all arguments passed by CLI.
 *
 * @param args program arguments
 */
Parameters::Parameters(vector<string> args) {
  for (int p = 1; p < args.size(); p++) {
    if (args[p][0] == '-') {
      p++;

      if (p >= args.size()) {
        throw CommandArgumentException("Missing something at the end of the command.");
      }

      char option = args[p - 1][1];
      const char *arg = &(args[p])[0];
      const char *lastArg = &(args[p - 1])[0];

      switch (option) { // Get letter after the -

      case 'T':
        setString(TRAIN_DATA_FILE, arg); // Parameter after -T
        break;

      case 'P':
        setString(TRAIN_DATA_PRED_FILE, arg);
        break;

      case 'C':
        setString(TRAIN_DATA_TRUE_CLASS_FILE, arg);
        break;

      case 'W':
        setString(WEIGHTS_FILE, arg);
        break;

      case 'f':
        setString(RULES_FILE, arg);
        break;

      case 'N':
        setInt(NB_DIMLP_NETS, arg);
        break;

      case 'Q':
        setInt(NB_QUANT_LEVELS, arg);
        break;

      case 'O':
        setString(RULES_FILE, arg);
        break;

      case 'r':
        setString(CONSOLE_FILE, arg);
        break;

      case 'A':
        setString(ATTRIBUTES_FILE, arg);
        break;

      case 'S':
        setString(ROOT_FOLDER, arg);
        break;

      case 'M':
        setInt(HEURISTIC, arg);
        break;

      case 'i':
        setInt(MAX_ITERATIONS, arg);
        break;

      case 'v':
        setInt(MIN_COVERING, arg);
        break;

      case 'p':
        setInt(NB_THREADS_USED, arg);
        break;

      case 'y':
        setFloat(MIN_FIDELITY, arg);
        break;

      case 'd':
        setFloat(DROPOUT_DIM, arg);
        break;

      case 'h':
        setFloat(DROPOUT_HYP, arg);
        break;

      case 'm':
        setInt(MAX_FAILED_ATTEMPTS, arg);
        break;

      case 't':
        setFloat(DECISION_THRESHOLD, arg);
        break;

      case 'x':
        setInt(INDEX_POSITIVE_CLASS, arg);
        break;

      case 'z':
        setInt(SEED, arg);
        break;

      default: // If we put another -X option
        throw CommandArgumentException("Illegal option : " + string(lastArg));
      }
    }
  }
}

// TODO: Implement Parameters::Parameters(string jsonfile) {}

// private setters
void Parameters::setInt(ParameterCode id, string value) {
  if (isIntSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (atoi(value.c_str())) {
    _intParams[id] = atoi(value.c_str());
  } else {
    throwInvalidDataTypeException(id, value, "integer");
  }
}

void Parameters::setFloat(ParameterCode id, string value) {
  if (isFloatSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (atof(value.c_str())) {
    _floatParams[id] = atof(value.c_str());
  } else {
    throwInvalidDataTypeException(id, value, "float");
  }
}

// TODO: find better double translation than atof
void Parameters::setDouble(ParameterCode id, string value) {
  if (isDoubleSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (atof(value.c_str())) {
    _doubleParams[id] = atof(value.c_str());
  } else {
    throwInvalidDataTypeException(id, value, "double");
  }
}

void Parameters::setString(ParameterCode id, string value) {
  if (isStringSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }
  _stringParams[id] = value;
}

// TODO: should I check for empty vector ?
void Parameters::setWeightFiles(vector<string> files) {
  _weightFiles = files;
}

// public getters
int Parameters::getInt(ParameterCode id) {
  if (_intParams.find(id) == _intParams.end()) {
    throwArgumentNotFoundException(id);
  }

  return _intParams[id];
}

float Parameters::getFloat(ParameterCode id) {
  if (_floatParams.find(id) == _floatParams.end()) {
    throwArgumentNotFoundException(id);
  }

  return _floatParams[id];
}

double Parameters::getDouble(ParameterCode id) {
  if (_doubleParams.find(id) == _doubleParams.end()) {
    throwArgumentNotFoundException(id);
  }

  return _doubleParams[id];
}

string Parameters::getString(ParameterCode id) {
  if (_stringParams.find(id) == _stringParams.end()) {
    throwArgumentNotFoundException(id);
  }

  return _stringParams[id];
}

bool Parameters::isIntSet(ParameterCode id) {
  return _intParams.find(id) != _intParams.end();
}

bool Parameters::isFloatSet(ParameterCode id) {
  return _floatParams.find(id) != _floatParams.end();
}

bool Parameters::isDoubleSet(ParameterCode id) {
  return _doubleParams.find(id) != _doubleParams.end();
}

bool Parameters::isStringSet(ParameterCode id) {
  return _stringParams.find(id) != _stringParams.end();
}

// TODO check if empty ?
vector<string> Parameters::getWeightsFiles() const {
  return _weightFiles;
}

// public special operations
// TODO check for duplicates ?
void Parameters::addWeightsFile(string file) {
  _weightFiles.push_back(file);
}
