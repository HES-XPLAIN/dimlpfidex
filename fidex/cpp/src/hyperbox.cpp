using namespace std;
#include "hyperbox.h"
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

Hyperbox::Hyperbox(vector<pair<int, int>> m_discriminativeHyperplans){
    discriminativeHyperplans = m_discriminativeHyperplans;

}

Hyperbox::Hyperbox(){

}

void Hyperbox::setCoveredSamples(vector<int> m_coveredSamples){
    coveredSamples = m_coveredSamples;
}

vector<pair<int, int>> Hyperbox::getDiscriminativeHyperplans(){
    return discriminativeHyperplans;
}


void Hyperbox::computeCoveredSamples(vector<int> ancienCoveredSamples, int attribut, vector<vector<double>>* trainData, bool mainSampleGreater, double hypValue, vector<double>* mainSampleData, vector<vector<double>> hyperLocus){
    vector<int> newCoveredSamples;
    for (int l=0; l<ancienCoveredSamples.size(); l++){ // We check all already covered samples
        int idCoveredSample = ancienCoveredSamples[l];
        double sampleValue = (*trainData)[idCoveredSample][attribut];
        bool sampleGreater = hypValue <= sampleValue;
        if (!(mainSampleGreater ^ sampleGreater)){ // If both samples are on same side of hyperplan (^ = xor)
            newCoveredSamples.push_back(idCoveredSample); // This sample is covered again
        }
    }
    coveredSamples = newCoveredSamples;
    //cout << "Size: " << coveredSamples.size() << " ";

    //check if newcoveredSamples are correct with current rule
    vector<tuple<int, bool, double>> currentRule;
    bool inequality;
    for (int k=0; k<getDiscriminativeHyperplans().size() ; k++){
        attribut = getDiscriminativeHyperplans()[k].first%(*mainSampleData).size();
        hypValue = hyperLocus[getDiscriminativeHyperplans()[k].first][getDiscriminativeHyperplans()[k].second];
        double mainSampleValue = (*mainSampleData)[attribut];
        if(hypValue <= mainSampleValue){
        inequality = 1;
        //inequalityBool = 1;
        }
        else{
        inequality = 0;
        //inequalityBool = 0;
        }
        currentRule.push_back(make_tuple(attribut,inequality,hypValue));

    }

    //Ajouter l'antécédent courant !!!
    
    bool covered;
    int nbCovered = 0;
    for (int d=0; d<coveredSamples.size(); d++){
        covered = true;
        for (auto ant : currentRule){
                if (get<1>(ant) == 0){
                    if ((*trainData)[d][get<0>(ant)] >= get<2>(ant)){
                        covered = false;
                        break;
                    }
                }
                else{
                    if ((*trainData)[d][get<0>(ant)] < get<2>(ant)){
                        covered = false;
                        break;
                    }
                }
            }
            if (covered){
                nbCovered += 1;
            }
    }

    if (nbCovered != coveredSamples.size()){
        cout << "PROBLEM" << endl;
        cout << "Size : " << coveredSamples.size() << endl;
        cout << "True size : " << nbCovered << endl;
    }
    else{
        cout << "Size : " << coveredSamples.size() << " True size : " << nbCovered << endl;
    }

}

vector<int> Hyperbox::getCoveredSamples(){
    return coveredSamples;
}

void Hyperbox::computeFidelity(const int mainsamplePred, vector<int>* trainPreds){
    int coveredTrueClass = 0; // Number of samples covered by the hyperbox and of same class as the example
    int nbCovered = coveredSamples.size(); // Number of samples covered by the hyperbox
    for (int k=0;k<coveredSamples.size();k++) { // Loop on all covered samples
        int idSample = coveredSamples[k];
        if (mainsamplePred == (*trainPreds)[idSample]){ // Check if sample is of right class (class predicted by dimlp network for our main sample)
            coveredTrueClass+=1;
        }
    }
    fidelity = (double)coveredTrueClass/(double)nbCovered;
}

double Hyperbox::getFidelity(){
    return fidelity;
}

void Hyperbox::setFidelity(double x){
    fidelity = x;
}

void Hyperbox::discriminateHyperplan(int i, int j){
    discriminativeHyperplans.push_back(make_pair(i,j));
}

void Hyperbox::resetDiscriminativeHyperplans(){
    vector<pair<int, int>> disc;
    discriminativeHyperplans = disc;
}