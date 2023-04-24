using namespace std;
#include "hyperspace.h"
#include <fstream>
#include <sstream>

Hyperspace::Hyperspace(char* hyperLocusFile){

     // Get hyperLocus
    fstream fileLocus;

    string line;

    fileLocus.open(hyperLocusFile,ios::in); //Read hyperLocus file
    if(fileLocus.fail()){
      throw std::runtime_error("Error : file " + std::string(hyperLocusFile) + " not found");
    }

    while (! fileLocus.eof() ){
      getline(fileLocus, line);
      if (line.length()!=0){
        std::stringstream myLine(line);
        double value;
        vector<double> tempVect;
        while ( myLine >> value ){
          tempVect.push_back(value);
        }
        hyperLocus.push_back(tempVect);
      }

    }
    fileLocus.close(); //close file

    vector<pair<int, int>> discriminativeHyperplans;

    hyperbox = new Hyperbox(discriminativeHyperplans);

}


vector<vector<double>> Hyperspace::getHyperLocus(){
    return hyperLocus;
}

Hyperbox* Hyperspace::getHyperbox(){
    return hyperbox;
}

void Hyperspace::ruleExtraction(vector<double>* mainSampleData, const int mainSamplePred, double ruleAccuracy, double ruleConfidence, vector<string> &lines){
    
  double hypValue;
  int attribut;
  bool inequalityBool;


  string line;
  string inequality;
  for (int k=0; k<hyperbox->getDiscriminativeHyperplans().size() ; k++){
    
    attribut = hyperbox->getDiscriminativeHyperplans()[k].first%(*mainSampleData).size();
    hypValue = hyperLocus[hyperbox->getDiscriminativeHyperplans()[k].first][hyperbox->getDiscriminativeHyperplans()[k].second];
    double mainSampleValue = (*mainSampleData)[attribut];
    if(hypValue <= mainSampleValue){
      inequality = ">=";
      //inequalityBool = 1;
    }
    else{
      inequality = "<";
      //inequalityBool = 0;
    }
    //rule.push_back(make_tuple(attribut, inequalityBool, hypValue));
    line += "X" + std::to_string(attribut) + inequality + std::to_string(hypValue) + " ";
  }
  line += "-> class " + std::to_string(mainSamplePred);
  line += " Covering size : " + std::to_string(hyperbox->getCoveredSamples().size());
  line += " Fidelity : " + std::to_string(hyperbox->getFidelity());
  line += " Accuracy : " + std::to_string(ruleAccuracy);
  line += " Confidence : " + std::to_string(ruleConfidence);

  std::cout << "Extracted rule :" << endl;
  std::cout << line << endl << endl;
  lines.push_back(line);
}


double Hyperspace::computeRuleAccuracy(vector<int>* trainPreds, vector<int>* trainTrueClass, bool mainSampleCorrect){ // Percentage of correct model prediction on samples covered by the rule
  int idSample;
  int total = 0; // Number of indexes predicted good 
  for (int i=0; i<hyperbox->getCoveredSamples().size(); i++){
    idSample = hyperbox->getCoveredSamples()[i];
    if ((*trainPreds)[idSample] == (*trainTrueClass)[idSample]){
      total += 1;
    }
  }
  if (mainSampleCorrect){ // Add test sample value
    total+=1;
  }
  int nbCovered = hyperbox->getCoveredSamples().size();
  nbCovered+=1;

  return float(total)/nbCovered;
}

double Hyperspace::computeRuleConfidence(vector<vector<double>>* trainOutputValuesPredictions, const int mainSamplePred, double mainSamplePredValue){ // Mean output value of prediction of class chosen by the rule for the covered samples
  int idSample;
  int classSample;
  double total = 0; // Number of indexes predicted good 
  for (int i=0; i<hyperbox->getCoveredSamples().size(); i++){
    idSample = hyperbox->getCoveredSamples()[i];
    total += (*trainOutputValuesPredictions)[idSample][mainSamplePred]; // value of output prediction of class chosen by the rule (mainSamplePred)
  }

  total += mainSamplePredValue; // Add test sample value


  int nbCovered = hyperbox->getCoveredSamples().size();
  nbCovered+=1;


  return float(total)/nbCovered;
}