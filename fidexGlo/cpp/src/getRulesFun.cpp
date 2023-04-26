#include "getRulesFun.h"
#include <fstream>
#include <sstream>

////////////////////////////////////////////////////////

void getRules(vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>>& rules, vector<string>& lines, vector<string>& stringRules, char* rulesFile, int nbTestAttributs){

    tuple<vector<tuple<int, bool, double>>, int, int, double, double> rule; // <[X0<0.606994 X15>=0.545037], 12(cov size), 0(class), 1(fidelity), 0.92(accuracy)> 

    fstream rulesData;
    rulesData.open(rulesFile,ios::in); // Read data file
    if(rulesData.fail()){
      throw std::runtime_error("Error : file " + std::string(rulesFile) + " not found");
    }

    lines.push_back("Global statistics of the rule set : ");

    string value;
    int attr;
    bool ineq;
    double val;
    int ruleClass;
    int covSize;
    double ruleFidelity;
    double ruleAccuracy;
    string line;
    getline(rulesData, line); // Skip first line;
    lines.push_back(line + "\n"); // Add global statistics to output file
    while (! rulesData.eof() ){
      getline(rulesData, line);
      vector<tuple<int, bool, double>> antecedents;
      if (line.length()!=0){
        std::stringstream myLine(line);
        stringRules.push_back(line);
        while ( myLine >> value ){
          if(value[0]=='X'){ // New attribut
            if (value.find('<') != std::string::npos){
              ineq = 0;
              int pos = value.find("<");
              attr = stoi(value.substr(1,pos-1));
              val = stof(value.erase(0, pos + 1));
            }
            else{
              ineq = 1;
              int pos = value.find(">=");
              attr = stoi(value.substr(1,pos-1));
              val = stof(value.erase(0, pos+2));
            }
            if (attr>=nbTestAttributs){
              throw std::runtime_error("Error : There is not enough attributs values in the test sample");
            }
            antecedents.push_back(make_tuple(attr, ineq, val));
          }
          if(value == "class"){ // Class
            myLine >> ruleClass;
          }
          if(value == "Covering"){ // Covering size
            myLine >> value;
            myLine >> value;
            myLine >> covSize;
          }
          if(value == "Fidelity"){ // Rule fidelity
            myLine >> value;
            myLine >> ruleFidelity;
          }
          if(value == "Accuracy"){ // Rule accuracy
            myLine >> value;
            myLine >> ruleAccuracy;
          }
        }
        rule = make_tuple(antecedents, covSize, ruleClass, ruleFidelity, ruleAccuracy);
        rules.push_back(rule);
      }
    }
    rulesData.close(); //close data file
}    
    
    
////////////////////////////////////////////////////////
    
void getActivatedRules(vector<int>& activatedRules, vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>>* rules, vector<double>* testValues){
    int attr;
    bool ineq;
    double val;
    for (int r=0; r<(*rules).size(); r++){ // For each rule
        bool notActivated = false;
        for(int a=0; a<get<0>((*rules)[r]).size(); a++){ // For each antecedent
            attr = get<0>(get<0>((*rules)[r])[a]);
            ineq = get<1>(get<0>((*rules)[r])[a]);
            val = get<2>(get<0>((*rules)[r])[a]);
            if (ineq == 0 && (*testValues)[attr] >= val){ // If the inequality is not verified
                notActivated = true;
            }
            if (ineq == 1 && (*testValues)[attr] < val){
                notActivated = true;
            }
        }
        if (!notActivated){
            activatedRules.push_back(r);
        }
    }
}
