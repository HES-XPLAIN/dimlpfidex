using namespace std;
#include "DimlpTrnFct.h"
#include <random>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>


void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "crossValid -L <training set file> -1 <file of train classes> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";
   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-K <K-fold cross-validation>\n"; // Not to be 10
   cout << "-N <number of times we do the cross-validation>\n"; // Not to be 10
   cout << "-F <Folder where to save weights, predictions and datas after training>\n"; // Not to be CrossValidation
   cout << "-A <file of attributes>\n";
   cout << "-W <file of pretrained weights>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-l <back-propagation learning parameter (Eta)>\n";
   cout << "-m <back-propagation momentum parameter (Mu)>\n";
   cout << "-f <back-propagation flat spot elimination parameter (Flat)>\n";
   cout << "-q <number of stairs in staircase activation function>\n";
   cout << "-e <error threshold>\n";
   cout << "-a <accuracy threshold>\n";
   cout << "-d <absolute difference error threshold>\n";
   cout << "-i <number of epochs>\n";
   cout << "-s <number of epochs to show error>\n";

   cout << "\n-------------------------------------------------\n\n";
}

// copy in binary mode
bool copyFile(const char *SRC, const char* DEST)
{
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    return src && dest;
}

////////////////////////////////////////////////////////////

int main(int nbParam, char** param)

{

    int k;

    DataSet  Train;
    DataSet  TrainClass;

    int N = 10; // Number of times we do cross-validation
    int K = 10; // Number of divisions of the dataset (10 = 8 for train, 1 for test and 1 for validation)

    char* learnTar   = 0;
    char* learnFile  = 0;
    char* folder = (char*) "CrossValidation";

    string genericCommand = "dimlpTrn";

    string eta, mu, flat, errThres, accThres, deltaErr, showErr, epochs, quant, nbIn, nbOut, arch, arch2, attrFile, weightFile;
    char* ptrParam;

    if (nbParam == 1)
    {
        GiveAllParam();
        return -1;
    }

    for (k=1; k<nbParam; k++)
    {
        if (*param[k] == '-')
        {
            k++;

            if (k >= nbParam)
            {
                if (*(param[k-1] + 1) != 'R')
                {
                    cout << "Missing something at the end of the command.\n";
                    return -1;
                }
            }

            switch(*(param[k-1] + 1))
            {
                case 'l' :
                            eta = param[k];
                            genericCommand += " -l " + eta;
                            break;

                case 'm' :
                            mu = param[k];
                            genericCommand += " -m " + mu;
                            break;

                case 'f' :
                            flat = param[k];
                            genericCommand += " -f " + flat;
                            break;

                case 'e' :
                            errThres = param[k];
                            genericCommand += " -e " + errThres;
                            break;

                case 'a' :
                            accThres = param[k];
                            genericCommand += " -a " + accThres;
                            break;

                case 'd' :

                            deltaErr = param[k];
                            genericCommand += " -d " + deltaErr;
                            break;

                case 's' :
                            showErr = param[k];
                            genericCommand += " -s " + showErr;
                            break;

                case 'i' :
                            epochs = param[k];
                            genericCommand += " -i " + epochs;
                            break;

                case 'q' :
                            quant = param[k];
                            genericCommand += " -q " + quant;
                            break;

                case 'I' :
                           nbIn = param[k];
                           genericCommand += " -I " + nbIn;
                           break;

                case 'H' :
                           ptrParam = param[k-1];

                           if (ptrParam[2] != '\0'){
                              arch = param[k];
                              arch2 = ptrParam[2];
                              genericCommand += " -H" + arch2 + " " + arch;
                           }
                           else{
                               cout << "Which hidden layer (-H) ?\n";
                               return -1;
                           }

                           break;

                case 'O' :
                           nbOut = param[k];
                           genericCommand += " -O " + nbOut;
                           break;

                case 'A' :
                            attrFile = param[k];
                            genericCommand += " -A " + attrFile;
                            break;

                case 'W' :
                            weightFile = param[k];
                            genericCommand += " -W " + weightFile;
                            break;

                case 'L' : learnFile  = param[k];
                            break;

                case '1' : learnTar   = param[k];
                            break;

                case 'K' : if (CheckInt(param[k]))
                              K = atoi(param[k]);
                           else return -1;

                           break;

                case 'N' : if (CheckInt(param[k]))
                              N = atoi(param[k]);
                           else return -1;

                           break;

                case 'F' : folder  = param[k];
                           break;

                default  : cout << "Illegal option: " << param[k-1] << "\n";
                            return -1;
            }
        }

        else
        {
            cout << "Illegal option: " << param[k] << "\n";
            return -1;
        }
    }

// ----------------------------------------------------------------------

   if (N <= 0)
   {
      cout << "The number of times we do cross-validation must be at least 1, change the option N.\n";
      return -1;
   }

   if (K <= 2)
   {
      cout << "The number of divisions of the dataset must be at least 3, change the option -I.\n";
      return -1;
   }

   if (learnFile == 0)
   {
      cout << "Give the training file with -L selection please." << "\n";
      return -1;
   }

   if (learnTar == 0)
   {
      cout << "Give the training target file with -1 selection please." << "\n";
      return -1;
   }

    // Get datas on vector
    fstream fileDta;

    fileDta.open(learnFile,ios::in); //Read data file
    if(fileDta.fail()){
        throw std::runtime_error("Error : file " + std::string(learnFile) + " not found");
    }
    vector<string> learnData;
    string line;
    while (! fileDta.eof() ){
        getline(fileDta, line);
        if (line.length()!=0){
            learnData.push_back(line);
        }
    }
    fileDta.close(); //close data file


    const int nbSamples = learnData.size();
    if (K > nbSamples)
    {
        cout << "The number of divisions of the dataset must be less or equal to the number of train samples(" << nbSamples << "), change the option -I.\n";
        return -1;
    }

    vector<string> learnTarData;

    // Get datas on vector
    fstream fileDtaTar;

    fileDtaTar.open(learnTar,ios::in); //Read data file
    if(fileDtaTar.fail()){
        throw std::runtime_error("Error : file " + std::string(learnTar) + " not found");
    }

    while (! fileDtaTar.eof() ){
        getline(fileDtaTar, line);
        if (line.length()!=0){
            learnTarData.push_back(line);
        }
    }
    fileDtaTar.close(); //close data file

    if (learnTarData.size() != nbSamples)
    {
        cout << "The number of samples in each learn files need to be the same.\n";
        return -1;
    }


    /*for (int i=0; i<learnData.size(); i++){
        cout << learnData[i] << endl;
    }

    for (int i=0; i<learnTarData.size(); i++){
        cout << learnTarData[i] << endl;
    }*/


    // Create folder if doesn't exist
    struct stat sb;
    if (stat(folder, &sb) == 0){
        cout << "Please, choose a folder with -F that doesn't already exist.\n";
        return -1;
    }
    #ifdef __unix__
    mkdir(folder, 0777);
    #elif defined(_WIN32)
    mkdir(folder);
    #endif

    std::random_device rd;
    // Loop on N executions of cross-validation
    for (int n=0; n<N; n++){
        // Create folder
        string folderName = std::string(folder)+"\\Execution"+std::to_string(n+1);
        string folderNameLinux = std::string(folder)+"/Execution"+std::to_string(n+1)+"/";

        #ifdef __unix__
        mkdir(folderNameLinux.c_str(), 0777);
        #elif defined(_WIN32)
        mkdir(folderName.c_str());
        #endif



        // Randomly split data in K sub-parts
        vector<vector<string>> learnDataSplit;
        vector<vector<string>> learnTarDataSplit;

        std::vector<int> indexes;
        for (int i = 0; i < nbSamples; ++i)
            indexes.push_back(i);
        std::mt19937 g(rd());
        std::shuffle(indexes.begin(), indexes.end(), g);

        // Vector of eveanly spaced numbers between 1 and nbSammples+1
        vector<double> range;
        double delta = nbSamples/double(K);
        for(int i=0; i<K+1; i++) {
            range.push_back(i*delta);
        }
        // Split data evenly in K groups
        for (int q=0; q<range.size()-1; q++){
            vector<string> tempVect;
            vector<string> tempVectTar;
            for(int ind=range[q]; ind<int(range[q+1]); ind ++){
                tempVect.push_back(learnData[indexes[ind]]);
                tempVectTar.push_back(learnTarData[indexes[ind]]);
            }
            learnDataSplit.push_back(tempVect);
            learnTarDataSplit.push_back(tempVectTar);
        }
        int validationIdx;
        int testIdx;
        for(int k=0; k<K; k++){ // K-fold, we shift groups by 1.
            // Create folder
            string folderName = std::string(folder)+"\\Execution"+std::to_string(n+1)+"\\Fold"+std::to_string(k+1);
            string folderNameLinux = std::string(folder)+"/Execution"+std::to_string(n+1)+"/Fold"+std::to_string(k+1)+"/";
            #ifdef __unix__
            mkdir(folderNameLinux.c_str(), 0777);
            #elif defined(_WIN32)
            mkdir(folderName.c_str());
            #endif
            // Get group index for test, validation and train
            vector<int> trainIdx;
            validationIdx = k;
            testIdx = (k+1)%K;
            for (int m=2; m<K; m++){
                trainIdx.push_back((k+m)%K);
            }
            //Creation of train, test and validation files (temp files)
            ofstream trainFile("tempTrain.txt");
            if(trainFile.fail()){
                throw std::runtime_error("Error : temp train file cound'nt open");
            }
            for (int  id : trainIdx){
                for(string line : learnDataSplit[id]){
                    trainFile << line << endl;
                }
            }
            trainFile.close();
            ofstream testFile("tempTest.txt");
            if(testFile.fail()){
                throw std::runtime_error("Error : temp test file cound'nt open");
            }
            for(string line : learnDataSplit[testIdx]){
                testFile << line << endl;
            }
            testFile.close();
            ofstream validFile("tempValid.txt");
            if(validFile.fail()){
                throw std::runtime_error("Error : temp valid file cound'nt open");
            }
            for(string line : learnDataSplit[validationIdx]){
                validFile << line << endl;
            }
            validFile.close();

            ofstream trainTarFile("tempTarTrain.txt");
            if(trainTarFile.fail()){
                throw std::runtime_error("Error : temp trainTar file cound'nt open");
            }
            for (int  id : trainIdx){
                for(string line : learnTarDataSplit[id]){
                    trainTarFile << line << endl;
                }
            }
            trainTarFile.close();

            ofstream testTarFile("tempTarTest.txt");
            if(testTarFile.fail()){
                throw std::runtime_error("Error : temp testTar file cound'nt open");
            }
            for(string line : learnTarDataSplit[testIdx]){
                testTarFile << line << endl;
            }
            testTarFile.close();

            ofstream validTarFile("tempTarValid.txt");
            if(validTarFile.fail()){
                throw std::runtime_error("Error : temp validTar file cound'nt open");
            }
            for(string line : learnTarDataSplit[validationIdx]){
                validTarFile << line << endl;
            }
            validTarFile.close();
            // Train datas with DimlpTrn
            string command = genericCommand;
            command += " -L tempTrain.txt ";
            command += "-T tempTest.txt ";
            command += "-V tempValid.txt ";
            command += "-1 tempTarTrain.txt ";
            command += "-2 tempTarTest.txt ";
            command += "-3 tempTarValid.txt ";


            #ifdef __unix__
            command += "-p " + std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/train.out "; // Output train pred file
            command += "-t " + std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/test.out "; // Output test pred file
            command += "-w " + std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/weights.wts "; // Output weight file
            #elif defined(_WIN32)
            command += "-p " + std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\train.out "; // Output train pred file
            command += "-t " + std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\test.out "; // Output test pred file
            command += "-w " + std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\weights.wts "; // Output weight file
            #endif
            command += "-r consoleTemp.txt"; // To not show console result

            int res = dimlpTrn(command);
            if (res == -1){
                return -1; // If there is an error in the Trn
            }

            #ifdef __unix__
            if (!copyFile("tempTrain.txt", (std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/train.txt").c_str())){
                cout << "File tempTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTest.txt", (std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/test.txt").c_str())){
                cout << "File tempTest.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTarTrain.txt", (std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/trainTarget.txt").c_str())){
                cout << "File tempTarTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTarTest.txt", (std::string(folder) + "/Execution" + std::to_string(n+1) + "/Fold" + std::to_string(k+1) + "/testTarget.txt").c_str())){
                cout << "File tempTarTest.txt coundn't be copied.\n";
                return -1;
            }
            #elif defined(_WIN32)
                        if (!copyFile("tempTrain.txt", (std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\train.txt").c_str())){
                cout << "File tempTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTest.txt", (std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\test.txt").c_str())){
                cout << "File tempTest.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTarTrain.txt", (std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\trainTarget.txt").c_str())){
                cout << "File tempTarTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile("tempTarTest.txt", (std::string(folder) + "\\Execution" + std::to_string(n+1) + "\\Fold" + std::to_string(k+1) + "\\testTarget.txt").c_str())){
                cout << "File tempTarTest.txt coundn't be copied.\n";
                return -1;
            }
            #endif
        }

    }

    remove("consoleTemp.txt");
    remove("tempTrain.txt");
    remove("tempTest.txt");
    remove("tempValid.txt");
    remove("tempTarTrain.txt");
    remove("tempTarTest.txt");
    remove("tempTarValid.txt");
    remove("dimlpValidation.out");
}
