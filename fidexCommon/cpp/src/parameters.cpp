#include "parameters.h"

/**
 * @brief Construct a new Parameters object containing all arguments passed by CLI.
 *
 * @param args program arguments
 */
Parameters::Parameters(vector<string> args) {
  // setting default values
  _nbDimlpNets = 1;    // Number of networks. 1 means no baggingu
  _nbQuantLevels = 50; // Number of steps of the step function
  _heuristic = -1;
  _itMax = 10;     // We stop if we have more than itMax iterations (impossible to have a rule with more than itMax antecedents)
  _minNbCover = 2; // Minimum size of covering that we ask
  _maxFailedAttempts = 30;
  _nbThreadsUsed = 1;
  _indexPositiveClass = -1;
  _seed = 0;
  _heuristicInit = false;
  _hasDecisionThreshold = false;
  _hasIndexPositiveClass = false;
  _hiKnot = 5; // High end of the interval for each dimension, a hyperplan can't be after
  _dropoutHyp = 0.5;
  _dropoutDim = 0.5;
  _minFidelity = 1;
  _decisionThreshold = -1;

  // TODO: I don't think it's apropriate to check for errors here. Should be done wherever it is used instead.

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
        _trainDataFile = arg; // Parameter after -T
        break;

      case 'P':
        _trainDataFilePred = arg;
        break;

      case 'C':
        _trainDataFileTrueClass = arg;
        break;

      case 'W': {
        _weightsFile = arg;
      } break;

      case 'f': {
        _inputRulesFile = arg;
      } break;

      case 'N':
        if (CheckPositiveInt(arg))
          _nbDimlpNets = atoi(arg);
        else
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");

        break;

      case 'Q':
        if (CheckPositiveInt(arg)) {
          _nbQuantLevels = atoi(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
        }
        break;

      case 'O':
        _rulesFile = arg;
        break;

      case 'r':
        _consoleFile = arg;
        break;

      case 'A':
        _attributFile = arg;
        break;

      case 'S':
        _rootFolder = arg;
        break;

      case 'M':
        if (CheckPositiveInt(arg) && atoi(arg) >= 1 && atoi(arg) <= 3) {
          _heuristic = atoi(arg);
          _heuristicInit = true;
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", heuristic must be an integer between 1 and 3(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo)");
        }
        break;

      case 'i':
        if (CheckPositiveInt(arg)) {
          _itMax = atoi(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
        }
        break;

      case 'v':
        if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
          _minNbCover = atoi(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", strictly positive integer requested");
        }
        break;

      case 'p':
        if (CheckPositiveInt(arg) && atoi(arg) >= 1) {
          if (atoi(arg) <= omp_get_num_procs()) {
            _nbThreadsUsed = atoi(arg);
          }
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", zero or positive integer requested");
        }
        break;

      case 'y':
        if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
          _minFidelity = atof(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
        }
        break;

      case 'd':
        if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
          _dropoutDim = atof(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
        }
        break;

      case 'h':
        if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
          _dropoutHyp = atof(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
        }
        break;

      case 'm':
        if (CheckPositiveInt(arg) && atoi(arg) > 0) {
          _maxFailedAttempts = atoi(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", strictly positive integer requested");
        }
        break;

      case 't':
        if (CheckFloatFid(arg) && atof(arg) >= 0 && atof(arg) <= 1) {
          _hasDecisionThreshold = true;
          _decisionThreshold = atof(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", float included in [0,1] requested");
        }
        break;

      case 'x':
        if (CheckPositiveInt(arg)) {
          _hasIndexPositiveClass = true;
          _indexPositiveClass = atoi(arg);
        } else {
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");
        }
        break;

      case 'z':
        if (CheckPositiveInt(arg))
          _seed = atoi(arg);
        else
          throw CommandArgumentException("Error : invalid type for parameter " + string(lastArg) + ", positive integer requested");

        break;

      default: // If we put another -X option
        throw CommandArgumentException("Illegal option : " + string(lastArg));
      }
    }
  }

  setRootDirectory();
}

/**
 * @brief Sets all root directories of files if root directory has been specified
 *
 */
void Parameters::setRootDirectory() {
  if (_rootFolder.empty()) {
    return;
  }

#if defined(__unix__) || defined(__APPLE__)
  _rootFolder += "/";
#elif defined(_WIN32)
  _rootFolder += "\\";
#endif

  cout << _rootFolder << endl;

  if (!_trainDataFile.empty()) {
    cout << _trainDataFile << endl;
    _trainDataFile = _rootFolder + _trainDataFile;
    cout << _trainDataFile << endl;
  }

  if (!_trainDataFilePred.empty()) {
    _trainDataFilePred = _rootFolder + _trainDataFilePred;
  }

  if (!_trainDataFileTrueClass.empty()) {
    _trainDataFileTrueClass = _rootFolder + _trainDataFileTrueClass;
  }

  if (!_rulesFile.empty()) {
    _rulesFile = _rootFolder + _rulesFile;
  }

  if (!_weightsFiles.empty()) {
    for (int i = 0; i < _weightsFiles.size(); i++) {
      _weightsFiles[i] = _rootFolder + _weightsFiles[i];
      ;
    }
  }

  if (!_inputRulesFile.empty()) {
    _inputRulesFile = _rootFolder + _inputRulesFile;
  }

  if (!_attributFile.empty()) {
    _attributFile = _rootFolder + _attributFile;
  }

  if (!_weightsFile.empty()) {
    _weightsFile = _rootFolder + _weightsFile;
  }
}

// TODO: Implement Parameters::Parameters(string jsonfile) {}
