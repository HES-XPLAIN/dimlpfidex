using namespace std;
#include "dataSet.h"
#include <fstream>
#include <sstream>
#include <algorithm>

DataSet::DataSet(char* dataFile, char* predFile, char* trueClassFile){
    hasDatas = true;
    int i; // iterator
    string line;

    // Get data

    fstream fileDta;

    fileDta.open(dataFile,ios::in); //Read data file
    if(fileDta.fail()){
      throw std::runtime_error("Error : file " + std::string(dataFile) + " not found");
    }

    while (! fileDta.eof() ){
          getline(fileDta, line);
          if (line.length()!=0){
            std::stringstream myLine(line);
            double value;
            vector<double> tempVect;
            while ( myLine >> value ){
              tempVect.push_back(value);
            }
            datas.push_back(tempVect);
          }
    }

    fileDta.close(); //close data file

    

    // Get data class

    if (trueClassFile != 0){

      hasClasses = true;
      fstream fileCl;

      fileCl.open(trueClassFile,ios::in); //read true dataclass file
      if(fileCl.fail()){
        throw std::runtime_error("Error : file " + std::string(trueClassFile) + " not found");
      }

      while (! fileCl.eof() ){
            i = 0;
            getline(fileCl, line);
            if (line.length()!=0){
              std::stringstream myLine(line);
              int value;
              while ( myLine >> value ){
                if (value == 1){
                  trueClasses.push_back(i);
                  break;
                }
                i++;
              }
            }
      }

      fileCl.close(); //close file
    }

    

    // Get predictions

    fstream filePrd;

    filePrd.open(predFile,ios::in); //read predictions data file
    if(filePrd.fail()){
      throw std::runtime_error("Error : file " + std::string(predFile) + " not found");
    }

    while (! filePrd.eof() ){
          i = 0;
          getline(filePrd, line);
          if (line.length()!=0){
            std::stringstream myLine(line);
            double value;
            vector <double> values;
            while ( myLine >> value ){
              values.push_back(value);
            }
            outputValuesPredictions.push_back(values);
            predictions.push_back(std::max_element(values.begin(),values.end()) - values.begin());
          }
    }

    filePrd.close(); //close file
}

DataSet::DataSet(char* weightFile){

    // Get weights
    hasWeights = true;
    fstream fileWts;

    string line;

    fileWts.open(weightFile,ios::in); //Read weight file
    if(fileWts.fail()){
      throw std::runtime_error("Error : file " + std::string(weightFile) + " not found");
    }

    while (! fileWts.eof() ){
      getline(fileWts, line);
      if (line.length()!=0){
        std::stringstream myLine(line);
        double value;
        vector<double> tempVect;
        while ( myLine >> value ){
          tempVect.push_back(value);
        }
        weights.push_back(tempVect);
      }

    }
    fileWts.close(); //close file
}


vector<vector<double>>* DataSet::getDatas(){
  if (hasDatas){
    return &datas;
  }
  else{
    throw std::runtime_error("Error : data file not specified for this dataset");
  }
}

vector<int>* DataSet::getTrueClasses(){
  if (hasClasses){
    return &trueClasses;
  }
  else{
    throw std::runtime_error("Error : dataClass file not specified for this dataset");
  }
}

vector<int>* DataSet::getPredictions(){
  if (hasDatas){
    return &predictions;
  }
  else{
    throw std::runtime_error("Error : prediction file not specified for this dataset");
  }
}

vector<vector<double>>* DataSet::getOutputValuesPredictions(){
  if (hasDatas){
    return &outputValuesPredictions;
  }
  else{
    throw std::runtime_error("Error : prediction file not specified for this dataset");
  }
}

vector<vector<double>> DataSet::getWeights(){
  if (hasWeights){
    return weights;
  }
  else{
    throw std::runtime_error("Error : weight file not specified for this dataset");
  }
}

vector<double> DataSet::getInBiais(){
  if (hasWeights){
    return weights[0];
  }
  else{
    throw std::runtime_error("Error : weight file not specified for this dataset");
  }
}

vector<double> DataSet::getInWeights(){
  if (hasWeights){
    return weights[1];
  }
  else{
    throw std::runtime_error("Error : weight file not specified for this dataset");
  }
}