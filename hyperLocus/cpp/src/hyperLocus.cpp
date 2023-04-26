#include <time.h>
#include "checkFun.h"
#include "dataSet.h"
#include <fstream>
#include <iostream>



void showParams(){
  cout << "\n-------------------------------------------------\n\n";

  cout << "Obligatory parameters : \n\n";
  cout << "hyperLocus -W <file of weights> ";
  cout << "-Q <number of stairs in staircase activation function> ";
  cout << "-I <high side of the interval> \n\n";

  cout << "Options are: \n\n";
  cout << "-O <output file>\n";

  cout << "\n-------------------------------------------------\n\n";



}


vector<vector<double>> calcHypLocus(int nbQuantLevels, double lowKnot, double hiKnot, vector<double> vectBias, vector<double> vectWeights){

    int nbIn = vectBias.size(); // Number of neurons in the first hidden layer (May be the number of input variables)
    int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

    double dist = hiKnot - lowKnot; // Size of the interval
    double binWidth = dist/nbQuantLevels; // Width of a box between 2 separations

    vector<vector<double>> matHypLocus(nbIn, vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
    std::vector<double> knots(nbKnots); // Vector of location of the separations for one dimension (hyperplans will be placed close)


    for(int k = 0; k<nbKnots; k++){
        knots[k] = lowKnot + (binWidth*k);  // location of each separation within a dimension (hyperplans will be placed close)
    }

    for(int i = 0; i<nbIn; i++){ // Loop on dimension
        for(int j = 0; j<nbKnots; j++){
            matHypLocus[i][j] = (knots[j] - vectBias[i]) / vectWeights[i]; // Placement of the hyperplan
        }
    }

    return matHypLocus;

}






int main(int nbParam, char** param)
{
  try{

    float temps;
    clock_t t1, t2;

    t1 = clock();


      // Parameters declaration

      char* dataFileWeights = 0; // Weights obtained with dimlp
      bool dataFileWeightsInit = false;

      int nbQuantLevels; // Number of steps of the step function
      bool nbQuantLevelsInit = false;
      double hiKnot; // High end of the interval for each dimension, a hyperplan can't be after
      bool hiKnotInit = false;

      vector<vector<double>> hyperLocus;

      string outputFile = "hyperLocus.txt";

      // Import parameters

      if(nbParam == 1){
        showParams();
        return -1;
      }

      for (int p=1; p<nbParam; p++){ // We skip "fidex"
        if(*param[p] == '-'){
          p++;

          switch(*(param[p-1] + 1)){ // Get letter after the -

            case 'W' :
              dataFileWeights = param[p];
              dataFileWeightsInit = true;
              break;

            case 'Q' :
              if (CheckPositiveInt(param[p])){
                nbQuantLevels = atoi(param[p]);
                nbQuantLevelsInit = true;
              }
              else{
                throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", positive integer requested");
              }
              break;

            case 'I' :
              if (CheckFloat(param[p]) && atof(param[p])>0){
                hiKnot = atof(param[p]);
                hiKnotInit = true;
              }
              else{
                throw std::runtime_error("Error : invalide type for parameter " + string(param[p-1]) +", strictly positive float requested");
              }
              break;

              case 'O' :
                outputFile = param[p];
              break;

            default  : // If we put another -X option
              throw std::runtime_error("Illegal option : "+ string(param[p-1]));
          }
        }
      }

      if (!dataFileWeightsInit){
        throw std::runtime_error("The weight file has to be given with option -W");
      }
      if (!nbQuantLevelsInit){
        throw std::runtime_error("The number of steps in the step function has to be given with option -Q");
      }
      if (!hiKnotInit){
        throw std::runtime_error("The right end of the interval in which hyperplans are contained has to be given with option -I");
      }


    double lowKnot = -hiKnot;

    cout << "\nParameters :\n\n";
    cout << "- Number of stairs " << nbQuantLevels << endl;
    cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << endl << endl;

    cout << "Import weight file..." << endl;

    DataSet weightDatas(dataFileWeights);

    vector<vector<double>> allWeights = weightDatas.getWeights();
    vector<double> biais = weightDatas.getInBiais();
    vector<double> weights = weightDatas.getInWeights();

    cout << "Weight file imported" << endl << endl;

    cout << "computation of hyperLocus" << endl;
    hyperLocus = calcHypLocus(nbQuantLevels, lowKnot, hiKnot, biais, weights);
    cout << "HyperLocus computed" << endl << endl;

    cout << "Extraction of the hyperLocus" << endl;

    ofstream hyperLocusFile (outputFile);
    if(hyperLocusFile.is_open()){
      for (int i=0;i<hyperLocus.size(); i++){
        for (int j=0; j<hyperLocus[0].size(); j++){
          hyperLocusFile << hyperLocus[i][j] << " ";
        }
        hyperLocusFile << "\n";
      }
      hyperLocusFile.close();
    }
    else{
      throw std::runtime_error("Error : Couldn't open file " + std::string(outputFile) + ".");
    }


    t2 = clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("\nTime to compute the hyperLocus = %f sec\n", temps);
  }

  catch (const char* msg) {
    printf(msg);
       cerr << msg << endl;
  }

}


  //Exemple : hyperLocus -W datafiles/dimlp.wts -Q 50 -I 5 -O datafiles/hyperLocus
