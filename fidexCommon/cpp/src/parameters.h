#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "checkFun.h"
#include "errorHandler.h"
#include <omp.h>
#include <string>
#include <vector>

using namespace std;

// TODO: adapt to other programs, yet it is only based on fidexGloRules parameters and can only work with it
class Parameters {
private:
  string _trainDataFile;          // Train data
  string _trainDataFilePred;      // Train class predictions from dimlp
  string _trainDataFileTrueClass; // Train true classes
  string _inputRulesFile;         // Rule file to be converted to hyperlocus, replace weight file
  string _rulesFile;
  string _consoleFile;
  string _rootFolder;
  string _attributFile; // attribut file
  string _weightsFile;
  vector<string> _weightsFiles;
  int _nbDimlpNets;   // Number of networks. 1 means no bagging
  int _nbQuantLevels; // Number of steps of the step function
  int _heuristic;
  int _itMax;      // We stop if we have more than itMax iterations (impossible to have a rule with more than itMax antecedents)
  int _minNbCover; // Minimum size of covering that we ask
  int _maxFailedAttempts;
  int _nbThreadsUsed;
  int _indexPositiveClass;
  int _seed;
  bool _heuristicInit;
  bool _hasDecisionThreshold;
  bool _hasIndexPositiveClass;
  double _hiKnot; // High end of the interval for each dimension, a hyperplan can't be after
  double _dropoutHyp;
  double _dropoutDim;
  double _minFidelity;
  double _decisionThreshold;
  void setRootDirectory();

public:
  Parameters() = default;
  Parameters(vector<string> args);
  Parameters(string jsonfile);

  // SPECIAL OP
  void addWeightFile(string file) {
    _weightsFiles.push_back(file);
  }

  // GETTERS
  string getTrainDataFile() { return _trainDataFile; }
  string getTrainDataFilePred() { return _trainDataFilePred; }
  string getTrainDataFileTrueClass() { return _trainDataFileTrueClass; }
  string getInputRulesFile() { return _inputRulesFile; }
  string getRulesFile() { return _rulesFile; }
  string getConsoleFile() { return _consoleFile; }
  string getRootFolder() { return _rootFolder; }
  string getAttributFile() { return _attributFile; }
  string getWeightsFile() { return _weightsFile; }
  vector<string> getWeightsFiles() { return _weightsFiles; }
  int getNbDimlpNets() { return _nbDimlpNets; }
  int getNbQuantLevels() { return _nbQuantLevels; }
  int getHeuristic() { return _heuristic; }
  int getItMax() { return _itMax; }
  int getMinNbCover() { return _minNbCover; }
  int getMaxFailedAttempts() { return _maxFailedAttempts; }
  int getNbThreadsUsed() { return _nbThreadsUsed; }
  int getIndexPositiveClass() { return _indexPositiveClass; }
  int getSeed() { return _seed; }
  bool getHeuristicInit() { return _heuristicInit; }
  bool getHasDecisionThreshold() { return _decisionThreshold; }
  bool getHasIndexPositiveClass() { return _indexPositiveClass; }
  double getHiKnot() { return _hiKnot; }
  double getDropoutHyp() { return _dropoutHyp; }
  double getDropoutDim() { return _dropoutDim; }
  double getMinFidelity() { return _minFidelity; }
  double getDecisionThreshold() { return _decisionThreshold; }
};

#endif // PARAMETERS_H
