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

  checkFilesIntegrity();
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

    parseArg(item.key(), value);
  }

  checkFilesIntegrity();
}

/**
 * @brief Ensures that every file/path is valid, every file related data should be added in this function
 *
 */
void Parameters::checkFilesIntegrity() {
  // input files checks
  sanitizePath(TRAIN_DATA_FILE, true);
  sanitizePath(TRAIN_PRED_FILE, true);
  sanitizePath(TRAIN_CLASS_FILE, true);
  sanitizePath(TEST_DATA_FILE, true);
  sanitizePath(VALID_DATA_FILE, true);
  sanitizePath(VALID_CLASS_FILE, true);
  sanitizePath(TEST_PRED_FILE, true);
  sanitizePath(TEST_CLASS_FILE, true);
  sanitizePath(RULES_FILE, true);
  sanitizePath(GLOBAL_RULES_FILE, true);
  sanitizePath(ATTRIBUTES_FILE, true);
  sanitizePath(WEIGHTS_FILE, true);
  sanitizePath(NORMALIZATION_FILE, true);

  // handling output files checks (files that will be generated by the program)
  sanitizePath(TRAIN_PRED_OUTFILE, false);
  sanitizePath(TEST_PRED_OUTFILE, false);
  sanitizePath(VALID_PRED_OUTFILE, false);
  sanitizePath(WEIGHTS_OUTFILE, false);
  sanitizePath(WEIGHTS_GENERIC_OUTFILENAME, false);
  sanitizePath(HID_FILE, false);
  sanitizePath(STATS_FILE, false);
  sanitizePath(CONSOLE_FILE, false);
  sanitizePath(RULES_OUTFILE, false);
  sanitizePath(GLOBAL_RULES_OUTFILE, false);
  sanitizePath(EXPLANATION_FILE, false);
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
    if (param[0] == 'H') {
      std::string numberPart = param.substr(1);
      if (checkInt(numberPart)) {
        option = H;
      } else {
        option = INVALID;
      }
    } else {
      option = INVALID;
    }
  }

  switch (option) {

  case TRAIN_DATA_FILE:
    setString(TRAIN_DATA_FILE, arg); // Parameter after -T
    break;

  case TRAIN_PRED_FILE:
    setString(TRAIN_PRED_FILE, arg);
    break;

  case TRAIN_PRED_OUTFILE:
    setString(TRAIN_PRED_OUTFILE, arg);
    break;

  case TEST_DATA_FILE:
    setString(TEST_DATA_FILE, arg); // Parameter after -T
    break;

  case TEST_PRED_FILE:
    setString(TEST_PRED_FILE, arg);
    break;

  case TEST_PRED_OUTFILE:
    setString(TEST_PRED_OUTFILE, arg);
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

  case VALID_DATA_FILE:
    setString(VALID_DATA_FILE, arg);
    break;

  case VALID_CLASS_FILE:
    setString(VALID_CLASS_FILE, arg);
    break;

  case VALID_PRED_OUTFILE:
    setString(VALID_PRED_OUTFILE, arg);
    break;

  case WEIGHTS_FILE:
    setString(WEIGHTS_FILE, arg);
    break;

  case WEIGHTS_OUTFILE:
    setString(WEIGHTS_OUTFILE, arg);
    break;

  case WEIGHTS_GENERIC_FILENAME:
    setString(WEIGHTS_GENERIC_FILENAME, arg);
    completePath(WEIGHTS_GENERIC_FILENAME);
    break;

  case WEIGHTS_GENERIC_OUTFILENAME:
    setString(WEIGHTS_GENERIC_OUTFILENAME, arg);
    break;

  case HID_FILE:
    setString(HID_FILE, arg);
    break;

  case STATS_FILE:
    setString(STATS_FILE, arg);
    break;

  case RULES_FILE:
    setString(RULES_FILE, arg);
    break;

  case RULES_OUTFILE:
    setString(RULES_OUTFILE, arg);
    break;

  case NB_DIMLP_NETS:
    setInt(NB_DIMLP_NETS, arg);
    break;

  case NB_EX_PER_NET:
    setInt(NB_EX_PER_NET, arg);
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
    checkPath(ROOT_FOLDER, arg); // root directory existence must be checked
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

  case LEARNING_RATE:
    setFloat(LEARNING_RATE, arg);
    break;

  case MOMENTUM:
    setFloat(MOMENTUM, arg);
    break;

  case FLAT:
    setFloat(FLAT, arg);
    break;

  case ERROR_THRESH:
    setFloat(ERROR_THRESH, arg);
    break;

  case ACC_THRESH:
    setFloat(ACC_THRESH, arg);
    break;

  case ABS_ERROR_THRESH:
    setFloat(ABS_ERROR_THRESH, arg);
    break;

  case NB_EPOCHS:
    setInt(NB_EPOCHS, arg);
    break;

  case NB_EPOCHS_ERROR:
    setInt(NB_EPOCHS_ERROR, arg);
    break;

  case WITH_RULE_EXTRACTION:
    setBool(WITH_RULE_EXTRACTION, arg);
    break;

  case COVERING_STRATEGY:
    setBool(COVERING_STRATEGY, arg);
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

  case H:
    setArch(H, arg, param);
    break;

  default: // If we put another -X option
    throw CommandArgumentException("Illegal option : " + param);
  }
}

void Parameters::setInt(ParameterCode id, const string &value) {
  if (isIntSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (checkInt(value)) {
    _intParams[id] = stoi(value, nullptr);
  } else {
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

  if (checkFloat(value)) {
    _floatParams[id] = stof(value, nullptr);
  } else {
    throwInvalidDataTypeException(id, value, "float");
  }
}

void Parameters::setFloat(ParameterCode id, float value) {
  if (isFloatSet(id)) {
    throwAlreadySetArgumentException(id, to_string(value));
  }

  _floatParams[id] = value;
}

void Parameters::checkPath(ParameterCode id, const string &path) const {
  if (!exists(path)) {
    throwInvalidFileOrDirectory(id, path);
  }
}

void Parameters::setDouble(ParameterCode id, const string &value) {
  if (isDoubleSet(id)) {
    throwAlreadySetArgumentException(id, value);
  }

  if (checkFloat(value)) {
    _doubleParams[id] = stod(value, nullptr);
  } else {
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

void Parameters::setArch(ParameterCode id, const string &value, const string &param) {
  if (checkInt(value)) {
    arch.Insert(stoi(value));

    const char *ptrParam = param.c_str();

    if (ptrParam[1] != '\0') {
      std::string str(ptrParam + 1);
      archInd.Insert(stoi(str));
    } else {
      throw CommandArgumentException("Error : Which hidden layer (-H) ?");
    }
  } else
    throwInvalidDataTypeException(id, value, "integer");
}

/**
 * @brief Completes the path of a given parameter with the ROOT_FOLDER, also checks if the given parameter is not empty
 *
 * @param id is the given parameter to be completed
 */
void Parameters::completePath(ParameterCode id) {
  // avoid error cases
  if (!isStringSet(id) || !isStringSet(ROOT_FOLDER) || id == ROOT_FOLDER) {
    return;
  }

  string fullPath;
  string target = getString(id);
  string separator = getOSSeparator();
  string root = isStringSet(ROOT_FOLDER) ? getString(ROOT_FOLDER) : "";

  if (target.empty() || target.back() == separator[0]) {
    throwInvalidFileOrDirectory(id, target);
  }

  if (root.empty() || !root.empty() && (root.back() == separator[0] || target.front() == separator[0])) {
    fullPath = root + target;
  } else {
    fullPath = root + separator + target;
  }

  _stringParams[id] = fullPath;
}

/**
 * @brief handles every aspect of parameters that represents files:
 *
 * -  Checks if a file exists if "shouldFileExist" is set to true
 * -  appends the ROOT_FOLDER path to every file if ROOT_FOLDER is set
 *
 * @param id parameter id to be processed
 * @param shouldFileExist whether it should check the existance of the file itself or only the path
 */
void Parameters::sanitizePath(ParameterCode id, bool shouldFileExist) {
  // ignore if target is not set and avoid duplicating the root path for no reason
  if (!isStringSet(id) || id == ROOT_FOLDER) {
    return;
  }

  completePath(id);
  std::string fullPath = getString(id);

  if (shouldFileExist) {
    checkPath(id, fullPath);
  } else {
    checkPath(id, fullPath.substr(0, fullPath.find_last_of(getOSSeparator()) + 1));
  }
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

StringInt Parameters::getArch() const {
  return arch;
}

StringInt Parameters::getArchInd() const {
  return archInd;
}

std::string Parameters::getParameterName(ParameterCode id) {
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

void Parameters::setDefaultString(ParameterCode id, const string &defaultValue, bool withRoot) {
  if (!isStringSet(id)) {
    std::string value = defaultValue;
    if (withRoot) {
      // define separator depending on OS
      std::string separator = getOSSeparator();
      value = getString(ROOT_FOLDER) + separator + defaultValue;
    }
    setString(id, value);
  }
}

void Parameters::checkAttributeAndClassCounts() {

  if (getInt(NB_ATTRIBUTES) < 1) {
    throw CommandArgumentException("Error : Number of attributes must be strictly positive (>=1).");
  }

  if (getInt(NB_CLASSES) < 2) {
    throw CommandArgumentException("Error : Number of classes must be greater than 1.");
  }
}

void Parameters::checkParametersCommon() {

  checkAttributeAndClassCounts();

  if (getInt(NB_QUANT_LEVELS) <= 2) {
    throw CommandArgumentException("Error : Number of stairs in staircase activation function must be greater than 2.");
  }
}

void Parameters::checkParametersFidex() {

  if (isStringSet(WEIGHTS_FILE) && isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : Do not specify both a weight file and an rules input file. Choose one of them.");
  }

  if (!isStringSet(WEIGHTS_FILE) && !isStringSet(RULES_FILE)) {
    throw CommandArgumentException("Error : A weight file or a rules file has to be given. Give the weights file with option -W or the rules file with option -f");
  }

  if (getInt(NB_DIMLP_NETS) < 1) {
    throw CommandArgumentException("Error : Number of networks must be strictly positive (>=1).");
  }

  if (getInt(MAX_ITERATIONS) < 1) {
    throw CommandArgumentException("Error : Maximum number of iterations must be strictly positive (>=1).");
  }

  if (getInt(MIN_COVERING) < 1) {
    throw CommandArgumentException("Error : Minimium covering must be strictly positive (>=1).");
  }

  if (getInt(MAX_FAILED_ATTEMPTS) < 0) {
    throw CommandArgumentException("Error : Maximum number of failed attempts has to be positive (>=0)");
  }

  if (getFloat(MIN_FIDELITY) < 0.0f || getFloat(MIN_FIDELITY) > 1.0f) {
    throw CommandArgumentException("Error : Minimum fidelity has to be between [0.0, 1.0]");
  }

  if (getFloat(DROPOUT_DIM) < 0.0f || getFloat(DROPOUT_DIM) > 1.0f) {
    throw CommandArgumentException("Error : Dropout dim must be between [0.0, 1.0].");
  }

  if (getFloat(DROPOUT_HYP) < 0.0f || getFloat(DROPOUT_HYP) > 1.0f) {
    throw CommandArgumentException("Error : Dropout hyp must be between [0.0, 1.0].");
  }

  if (getInt(SEED) < 0) {
    throw CommandArgumentException("Error : random seed mus be positive (>=0).");
  }
}

void Parameters::checkParametersDecisionThreshold() {

  if ((getFloat(DECISION_THRESHOLD) < 0.0f || getFloat(DECISION_THRESHOLD) > 1.0f) && getFloat(DECISION_THRESHOLD) != -1.0f) {
    throw CommandArgumentException("Error : Decision threshold must be beetween [0.0, 1.0].");
  }

  if (getInt(POSITIVE_CLASS_INDEX) < 0 && getInt(POSITIVE_CLASS_INDEX) != -1) {
    throw CommandArgumentException("Error : Positive class index must be positive (>=0)");
  }

  if (getInt(POSITIVE_CLASS_INDEX) >= getInt(NB_CLASSES)) {
    throw CommandArgumentException("Error : The index of positive class cannot be greater or equal to the number of classes (" + to_string(getInt(NB_CLASSES)) + ").");
  }

  if (getFloat(DECISION_THRESHOLD) != -1 && getInt(POSITIVE_CLASS_INDEX) == -1) {
    throw CommandArgumentException("Error : The positive class index has to be given with option --positive_class_index if the decision threshold is given (--decision_threshold)");
  }
}

void Parameters::checkParametersDimlpTrn() {

  if (getFloat(LEARNING_RATE) <= 0) {
    throw CommandArgumentException("The learning parameter must be strictly positive (>0).");
  }

  if (getFloat(MOMENTUM) < 0) {
    throw CommandArgumentException("The momentum parameter must be positive (>=0).");
  }

  if (getFloat(FLAT) < 0) {
    throw CommandArgumentException("The flat parameter must be positive (>=0).");
  }

  if (getFloat(ERROR_THRESH) < 0 && getFloat(ERROR_THRESH) != -1111111111.0f) {
    throw CommandArgumentException("The error threshold must be positive (>=0).");
  }

  if ((getFloat(ACC_THRESH) <= 0 || getFloat(ACC_THRESH) > 1) && getFloat(ACC_THRESH) != 11111111111111.0f) {
    throw CommandArgumentException("The accuracy threshold must be between ]0,1].");
  }

  if (getFloat(ABS_ERROR_THRESH) < 0) {
    throw CommandArgumentException("The delta error parameter (ABS_ERROR_THRESH) must be positive (>=0, 0=no delta).");
  }

  if (getInt(NB_EPOCHS) < 1) {
    throw CommandArgumentException("Error : Number of epochs must be strictly positive (>=1).");
  }

  if (getInt(NB_EPOCHS_ERROR) < 1) {
    throw CommandArgumentException("Error : Number of epochs to show errors must be strictly positive (>=1).");
  }

  if (getInt(SEED) < 0) {
    throw CommandArgumentException("Error : random seed mus be positive (>=0).");
  }
}

void Parameters::checkParametersNormalization() {

  // Check denormalization parameters

  // If normalizationIndices were not specified, it's all attributes
  if (!isStringSet(NORMALIZATION_FILE) && !isIntVectorSet(NORMALIZATION_INDICES) && isDoubleVectorSet(MUS)) {
    vector<int> normalizationIndicesTemp;
    for (int i = 0; i < getInt(NB_ATTRIBUTES); ++i) {
      normalizationIndicesTemp.push_back(i);
    }
    setIntVector(NORMALIZATION_INDICES, normalizationIndicesTemp);
  }

  // Check if mus and sigmas are both given or both not
  if ((isDoubleVectorSet(MUS) || isDoubleVectorSet(SIGMAS)) &&
      !(isDoubleVectorSet(MUS) && isDoubleVectorSet(SIGMAS))) {
    throw CommandArgumentException("Error : One of Mus(--mus) and sigmas(--sigmas) is given but not the other.");
  }

  if (isStringSet(NORMALIZATION_FILE) && isDoubleVectorSet(MUS) || isStringSet(NORMALIZATION_FILE) && isIntVectorSet(NORMALIZATION_INDICES)) {
    throw CommandArgumentException("Error : normlization file (--normalization_file) and mus or normalizationIndices (--normalization_indices) are both given.");
  }

  // Mus, sigmas and normalizationIndices must have the same size and not be empty
  if (isDoubleVectorSet(MUS) && (getDoubleVector(MUS).size() != getDoubleVector(SIGMAS).size() || getDoubleVector(MUS).size() != getIntVector(NORMALIZATION_INDICES).size() || getDoubleVector(MUS).empty())) {
    throw CommandArgumentException("Error : mus (--mus), sigmas (--sigmas) and normalization indices (--normalization_indices) don't have the same size or are empty.");
  }

  // Check normalizationIndices
  if (isIntVectorSet(NORMALIZATION_INDICES)) {
    vector<int> tempVect = getIntVector(NORMALIZATION_INDICES);
    std::set<int> uniqueIndices(tempVect.begin(), tempVect.end());
    if (uniqueIndices.size() != getIntVector(NORMALIZATION_INDICES).size() ||
        *std::max_element(uniqueIndices.begin(), uniqueIndices.end()) >= getInt(NB_ATTRIBUTES) ||
        *std::min_element(uniqueIndices.begin(), uniqueIndices.end()) < 0) {
      throw CommandArgumentException("Error : parameter normalization indices (--normalization_indices) must be a list composed of integers between [0, nb_attributes-1] without repeted elements.");
    }
  }
}

void Parameters::setDefaultNbQuantLevels() {
  setDefaultInt(NB_QUANT_LEVELS, 50);
}

void Parameters::setDefaultFidex() {
  setDefaultInt(NB_DIMLP_NETS, 1);
  setDefaultInt(MAX_ITERATIONS, 10);
  setDefaultInt(MIN_COVERING, 2);
  setDefaultInt(MAX_FAILED_ATTEMPTS, 30);
  setDefaultFloat(MIN_FIDELITY, 1.0);
  setDefaultFloat(DROPOUT_DIM, 0.0f);
  setDefaultFloat(DROPOUT_HYP, 0.0f);
  setDefaultFloat(HI_KNOT, 5.0f);
  setDefaultInt(SEED, 0);
  if (isStringSet(WEIGHTS_FILE)) {
    setWeightsFiles(); // must be called to initialize
  }
}

void Parameters::setDefaultDecisionThreshold() {
  setDefaultFloat(DECISION_THRESHOLD, -1.0f);
  setDefaultInt(POSITIVE_CLASS_INDEX, -1);
}

void Parameters::setDefaultDimlpTrn() {
  setDefaultFloat(LEARNING_RATE, 0.1f);
  setDefaultFloat(MOMENTUM, 0.6f);
  setDefaultFloat(FLAT, 0.01f);
  setDefaultFloat(ERROR_THRESH, -1111111111.0f);
  setDefaultFloat(ACC_THRESH, 11111111111111.0f);
  setDefaultFloat(ABS_ERROR_THRESH, 0.0f);
  setDefaultInt(NB_EPOCHS_ERROR, 10);
  setDefaultInt(NB_EPOCHS, 1500);
  setDefaultBool(WITH_RULE_EXTRACTION, false);
  setDefaultInt(SEED, 0);
}