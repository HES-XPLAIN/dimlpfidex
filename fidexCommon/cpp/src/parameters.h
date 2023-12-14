#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "checkFun.h"
#include "errorHandler.h"
#include <iostream>
#include <omp.h>
#include <ostream>
#include <sstream>
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
  string getTrainDataFile() const { return _trainDataFile; }
  string getTrainDataFilePred() const { return _trainDataFilePred; }
  string getTrainDataFileTrueClass() const { return _trainDataFileTrueClass; }
  string getInputRulesFile() const { return _inputRulesFile; }
  string getRulesFile() const { return _rulesFile; }
  string getConsoleFile() const { return _consoleFile; }
  string getRootFolder() const { return _rootFolder; }
  string getAttributFile() const { return _attributFile; }
  string getWeightsFile() const { return _weightsFile; }
  vector<string> getWeightsFiles() const { return _weightsFiles; }
  int getNbDimlpNets() const { return _nbDimlpNets; }
  int getNbQuantLevels() const { return _nbQuantLevels; }
  int getHeuristic() const { return _heuristic; }
  int getItMax() const { return _itMax; }
  int getMinNbCover() const { return _minNbCover; }
  int getMaxFailedAttempts() const { return _maxFailedAttempts; }
  int getNbThreadsUsed() const { return _nbThreadsUsed; }
  int getIndexPositiveClass() const { return _indexPositiveClass; }
  int getSeed() const { return _seed; }
  bool getHeuristicInit() const { return _heuristicInit; }
  bool getHasDecisionThreshold() const { return _hasDecisionThreshold; }
  bool getHasIndexPositiveClass() const { return _indexPositiveClass; }
  double getHiKnot() const { return _hiKnot; }
  double getDropoutHyp() const { return _dropoutHyp; }
  double getDropoutDim() const { return _dropoutDim; }
  double getMinFidelity() const { return _minFidelity; }
  double getDecisionThreshold() const { return _decisionThreshold; }
};

inline ostream &operator<<(ostream &stream, const Parameters &p) {
  stream << "Parameters list:                 " << endl
         << "   Train data file:              " << p.getTrainDataFile() << endl
         << "   Train data predictions file:  " << p.getTrainDataFilePred() << endl
         << "   Train data true classes file: " << p.getTrainDataFileTrueClass() << endl
         << "   Input rules file:             " << p.getInputRulesFile() << endl
         << "   Generated rules file:         " << p.getRulesFile() << endl
         << "   Console file:                 " << p.getConsoleFile() << endl
         << "   Root path:                    " << p.getRootFolder() << endl
         << "   Attributes file:              " << p.getAttributFile() << endl
         << "   Weights file:                 " << p.getWeightsFile() << endl
         << "   # DIMLP nets:                 " << p.getNbDimlpNets() << endl
         << "   nbQuantLevels:                " << p.getNbQuantLevels() << endl
         << "   heuristic:                    " << p.getHeuristic() << endl
         << "   itMax:                        " << p.getItMax() << endl
         << "   minNbCover:                   " << p.getMinNbCover() << endl
         << "   maxFailedAttempts:            " << p.getMaxFailedAttempts() << endl
         << "   nbThreadsUsed:                " << p.getNbThreadsUsed() << endl
         << "   indexPositiveClass:           " << p.getIndexPositiveClass() << endl
         << "   seed:                         " << p.getSeed() << endl
         << "   heuristicInit:                " << p.getHeuristicInit() << endl
         << "   hasDecisionThreshold:         " << p.getHasDecisionThreshold() << endl
         << "   hasIndexPositiveClass:        " << p.getHasIndexPositiveClass() << endl
         << "   hiKnot:                       " << p.getHiKnot() << endl
         << "   dropoutHyp:                   " << p.getDropoutHyp() << endl
         << "   dropoutDim:                   " << p.getDropoutDim() << endl
         << "   minFidelity:                  " << p.getMinFidelity() << endl
         << "   decisionThreshold:            " << p.getDecisionThreshold() << endl
         << endl;

  return stream;
}

#endif // PARAMETERS_H
