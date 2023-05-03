#include "fidexAlgo.h"
#include <time.h>
#include "dataSet.h"
#include "checkFun.h"
#include <string>
#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>
#include <limits.h>

using namespace std;


void showParams(){
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "fidexGloRules -T <train dataset file> -P <train prediction file> -C <train true class file> ";
  std::cout << "-H <hyperLocus file> ";
  std::cout << "-O <Rules output file> ";
  std::cout << "-M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> ";
  std::cout << "<Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-i <max iteration number>\n";
  std::cout << "-v <minimum covering number>\n";
  std::cout << "-d <dimension dropout parameter>\n";
  std::cout << "-h <hyperplan dropout parameter>\n";

  std::cout << "\n-------------------------------------------------\n\n";

}


int main(int nbParam, char** param)
{
  try{

    float temps;
    clock_t t1, t2;

    t1 = clock();

    // Import parameters
    if(nbParam == 1){
      showParams();
      return -1;
    }


    // Parameters declaration

    char* trainDataFile      = 0; // Train data
    bool trainDataFileInit = false;
    char* trainDataFilePred  = 0; // Train class predictions from dimlp
    bool trainDataFilePredInit = false;
    char* trainDataFileTrueClass = 0; // Train true classes
    bool trainDataFileTrueClassInit = false;

    bool hyperLocusFileInit = false;
    char* hyperLocusFile = 0;
    bool rulesFileInit = false;
    char* rulesFile = 0;
    int heuristic = -1;
    bool heuristicInit = false;

    //const char* trainDataFile      = 'train';
    //const char* trainDataFilePred  = 'netTrainAns';
    //const char* trainDataFileTrueClass = 'trainClass';

    int itMax = 100;  // We stop if we have more than itMax iterations
    int minNbCover = 2; // Minimum size of covering that we ask
    bool dropoutHyp = false; // We dropout a bunch of hyperplans each iteration (could accelerate the processus)
    double dropoutHypParam = 0.5;
    bool dropoutDim = false; // We dropout a bunch of dimensions each iteration (could accelerate the processus)
    double dropoutDimParam = 0.5;
    DataSet* trainDatas;

    int nbRules; // Number of rules created


  // Import parameters

    for (int p=1; p<nbParam; p++){ // We skip "fidexGloRules"
      if(*param[p] == '-'){
        p++;

        if (p >= nbParam){
          throw std::runtime_error("Missing something at the end of the command.");
        }

        switch(*(param[p-1] + 1)){ // Get letter after the -

          case 'T' :
            trainDataFile = param[p]; // Parameter after -T
            trainDataFileInit = true;
            break;

          case 'P' :
            trainDataFilePred = param[p];
            trainDataFilePredInit = true;
            break;

          case 'C' :
            trainDataFileTrueClass = param[p];
            trainDataFileTrueClassInit = true;
            break;

          case 'H' :
            hyperLocusFile = param[p];
            hyperLocusFileInit = true;
            break;

          case 'O' :
            rulesFile = param[p];
            rulesFileInit = true;
            break;

          case 'M' :
            if (CheckPositiveInt(param[p]) && atoi(param[p])>=1 && atoi(param[p])<=3){
              heuristic = atoi(param[p]);
              heuristicInit = true;
            }
            else{
              throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", heuristic must be an integer between 1 and 3(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo)");
            }
            break;

          case 'i' :
            if (CheckPositiveInt(param[p])){
              itMax = atoi(param[p]);
            }
            else{
              throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", positive integer requested");
            }
            break;

          case 'v' :
            if (CheckPositiveInt(param[p]) && atoi(param[p])>=1){
              minNbCover = atoi(param[p]);
            }
            else{
              throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", strictly positive integer requested");
            }
            break;

          case 'd' :
            if (CheckFloat(param[p]) && atof(param[p])>=0 && atof(param[p])<=1){
              dropoutDimParam = atof(param[p]);
              dropoutDim = true; // We dropout a bunch of dimensions each iteration (accelerate the processus)
              }
              else{
                throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", float included in [0,1] requested");
              }
            break;

            case 'h' :
            if (CheckFloat(param[p]) && atof(param[p])>=0 && atof(param[p])<=1){
              dropoutHypParam = atof(param[p]);
              dropoutHyp = true; // We dropout a bunch of hyperplans each iteration (accelerate the processus)
              }
              else{
                throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", float included in [0,1] requested");
              }
            break;

          default  : // If we put another -X option
            throw std::runtime_error("Illegal option : "+ string(param[p-1]));
        }

      }
    }

    if (!trainDataFileInit){
        throw std::runtime_error("The train data file has to be given with option -T");
    }
    if (!trainDataFilePredInit){
        throw std::runtime_error("The train prediction file has to be given with option -P");
    }
    if (!trainDataFileTrueClassInit){
        throw std::runtime_error("The train true classes file has to be given with option -C");
    }
    if (!hyperLocusFileInit){
        throw std::runtime_error("The hyperLocus file has to be given with option -H");
    }
    if (!rulesFileInit){
        throw std::runtime_error("The output rules file has to be given with option -O");
    }
    if (!heuristicInit){
        throw std::runtime_error("The heuristic(1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo) has to be given with option -M");
    }

    std::cout << "\nParameters :\n\n";
    std::cout << "- Max iteration number : " << itMax << endl;
    std::cout << "- Min size of covering : " << minNbCover << endl;
    if (dropoutDim){
        std::cout << "- We use a dimension dropout of " << dropoutDimParam << endl << endl;
    }
    else{
        std::cout << "- We don't use dimension dropout\n\n";
    }
    if (dropoutHyp){
        std::cout << "- We use a hyperplan dropout of " << dropoutHypParam << endl << endl;
    }
    else{
        std::cout << "- We don't use hyperplan dropout\n\n";
    }

    // Import files

    std::cout << "Import files..." << endl;

    trainDatas = new DataSet(trainDataFile, trainDataFilePred, trainDataFileTrueClass);

    vector<vector<double>>* trainData = trainDatas->getDatas();
    vector<int>* trainPreds = trainDatas->getPredictions();
    vector<vector<double>>* trainOutputValuesPredictions = trainDatas->getOutputValuesPredictions();
    vector<int>* trainTrueClass = trainDatas->getTrueClasses();

    const int nbDatas = (*trainData).size();
    if((*trainPreds).size() != nbDatas || (*trainTrueClass).size() != nbDatas){
      throw std::runtime_error("All the train files need to have the same amount of datas");
    }


    if (minNbCover > nbDatas){
    throw std::runtime_error("Error : invalide type for parameter -c, strictly positive integer smaller or equal than the number of data sample requested");
  }

    std::cout << "Files imported" << endl << endl;

    // compute hyperspace

    std::cout << "Creation of hyperspace..." << endl;

    Hyperspace hyperspace(hyperLocusFile); // Initialize hyperbox and get hyperplans

    const int nbIn = hyperspace.getHyperLocus().size(); // Number of neurons in the first hidden layer (May be the number of input variables or a multiple)
    const int nbAttributs = (*trainData)[0].size();
    const int nbHyp = hyperspace.getHyperLocus()[0].size();

    // Check size of hyperlocus
    if (nbIn == 0 || nbIn % nbAttributs != 0 ){
        throw std::runtime_error("Error : the size of hyperLocus - " + std::to_string(nbIn) + " is not a multiple of the number of attributs - " + std::to_string(nbAttributs));
    }

    std::cout << "Hyperspace created" << endl << endl;

    // Samples not yet covered by any rules
    vector<int> notCoveredSamples(nbDatas);
    std::iota(std::begin(notCoveredSamples), std::end(notCoveredSamples),0); // Vector from 0 to nbDatas-1

    vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> chosenRules; // antecedents, cover vector, class, rule accuracy, rule confidence




//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------



// First heuristic : optimal (slower)
    float temps1;
    clock_t c1, c2;

    c1 = clock();
    if(heuristic == 1){
      std::cout << "Optimal FidexGlo" << endl << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl << endl;
      for(int idSample=0; idSample<nbDatas; idSample++){
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        while(!ruleCreated){
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam);
          if(currentMinNbCov>=2){
            currentMinNbCov-=1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
        }
        if(currentMinNbCov+1 < minNbCover){
          nbProblems += 1;
        }

        rules.push_back(rule);
      }

      std::cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;

      std::cout << "Fidex rules computed" << endl << endl;

      std::cout << "Computing global ruleset..." << endl << endl;

      nbRules = 0;

      // While there is some not covered samples
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> currentRule;
      std::vector<int>::iterator ite;
      vector<int> newNotCoveredSamples;
      vector<int> bestNewNotCoveredSamples;
      while (notCoveredSamples.size()!=0){

        //Get rule that covers the most new samples
        int bestRule = -1;
        int bestCovering = INT_MAX;
        int currentCovering; // Size of new covering if we choose this rule
        for (int r=0; r<rules.size(); r++){
          newNotCoveredSamples = notCoveredSamples;
          // Remove samples that are in current covering
          ite = std::set_difference (newNotCoveredSamples.begin(), newNotCoveredSamples.end(), get<1>(rules[r]).begin(), get<1>(rules[r]).end(), newNotCoveredSamples.begin()); //vectors have to be sorted
          newNotCoveredSamples.resize(ite-newNotCoveredSamples.begin());
          currentCovering = newNotCoveredSamples.size();

          if (currentCovering < bestCovering){
            bestRule = r;
            bestCovering = currentCovering;
            bestNewNotCoveredSamples = newNotCoveredSamples;
          }
        }
        currentRule = rules[bestRule];

        notCoveredSamples = bestNewNotCoveredSamples; // Delete new covered samples


        nbRules += 1;
        //std::cout << "Rule number " << nbRules << " ";
        //std::cout << "Size of covering : " << std::get<1>(currentRule).size() << " ";
        chosenRules.push_back(currentRule); // add best rule with maximum covering
        //cout << "nbRules : " << chosenRules.size() << " ";
        //std::cout << "New size : " << notCoveredSamples.size() << " ";

        rules.erase(rules.begin()+bestRule); //Remove this rule

        //cout << "Rules left : " << rules.size() << endl;

      }


      std::cout << "Global ruleset Computed" << endl << endl;

      std::cout << "We created " << nbRules << " rules." << endl << endl;

      c2 = clock();
      temps1 = (float)(c2-c1)/CLOCKS_PER_SEC;
      std::printf("\nTime first heuristic = %f sec\n\n", temps1);

    }


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------


    // Second heuristic : fast
    float temps2;
    clock_t d1, d2;

    d1 = clock();
    if(heuristic == 2){
      std::cout << "Fast FidexGlo" << endl << endl;

      vector<tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double>> rules;
      bool ruleCreated;
      int nbProblems = 0;
      int currentMinNbCov;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      // Get the rule for each data sample from fidex
      std::cout << "Computing fidex rules..." << endl << endl;
      for(int idSample=0; idSample<nbDatas; idSample++){

        currentMinNbCov = minNbCover;
        ruleCreated = false;
        while(!ruleCreated){
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam);
          if(currentMinNbCov>=2){
            currentMinNbCov-=1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
        }
        if(currentMinNbCov+1 < minNbCover){
          nbProblems += 1;
        }

        rules.push_back(rule);
      }

      std::cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;


      std::cout << "Fidex rules computed" << endl << endl;

      std::cout << "Computing global ruleset..." << endl << endl;

      // Sort the rules(dataIds) depending of their covering size
      vector<int> dataIds = notCoveredSamples;
      std::sort(dataIds.begin(), dataIds.end(), [&rules](int ruleBest, int ruleWorst){
          return std::get<1>(rules[ruleWorst]).size() < std::get<1>(rules[ruleBest]).size();
      });

      nbRules = 0;

      // While there is some not covered samples
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> currentRule;
      int ancienNotCoveringSize = notCoveredSamples.size();
      int dataId = 0;

      std::vector<int>::iterator ite;
      while (notCoveredSamples.size()!=0){
        currentRule = rules[dataIds[dataId]];

        // Delete covered samples
        ite = std::set_difference (notCoveredSamples.begin(), notCoveredSamples.end(), get<1>(currentRule).begin(), get<1>(currentRule).end(), notCoveredSamples.begin()); //vectors have to be sorted
        notCoveredSamples.resize(ite-notCoveredSamples.begin());

        // If the rule coveres a new sample
        if (ancienNotCoveringSize > notCoveredSamples.size()){
          nbRules += 1;
          //std::cout << "Rule number " << nbRules << " ";
          //std::cout << "Size of covering : " << std::get<1>(currentRule).size() << " ";
          chosenRules.push_back(currentRule); // add best rule with maximum covering
          //cout << "nbRules : " << chosenRules.size() << " ";
          //std::cout << "New size : " << notCoveredSamples.size() << " ";
        }
        ancienNotCoveringSize = notCoveredSamples.size();

        //cout << "Rules left : " << rules.size()-dataId << endl;

        dataId+=1;

      }


      std::cout << "Global ruleset Computed" << endl << endl;

      std::cout << "We created " << nbRules << " rules." << endl << endl;

      d2 = clock();
      temps2 = (float)(d2-d1)/CLOCKS_PER_SEC;
      std::printf("\nTime second heuristic = %f sec\n\n", temps2);

    }


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------


    // Third heuristic : Very fast
    float temps3;
    clock_t e1, e2;

    e1 = clock();

    if(heuristic == 3){
      std::cout << "Very fast FidexGlo" << endl << endl;

      //  Sort data randomly
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::shuffle(std::begin(notCoveredSamples), std::end(notCoveredSamples), std::default_random_engine(seed));

      nbRules = 0;
      int idSample;
      bool ruleCreated;
      int currentMinNbCov;
      int nbProblems = 0;
      tuple<vector<tuple<int, bool, double>>, vector<int>, int, double, double> rule; // Ex: ([X2<3.5 X3>=4], covering, class)
      FidexAlgo exp = FidexAlgo();

      std::cout << "Computing rules..." << endl << endl;

      // While there is some not covered samples
      while (notCoveredSamples.size()!=0)
      {

        //std::cout << "iteration " << nbRules << " ";

        idSample = notCoveredSamples[0];
        //idSample = 14;
        currentMinNbCov = minNbCover;
        ruleCreated = false;
        while(!ruleCreated){
          ruleCreated = exp.fidex(rule, trainData, trainPreds, trainOutputValuesPredictions, trainTrueClass, &(*trainData)[idSample], (*trainPreds)[idSample], (*trainTrueClass)[idSample], &hyperspace, nbIn, nbAttributs, nbHyp, itMax, currentMinNbCov, dropoutDim, dropoutDimParam, dropoutHyp, dropoutHypParam);
          if(currentMinNbCov>=2){
            currentMinNbCov-=1; // If we didnt found a rule with desired covering, we check with a lower covering
          }
          if(currentMinNbCov+1 < minNbCover){
            nbProblems += 1;
          }
        }

        //std::cout << "Sample : " << idSample << " ";
        //std::cout << "Size of covering : " << std::get<1>(rule).size() << " ";
        /*cout << "Covered Samples : ";
        for (int i=0;i<coveredSamples.size();i++){
          cout << coveredSamples[i] << " ";
        }
        cout << endl;
        cout << "Rule :" << endl;
        for (int j=0; j<get<0>(rule).size(); j++){
          cout  << std::get<0>(std::get<0>(rule)[j]) << " " << std::get<1>(std::get<0>(rule)[j]) << " " << std::get<2>(std::get<0>(rule)[j]) << endl;
        }
        cout << "Rule Accuracy : " << std::get<3>(rule) << endl;
        */

        chosenRules.push_back(rule); // We add the new rule

        // Delete covered samples
        notCoveredSamples.erase(
          std::remove_if(
            notCoveredSamples.begin(), notCoveredSamples.end(), [&rule](int x){
              return std::find(get<1>(rule).begin(), get<1>(rule).end(), x) != get<1>(rule).end();
              // find index of coveredSamples which is x (x is element of notCoveredSamples), std::find returns last if x not found
              // -> Removes x if it appears on coveredSamples (found before the end of coveredSamples)
            }
          )
          , notCoveredSamples.end()
        );


        //std::cout << "New size : " << notCoveredSamples.size() << endl;

        nbRules++;

      }

      std::cout << endl;

      std::cout << "Number of sample with lower covering than " << minNbCover << " : " << nbProblems << endl;

      std::cout << "We created " << nbRules << " rules." << endl << endl;


      e2 = clock();
      temps3 = (float)(e2-e1)/CLOCKS_PER_SEC;
      std::printf("\nTime third heuristic = %f sec\n\n", temps3);

    }


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------


    std::cout << "Rules extraction..." << endl << endl;

    double meanCovSize = 0;
    double meanNbAntecedents = 0;

    //Export rules
    string inequality;
    string line;
    vector<string> lines;

    // Sort the rules(RulesIds) depending of their covering size
    vector<int> RulesIds(nbRules); // Rules indexes
    std::iota(std::begin(RulesIds), std::end(RulesIds),0); // Vector from 0 to nbRules-1
    std::sort(RulesIds.begin(), RulesIds.end(), [&chosenRules](int ruleBest, int ruleWorst){
        return std::get<1>(chosenRules[ruleWorst]).size() < std::get<1>(chosenRules[ruleBest]).size();
    });


    for (int c=0;c<nbRules;c++){ // each rule
      int r = RulesIds[c];
      meanCovSize+=get<1>(chosenRules[r]).size();
      meanNbAntecedents+=get<0>(chosenRules[r]).size();
      line = "";
      line += std::to_string(get<0>(chosenRules[r]).size()) + " : "; // If we need to specify the antecedant number
      for (int a=0;a<get<0>(chosenRules[r]).size();a++){ // each antecedant
        if(get<1>(get<0>(chosenRules[r])[a]) == 1){ // check inequality
          inequality = ">=";
        }
        else{
          inequality = "<";
        }
        line += "X" + std::to_string(get<0>(get<0>(chosenRules[r])[a])) + inequality + std::to_string(get<2>(get<0>(chosenRules[r])[a])) + " "; // Write antecedant
      }
      line += "-> class " + std::to_string(std::get<2>(chosenRules[r])); // class of the rule
      line += " Covering size : " + std::to_string(std::get<1>(chosenRules[r]).size()); // Covering size
      line += " Fidelity : 1"; // Rule fidelity
      line += " Accuracy : " + std::to_string(std::get<3>(chosenRules[r])); // Rule accuracy
      line += " Confidence : " + std::to_string(std::get<4>(chosenRules[r])) + "\n"; // Rule confidence
      lines.push_back(line);
    }


    meanCovSize/=nbRules;
    meanNbAntecedents/=nbRules;

    ofstream file2 (rulesFile);
    if(file2.is_open()){
      // We add to the file the number of rules, the mean covering per rule and the mean number of antecedents per rule
      string startLine = "Number of rules : " + std::to_string(nbRules) + ", mean sample covering number per rule : " + std::to_string(meanCovSize) + ", mean number of antecedents per rule : " + std::to_string(meanNbAntecedents)+"\n";
      file2 << startLine;
      for (int l=0; l<lines.size(); l++){
        file2 << lines[l];
      }

      file2.close();

    }
    else{
      throw std::runtime_error("Error : Couldn't open rules extraction file " + std::string(rulesFile) + ".");
    }

    std::cout << "Mean covering size per rule : " << meanCovSize << endl;
    std::cout << "Mean number of antecedents per rule : " << meanNbAntecedents << endl;

    t2 = clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    std::printf("\nFull execution time = %f sec\n", temps);
  }
    catch (const char* msg) {
    std::printf(msg);
    cerr << msg << endl;
  }


}


//Exemple : .\fidexGloRules.exe -T ../fidexGlo/datafiles/datanorm -P ../fidexGlo/datafiles/dimlp.out -C ../fidexGlo/datafiles/dataclass2 -H ../fidexGlo/datafiles/hyperLocus -O ../fidexGlo/datafiles/globalRules.txt -M 3 -i 100 -v 2 -d 0.5 -h 0.5
// .\fidexGloRules.exe -T ../fidexGlo/datafiles/datanormTrain -P ../fidexGlo/datafiles/dimlpDatanormTrain.out -C ../fidexGlo/datafiles/dataclass2Train -H ../fidexGlo/datafiles/hyperLocusDatanorm -O ../fidexGlo/datafiles/globalRulesDatanorm.txt -M 1 -i 100 -v 2 -d 0.5 -h 0.5
