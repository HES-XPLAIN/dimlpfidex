using namespace std;
#include <vector>
#include <iostream>
#include <cstring>

class DataSet {

      vector<vector<double>> datas;
      vector<int> trueClasses;
      vector<int> predictions;
      vector<vector<double>> outputValuesPredictions;
      vector<vector<double>> weights;
      bool hasDatas = 0;
      bool hasClasses = 0;
      bool hasWeights = 0;

public:

      DataSet();
      DataSet(char* dataFile, char* predFile, char* trueClassFile = 0);
      DataSet(char* weightFile);

      vector<vector<double>>* getDatas();
      vector<int>* getTrueClasses();
      vector<int>* getPredictions();
      vector<vector<double>>* getOutputValuesPredictions();
      vector<vector<double>> getWeights();
      vector<double> getInBiais();
      vector<double> getInWeights();

};
