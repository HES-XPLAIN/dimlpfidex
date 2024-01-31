#include "parameters.h"

/**
 * @brief Construct a new Parameters object containing all arguments passed by CLI.
 *
 *
 * @param args program arguments
 */
Parameters::Parameters(const vector<string> &args) {
  for (int p = 1; p < args.size(); p++) {
    string param = args[p];

    if (param.substr(0, 2) == "--") {
      param = param.substr(2);
      p++;

      if (p >= args.size()) {
        throw CommandArgumentException("Missing something at the end of the command.");
      }
      const string arg = args[p];

      if (p + 1 < args.size() && args[p + 1].substr(0, 2) != "--") {
        throw CommandArgumentException("There is a parameter without -- (" + args[p + 1] + ").");
      }

      parseArg(param, arg);
    }
  }

  // updating paths of files
  if (isStringSet(ROOT_FOLDER)) {
    setRootDirectory(TRAIN_DATA_FILE);
    setRootDirectory(TRAIN_PRED_FILE);
    setRootDirectory(TRAIN_CLASS_FILE);
    setRootDirectory(TEST_DATA_FILE);
    setRootDirectory(TEST_PRED_FILE);
    setRootDirectory(TEST_CLASS_FILE);
    setRootDirectory(RULES_FILE);
    setRootDirectory(GLOBAL_RULES_OUTFILE);
    setRootDirectory(GLOBAL_RULES_FILE);
    setRootDirectory(EXPLANATION_FILE);
    setRootDirectory(CONSOLE_FILE);
    setRootDirectory(ATTRIBUTES_FILE);
    setRootDirectory(WEIGHTS_FILE);
    setRootDirectory(NORMALIZATION_FILE);
  }
}

/**
 * @brief Construct a new Parameters object containing all arguments passed by JSON config file.
 *
 * @param jsonfile JSON config file name
 */
Parameters::Parameters(const string &jsonfile) {
  ifstream ifs;
  ifs.open(jsonfile);
  vector<string> args;

  if (!ifs.is_open() || ifs.fail()) {
    throw FileNotFoundError("JSON file to parse named '" + jsonfile + "' was not found, cannot proceed.");
  }

  Json jsonData = Json::parse(ifs);

  for (auto &item : jsonData.items()) {
    string value;

    if (item.value().is_string()) {
      value = static_cast<string>(item.value());
    } else {
      value = to_string(item.value());
    }

    parseArg(item.key(), value.c_str());
  }

  // updating paths of files
  if (isStringSet(ROOT_FOLDER)) {
    setRootDirectory(TRAIN_DATA_FILE);
    setRootDirectory(TRAIN_PRED_FILE);
    setRootDirectory(TRAIN_CLASS_FILE);
    setRootDirectory(RULES_FILE);
    setRootDirectory(GLOBAL_RULES_OUTFILE);
    setRootDirectory(CONSOLE_FILE);
    setRootDirectory(ATTRIBUTES_FILE);
    setRootDirectory(WEIGHTS_FILE);
    setRootDirectory(NORMALIZATION_FILE);
  }
}

/**
 * @brief parse a given parameter name & its value in order to add it to the Parameter class
 *
 * To add a new parameter you must follow this workflow:
 *    - Add a new element in the ParameterCode enum in Paramerters.h
 *    - Add a new element in the unordered_map with the parameter litteral name and its enum in Paramerters.h
 *    - Adapt the code below to accept your new argument in the switch case
 *
 * @param param parameter name (ex: nb_threads, min_fidelity etc...)
 * @param arg parameter's associated value
 */
void Parameters::parseArg(const string &param, const string &arg) {
  ParameterCode option;
  auto it = parameterNames.find(param);
  if (it != parameterNames.end()) {
    option = it->second;
  } else {
    option = INVALID;
  }

  switch (option) {

  case TRAIN_DATA_FILE:
    setString(TRAIN_DATA_FILE, arg); // Parameter after -T
    break;

  case TRAIN_PRED_FILE:
    setString(TRAIN_PRED_FILE, arg);
    break;

  case TEST_DATA_FILE:
    setString(TEST_DATA_FILE, arg); // Parameter after -T
    break;

  case TEST_PRED_FILE:
    setString(TEST_PRED_FILE, arg);
    break;

  case NB_ATTRIBUTES:
    setInt(NB_ATTRIBUTES, arg);
    break;

  case NB_CLASSES:
    setInt(NB_CLASSES, arg);
    break;

  case TRAIN_CLASS_FILE:
    setString(TRAIN_CLASS_FILE, arg);
    break;

  case TEST_CLASS_FILE:
    setString(TEST_CLASS_FILE, arg);
    break;

  case WEIGHTS_FILE:
    setString(WEIGHTS_FILE, arg);
    break;

  case RULES_FILE:
    setString(RULES_FILE, arg);
    break;

  case NB_DIMLP_NETS:
    setInt(NB_DIMLP_NETS, arg);
    break;

  case NB_QUANT_LEVELS:
    setInt(NB_QUANT_LEVELS, arg);
    break;

  case GLOBAL_RULES_OUTFILE:
    setString(GLOBAL_RULES_OUTFILE, arg);
    break;

  case GLOBAL_RULES_FILE:
    setString(GLOBAL_RULES_FILE, arg);
    break;

  case EXPLANATION_FILE:
    setString(EXPLANATION_FILE, arg);
    break;

  case CONSOLE_FILE:
    setString(CONSOLE_FILE, arg);
    break;

  case ATTRIBUTES_FILE:
    setString(ATTRIBUTES_FILE, arg);
    break;

  case ROOT_FOLDER:
    setString(ROOT_FOLDER, arg);
    break;

  case HEURISTIC:
    setInt(HEURISTIC, arg);
    break;

  case MAX_ITERATIONS:
    setInt(MAX_ITERATIONS, arg);
    break;

  case MIN_COVERING:
    setInt(MIN_COVERING, arg);
    break;

  case NB_THREADS:
    setInt(NB_THREADS, arg);
    break;

  case MIN_FIDELITY:
    setFloat(MIN_FIDELITY, arg);
    break;

  case DROPOUT_DIM:
    setFloat(DROPOUT_DIM, arg);
    break;

  case DROPOUT_HYP:
    setFloat(DROPOUT_HYP, arg);
    break;

  case MAX_FAILED_ATTEMPTS:
    setInt(MAX_FAILED_ATTEMPTS, arg);
    break;

  case DECISION_THRESHOLD:
    setFloat(DECISION_THRESHOLD, arg);
    break;

  case POSITIVE_CLASS_INDEX:
    setInt(POSITIVE_CLASS_INDEX, arg);
    break;

  case NORMALIZATION_FILE:
    setString(NORMALIZATION_FILE, arg);
    break;

  case MUS:
    setDoubleVector(MUS, arg);
    break;

  case SIGMAS:
    setDoubleVector(SIGMAS, arg);
    break;

  case NORMALIZATION_INDICES:
    setIntVector(NORMALIZATION_INDICES, arg);
    break;

  case WITH_FIDEX:
    setBool(WITH_FIDEX, arg);
    break;

  case WITH_MINIMAL_VERSION:
    setBool(WITH_MINIMAL_VERSION, arg);
    break;

  case SEED:
    setInt(SEED, arg);
    break;

  default: // If we put another -X option
    throw CommandArgumentException("Illegal option : " + param);
  }
}

// private setters
void Parameters::setInt(ParameterCode id, const string &value) {
  if (isIntSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  try {
    _intParams[id] = stoi(value, nullptr);
  } catch (const std::exception &) { // out_of_range & invalid_argument are thrown
    throwInvalidDataTypeException(id, value, "integer");
  }
}

void Parameters::setInt(ParameterCode id, int value) {
  if (isIntSet(id)) {
    throwAlreadySetArgumentException(id, to_string(value));
  }

  _intParams[id] = value;
}

void Parameters::setFloat(ParameterCode id, const string &value) {
  if (isFloatSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  try {
    _floatParams[id] = stof(value, nullptr);
  } catch (const std::exception &) { // out_of_range & invalid_argument are thrown
    throwInvalidDataTypeException(id, value, "float");
  }
}

void Parameters::setFloat(ParameterCode id, float value) {
  if (isFloatSet(id)) {
    throwAlreadySetArgumentException(id, to_string(value));
  }

  _floatParams[id] = value;
}

void Parameters::setDouble(ParameterCode id, const string &value) {
  if (isDoubleSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  try {
    _doubleParams[id] = stod(value, nullptr);
  } catch (const std::exception &) { // out_of_range & invalid_argument are thrown
    throwInvalidDataTypeException(id, value, "double");
  }
}

void Parameters::setDouble(ParameterCode id, double value) {
  if (isDoubleSet(id)) {
    throwAlreadySetArgumentException(id, to_string(value));
  }

  _doubleParams[id] = value;
}

void Parameters::setBool(ParameterCode id, string value) {
  if (isBoolSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (checkBool(value)) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    _boolParams[id] = (value == "true" || value == "1") ? true : false;
  } else {
    throw CommandArgumentException("Error : invalide type for parameter " + getParameterName(id) + ", boolean requested.");
  }
}

void Parameters::setBool(ParameterCode id, bool value) {
  _boolParams[id] = value;
}
void Parameters::setDoubleVector(ParameterCode id, const string &value) {
  if (isDoubleVectorSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (!checkList(value)) {
    throw CommandArgumentException("Error : invalide type for parameter " + getParameterName(id) + ", list in the form [a,b,...,c] without spaces requested, a,b,c are numbers. Received " + value + ".");
  }
  _doubleVectorParams[id] = getDoubleVectorFromString(value);
}

void Parameters::setDoubleVector(ParameterCode id, const vector<double> &value) {
  _doubleVectorParams[id] = value;
}

void Parameters::setIntVector(ParameterCode id, const string &value) {
  if (isIntVectorSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (!checkList(value)) {
    throw CommandArgumentException("Error : invalide type for parameter " + getParameterName(id) + ", list in the form [a,b,...,c] without spaces requested, a,b,c are integers. Received " + value + ".");
  }
  _intVectorParams[id] = getIntVectorFromString(value);
}

void Parameters::setIntVector(ParameterCode id, const vector<int> &value) {
  _intVectorParams[id] = value;
}

void Parameters::setString(ParameterCode id, const string &value) {
  if (isStringSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }
  _stringParams[id] = value;
}

void Parameters::setRootDirectory(ParameterCode id) {
  // ignore if root or target were not set
  if (!isStringSet(ROOT_FOLDER) || !isStringSet(id))
    return;

  // avoid duplicating the root path for no reason
  if (id == ROOT_FOLDER)
    return;

  string root = getString(ROOT_FOLDER);
  string target = getString(id);
  string separator;

  // define separator depending on OS
#if defined(__unix__) || defined(__APPLE__)
  separator = "/";
#elif defined(_WIN32)
  separator = "\\";
#endif

  // to avoid AlreadySetException errors thrown by setters, the map is directly midified
  _stringParams[id] = root + separator + target;
}

void Parameters::setWeightsFiles() {
  if (!isIntSet(NB_DIMLP_NETS)) {
    cout << "Error: cannot create weights files list." << endl;
    throwArgumentNotFoundException(NB_DIMLP_NETS);
  }

  if (!isStringSet(WEIGHTS_FILE)) {
    cout << "Error: cannot create weights files list." << endl;
    throwArgumentNotFoundException(WEIGHTS_FILE);
  }

  int dimlpNets = getInt(NB_DIMLP_NETS);
  string weigthsFile = getString(WEIGHTS_FILE);

  if (dimlpNets == 1) {
    addWeightsFile(weigthsFile);
  } else {
    for (int i = 1; i <= dimlpNets; i += 1) {
      addWeightsFile(weigthsFile + to_string(i) + ".wts");
    }
  }
}

// public getters
int Parameters::getInt(ParameterCode id) {
  assertIntExists(id);
  return _intParams[id];
}

float Parameters::getFloat(ParameterCode id) {
  assertFloatExists(id);
  return _floatParams[id];
}

double Parameters::getDouble(ParameterCode id) {
  assertDoubleExists(id);
  return _doubleParams[id];
}

bool Parameters::getBool(ParameterCode id) {
  assertBoolExists(id);
  return _boolParams[id];
}

vector<double> Parameters::getDoubleVector(ParameterCode id) {
  assertDoubleVectorExists(id);
  return _doubleVectorParams[id];
}

vector<int> Parameters::getIntVector(ParameterCode id) {
  assertIntVectorExists(id);
  return _intVectorParams[id];
}

string Parameters::getString(ParameterCode id) {
  assertStringExists(id);
  return _stringParams[id];
}

std::string Parameters::getParameterName(ParameterCode id) const {
  for (const auto &pair : parameterNames) {
    if (pair.second == id) {
      return pair.first;
    }
  }
  return "unknown";
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

bool Parameters::isBoolSet(ParameterCode id) {
  return _boolParams.find(id) != _boolParams.end();
}

bool Parameters::isDoubleVectorSet(ParameterCode id) {
  return _doubleVectorParams.find(id) != _doubleVectorParams.end();
}

bool Parameters::isIntVectorSet(ParameterCode id) {
  return _intVectorParams.find(id) != _intVectorParams.end();
}

bool Parameters::isStringSet(ParameterCode id) const {
  return _stringParams.find(id) != _stringParams.end();
}

vector<string> Parameters::getWeightsFiles() const {
  return _weightFiles;
}

// public assertions
void Parameters::assertStringExists(ParameterCode id) const {
  if (!isStringSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertIntExists(ParameterCode id) {
  if (!isIntSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertFloatExists(ParameterCode id) {
  if (!isFloatSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertDoubleExists(ParameterCode id) {
  if (!isDoubleSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertBoolExists(ParameterCode id) {
  if (!isBoolSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertDoubleVectorExists(ParameterCode id) {
  if (!isDoubleVectorSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

void Parameters::assertIntVectorExists(ParameterCode id) {
  if (!isIntVectorSet(id)) {
    throwArgumentNotFoundException(id);
  }
}

// public special operations
void Parameters::addWeightsFile(const string &file) {
  _weightFiles.push_back(file);
}

// Default setters
void Parameters::setDefaultInt(ParameterCode id, int defaultValue) {
  if (!isIntSet(id))
    setInt(id, defaultValue);
}

void Parameters::setDefaultFloat(ParameterCode id, float defaultValue) {
  if (!isFloatSet(id))
    setFloat(id, defaultValue);
}

void Parameters::setDefaultDouble(ParameterCode id, double defaultValue) {
  if (!isDoubleSet(id))
    setDouble(id, defaultValue);
}

void Parameters::setDefaultBool(ParameterCode id, bool defaultValue) {
  if (!isBoolSet(id))
    setBool(id, defaultValue);
}

void Parameters::setDefaultDoubleVector(ParameterCode id, const string &defaultValue) {
  if (!isDoubleVectorSet(id))
    setDoubleVector(id, defaultValue);
}

void Parameters::setDefaultIntVector(ParameterCode id, const string &defaultValue) {
  if (!isIntVectorSet(id))
    setIntVector(id, defaultValue);
}

void Parameters::setDefaultString(ParameterCode id, const string &defaultValue) {
  if (!isStringSet(id))
    setString(id, defaultValue);
}
