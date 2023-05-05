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
#include "../../../hyperLocus/cpp/src/hyperLocusFct.h"
#include "../../../fidex/cpp/src/fidexFct.h"


void GiveAllParam()

{
   cout << "\n-------------------------------------------------\n\n";

   cout << "crossValid -S <Root folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
   cout << "-C <choose the algorithms by giving : fidex fidexGlo or both>";
   cout << "-L <training set file(path with respect to specified root folder)> -1 <file of train classes> ";
   cout << "-Q <number of stairs in staircase activation function> ";
   cout << "-h <high side of the interval> ";
   cout << "-I <number of input neurons> -O <number of output neurons>";

   cout << " <Options>\n\n";

   cout << "Options are: \n\n";
   cout << "-K <K-fold cross-validation>\n"; // Not to be 10
   cout << "-N <number of times we do the cross-validation>\n"; // Not to be 10
   cout << "-F <Folder where to save weights, predictions and datas after training>\n"; // Not to be CrossValidation
   cout << "-o <output statistic file name, saved in specific crossValidation folder>\n"; // Not to be crossValidationStats.txt
   cout << "-A <file of attributes>\n";
   cout << "-W <file of pretrained weights>\n";
   cout << "-H1 <number of neurons in the first hidden layer> ";
   cout << "(if not specified this number will be equal to the ";
   cout << "number of input neurons)\n";
   cout << "-Hk <number of neurons in the kth hidden layer>\n";
   cout << "-l <back-propagation learning parameter (Eta)>\n";
   cout << "-m <back-propagation momentum parameter (Mu)>\n";
   cout << "-f <back-propagation flat spot elimination parameter (Flat)>\n";
   cout << "-e <error threshold>\n";
   cout << "-a <accuracy threshold>\n";
   cout << "-d <absolute difference error threshold>\n";
   cout << "-i <number of train epochs>\n";
   cout << "-s <number of train epochs to show error>\n";
   cout << "-n <max fidex iteration number>\n";
   cout << "-v <minimum fidex covering number>\n";
   cout << "-x <dimension dropout parameter for fidex>\n";
   cout << "-y <hyperplan dropout parameter for fidex>\n";
   cout << "-z <seed (0=ranodom)>";

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

    float temps;
    clock_t t1, t2;

    t1 = clock();

    int k;

    DataSet  Train;
    DataSet  TrainClass;

    int N = 10; // Number of times we do cross-validation
    int K = 10; // Number of divisions of the dataset (10 = 8 for train, 1 for test and 1 for validation)
    int seed = 0;

    string learnTarTemp;
    bool learnTarInit = false;
    string learnFileTemp;
    bool learnFileInit = false;
    string folderTemp = "CrossValidation";
    string rootFolderTemp;
    bool rootFolderInit = false;
    string algorithm;
    bool algorithmInit = false;
    string statFile = "crossValidationStats.txt";
    bool isFidex = false;
    bool isFidexGlo = false;

    string genericCommand = "dimlpTrn";
    string hyperLocusGenericCommand = "hyperLocus -h 1";
    string fidexGenericCommand = "fidex";

    string eta, mu, flat, errThres, accThres, deltaErr, showErr, epochs, quant, hiKnot, nbIn, nbOut, arch, arch2, attrFile,  weightFile, itMax, minNbCover, dropoutDimParam, dropoutHypParam;
    bool quantInit = false;
    bool hiKnotInit = false;
    bool nbInInit = false;
    bool nbOutInit = false;
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

                case 'Q' :
                            quant = param[k];
                            quantInit = true;
                            genericCommand += " -q " + quant;
                            hyperLocusGenericCommand += " -Q " + quant;
                            break;

                case 'h' :
                            hiKnot = param[k];
                            hiKnotInit = true;
                            hyperLocusGenericCommand += " -I " + hiKnot;
                            break;

                case 'I' :
                           nbIn = param[k];
                           nbInInit = true;
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
                           nbOutInit = true;
                           genericCommand += " -O " + nbOut;
                           break;

                case 'S' :
                           rootFolderTemp = param[k];
                           rootFolderInit = true;
                           genericCommand += " -S " + rootFolderTemp;
                           fidexGenericCommand += " -R " + rootFolderTemp;
                           break;

                case 'A' :
                            attrFile = param[k];
                            genericCommand += " -A " + attrFile;
                            break;

                case 'W' :
                            weightFile = param[k];
                            genericCommand += " -W " + weightFile;
                            break;

                case 'L' :
                            learnFileTemp  = param[k];
                            learnFileInit = true;
                            break;

                case '1' :
                            learnTarTemp   = param[k];
                            learnTarInit = true;
                            break;

                case 'C' :
                            algorithm   = param[k];
                            algorithmInit = true;
                            break;

                case 'o' :
                            statFile   = param[k];
                            break;

                case 'K' : if (CheckInt(param[k]))
                              K = atoi(param[k]);
                           else return -1;

                           break;

                case 'N' : if (CheckInt(param[k]))
                              N = atoi(param[k]);
                           else return -1;

                           break;

                case 'n' :
                    itMax = param[k];
                    fidexGenericCommand += " -i " + itMax;
                    break;

                case 'v' :
                    minNbCover = param[k];
                    fidexGenericCommand += " -v " + minNbCover;
                    break;

                case 'x' :
                    dropoutDimParam = param[k];
                    fidexGenericCommand += " -d " + dropoutDimParam;
                    break;

                case 'y' :
                    dropoutHypParam = param[k];
                    fidexGenericCommand += " -h " + dropoutHypParam;
                    break;

                case 'z' : if (CheckInt(param[k])){
                                seed  = atoi(param[k]);
                                genericCommand += " -z " + std::to_string(seed);
                            }

                            else return -1;

                            break;

                case 'F' :
                           folderTemp  = param[k];
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
    if (rootFolderInit == false)
    {
        cout << "Give a root folder to save results with -S selection please." << "\n";
        return -1;
    }

    if (algorithmInit == false)
    {
        cout << "Give a algorithm with -C selection please. You can give fidex, fidexGlo or both." << "\n";
        return -1;
    }

    if (algorithm != "fidex" && algorithm != "fidexGlo" && algorithm != "both")
    {
        cout << "Give a correct algorithm with -C selection please. You can give fidex, fidexGlo or both." << "\n";
        return -1;
    }

    if (quantInit == false)
    {
        cout << "Give a number of stairs in staircase activation function with -Q selection please." << "\n";
        return -1;
    }

    if (hiKnotInit == false)
    {
        cout << "Give the high end of the interval for each dimension where an hyperplan can't be after with -h selection please." << "\n";
        return -1;
    }

    if (nbInInit == false)
    {
        cout << "Give the number of input neurons with option with -I selection please." << "\n";
        return -1;
    }

    if (nbOutInit == false)
    {
        cout << "Give the number of output neurons with option with -O selection please." << "\n";
        return -1;
    }



    // create paths with root foler
    char learnTarTmp[160], learnFileTmp[160], folderTmp[160];

    char* learnTar = 0;
    char* learnFile = 0;
    char* folder = 0;

    #ifdef __unix__
    string separator = "/";
    #elif defined(_WIN32)
    string separator = "\\";
    #endif

    string root = rootFolderTemp + separator;

    if (learnTarInit){
        learnTarTemp = root + learnTarTemp;
        if(learnTarTemp.length()>=160){
            cout << "Path " << learnTarTemp << "is too long" << "\n";
            return -1;
        }
        strcpy(learnTarTmp, learnTarTemp.c_str());
        learnTar = learnTarTmp;
    }

    if (learnFileInit){
        learnFileTemp = root + learnFileTemp;
        if(learnFileTemp.length()>=160){
            cout << "Path " << learnFileTemp << "is too long" << "\n";
            return -1;
        }
        strcpy(learnFileTmp, learnFileTemp.c_str());
        learnFile = learnFileTmp;
    }

    string folderTempFull = root + folderTemp;
    if(folderTempFull.length()>=160){
        cout << "Path " << folderTempFull << "is too long" << "\n";
        return -1;
    }
    strcpy(folderTmp, folderTempFull.c_str());
    folder = folderTmp;

    statFile = std::string(folder) + separator + statFile;


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

   if (learnFileInit == false)
   {
      cout << "Give the training file with -L selection please." << "\n";
      return -1;
   }

   if (learnTarInit == false)
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

    std::mt19937 g;
    std::random_device rd;
    std::vector<int> seeds;
    if (seed != 0){ // Not random
        for (int i = 0; i < 10*N; ++i)
            seeds.push_back(i);
        std::mt19937 g(seed);
        std::shuffle(seeds.begin(), seeds.end(), g);
    }



    // Create temp file strings for train, test and validation
    char trainFileStr[160];
    string trainFileStrTemp = root + "tempTrain.txt";
    strcpy(trainFileStr, trainFileStrTemp.c_str());

    char testFileStr[160];
    string testFileStrTemp = root + "tempTest.txt";
    strcpy(testFileStr, testFileStrTemp.c_str());

    char validFileStr[160];
    string validFileStrTemp = root + "tempValid.txt";
    strcpy(validFileStr, validFileStrTemp.c_str());

    char trainTarFileStr[160];
    string trainTarFileStrTemp = root + "tempTarTrain.txt";
    strcpy(trainTarFileStr, trainTarFileStrTemp.c_str());

    char testTarFileStr[160];
    string testTarFileStrTemp = root + "tempTarTest.txt";
    strcpy(testTarFileStr, testTarFileStrTemp.c_str());

    char validTarFileStr[160];
    string validTarFileStrTemp = root + "tempTarValid.txt";
    strcpy(validTarFileStr, validTarFileStrTemp.c_str());


    if (algorithm == "fidex" || algorithm == "both"){
        isFidex = true;
    }
    if (algorithm == "fidexGlo" || algorithm == "both"){
        isFidexGlo = true;
    }

    // Statistics for Fidex
    double meanCovSizeFid = 0;
    double meanNbAntFid = 0;
    double meanFidelFid = 0;
    double meanAccFid = 0;
    double meanConfidFid = 0;

    // Loop on N executions of cross-validation
    for (int n=0; n<N; n++){
        cout << "------------------------------" << endl;
        cout << "n=" << n << endl;

        // Create folder
        #ifdef __unix__
        string folderName = std::string(folder)+"/Execution"+std::to_string(n+1)+"/";
        mkdir(folderName.c_str(), 0777);
        #elif defined(_WIN32)
        string folderName = std::string(folder)+"\\Execution"+std::to_string(n+1);
        mkdir(folderName.c_str());
        #endif


        // Randomly split data in K sub-parts
        vector<vector<string>> learnDataSplit;
        vector<vector<string>> learnTarDataSplit;

        std::vector<int> indexes;
        for (int i = 0; i < nbSamples; ++i)
            indexes.push_back(i);
        if (seed != 0){ // Not random
            std::mt19937 g(seeds[n]);
            std::shuffle(indexes.begin(), indexes.end(), g);
        }
        else{ // random
            std::mt19937 g(rd());
            std::shuffle(indexes.begin(), indexes.end(), g);
        }

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
            cout << "----" << endl;
            cout << "k=" << k << endl;
            // path to folder
            string folderPath = std::string(folder) + separator + "Execution" + std::to_string(n+1) + separator + "Fold" + std::to_string(k+1);
            // Create folder
            #ifdef __unix__
            string folderName = folderPath + separator;
            mkdir(folderName.c_str(), 0777);
            #elif defined(_WIN32)
            string folderName = folderPath;
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
            ofstream trainFile(trainFileStr);
            if(trainFile.fail()){
                throw std::runtime_error("Error : temp train file cound'nt open");
            }
            for (int  id : trainIdx){
                for(string line : learnDataSplit[id]){
                    trainFile << line << endl;
                }
            }
            trainFile.close();

            ofstream testFile(testFileStr);
            if(testFile.fail()){
                throw std::runtime_error("Error : temp test file cound'nt open");
            }
            for(string line : learnDataSplit[testIdx]){
                testFile << line << endl;
            }
            testFile.close();

            ofstream validFile(validFileStr);
            if(validFile.fail()){
                throw std::runtime_error("Error : temp valid file cound'nt open");
            }
            for(string line : learnDataSplit[validationIdx]){
                validFile << line << endl;
            }
            validFile.close();

            ofstream trainTarFile(trainTarFileStr);
            if(trainTarFile.fail()){
                throw std::runtime_error("Error : temp trainTar file cound'nt open");
            }
            for (int  id : trainIdx){
                for(string line : learnTarDataSplit[id]){
                    trainTarFile << line << endl;
                }
            }
            trainTarFile.close();

            ofstream testTarFile(testTarFileStr);
            if(testTarFile.fail()){
                throw std::runtime_error("Error : temp testTar file cound'nt open");
            }
            for(string line : learnTarDataSplit[testIdx]){
                testTarFile << line << endl;
            }
            testTarFile.close();

            ofstream validTarFile(validTarFileStr);
            if(validTarFile.fail()){
                throw std::runtime_error("Error : temp validTar file cound'nt open");
            }
            for(string line : learnTarDataSplit[validationIdx]){
                validTarFile << line << endl;
            }
            validTarFile.close();


            // Get train, test and validation files in folder

            if (!copyFile(trainFileStr, (folderPath + separator + "train.txt").c_str())){
                cout << "File tempTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile(testFileStr, (folderPath + separator + "test.txt").c_str())){
                cout << "File tempTest.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile(validFileStr, (folderPath + separator + "valid.txt").c_str())){
                cout << "File tempValid.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile(trainTarFileStr, (folderPath + separator + "trainTarget.txt").c_str())){
                cout << "File tempTarTrain.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile(testTarFileStr, (folderPath + separator + "testTarget.txt").c_str())){
                cout << "File tempTarTest.txt coundn't be copied.\n";
                return -1;
            }
            if (!copyFile(validTarFileStr, (folderPath + separator + "validTarget.txt").c_str())){
                cout << "File tempTarValid.txt coundn't be copied.\n";
                return -1;
            }





            // Train datas with DimlpTrn
            string command = genericCommand;
            string folderPathFromRoot = std::string(folderTemp) + separator + "Execution" + std::to_string(n+1) + separator + "Fold" + std::to_string(k+1);

            command += " -L " + folderPathFromRoot + separator + "train.txt ";
            command += "-T " + folderPathFromRoot + separator + "test.txt ";
            command += "-V " + folderPathFromRoot + separator + "valid.txt ";
            command += "-1 " + folderPathFromRoot + separator + "trainTarget.txt ";
            command += "-2 " + folderPathFromRoot + separator + "testTarget.txt ";
            command += "-3 " + folderPathFromRoot + separator + "validTarget.txt ";

            command += "-p " + folderPathFromRoot + separator + "train.out "; // Output train pred file
            command += "-t " + folderPathFromRoot + separator + "test.out "; // Output test pred file
            command += "-v " + folderPathFromRoot + separator + "valid.out "; // Output validation pred file
            command += "-w " + folderPathFromRoot + separator + "weights.wts "; // Output weight file

            command += "-r consoleTemp.txt"; // To not show console result

            cout << "Enter in DimlpTrn function" << endl;
            int res = dimlpTrn(command);
            if (res == -1){
                return -1; // If there is an error in the Trn
            }



            // Compute hyperlocus in folder
            string hyperLocusCommand = hyperLocusGenericCommand;

            hyperLocusCommand += " -W " + folderPath + separator + "weights.wts ";
            hyperLocusCommand += "-O " + folderPath + separator + "hyperLocus.txt ";


            cout << "Enter in hyperlocus function" << endl;
            int resHyp = hyperLocus(hyperLocusCommand);
            if (resHyp == -1){
                return -1; // If there is an error in the hyperLocus
            }

            if (isFidex){
                // Compute fidex stats in folder
                string fidexCommand = fidexGenericCommand;

                fidexCommand += " -T " + folderPathFromRoot + separator + "train.txt ";
                fidexCommand += "-P " + folderPathFromRoot + separator + "train.out ";
                fidexCommand += "-C " + folderPathFromRoot + separator + "trainTarget.txt ";
                fidexCommand += "-S " + folderPathFromRoot + separator + "test.txt ";
                fidexCommand += "-p " + folderPathFromRoot + separator + "test.out ";
                fidexCommand += "-c " + folderPathFromRoot + separator + "testTarget.txt ";
                fidexCommand += "-H " + folderPathFromRoot + separator + "hyperLocus.txt ";
                fidexCommand += "-O " + folderPathFromRoot + separator + "fidexRule.txt ";
                fidexCommand += "-s " + folderPathFromRoot + separator + "fidexStats.txt ";
                fidexCommand += "-r " + folderPathFromRoot + separator + "fidexResult.txt ";

                cout << "Enter in fidex function" << endl;
                int resFid = fidex(fidexCommand);
                if (resFid == -1){
                    return -1; // If there is an error in fidex
                }

                // Get statistics from fidex
                string statsFile = "";
                statsFile = folderPath + separator + "fidexStats.txt";

                fstream statsData;
                statsData.open(statsFile,ios::in); //Read data file
                if(statsData.fail()){
                    throw std::runtime_error("Error : fidex stat file " + std::string(statsFile) + " not found");
                }
                string line;
                getline(statsData, line);
                getline(statsData, line);
                vector<double> statVals; //stat values
                while (! statsData.eof() ){
                    getline(statsData, line);
                    if (line.length()!=0){
                        std::string lastElement(line.substr(line.rfind(" ") + 1));
                        statVals.push_back(std::stod(lastElement));
                    }
                }
                statsData.close();

                meanCovSizeFid += statVals[0];
                meanNbAntFid += statVals[1];
                meanFidelFid += statVals[2];
                meanAccFid += statVals[3];
                meanConfidFid += statVals[4];
            }
        }

    }

    // Compute stats

    int nbExec = N*K;
    if (isFidex){
        meanCovSizeFid /= nbExec;
        meanNbAntFid /= nbExec;
        meanFidelFid /= nbExec;
        meanAccFid /= nbExec;
        meanConfidFid /= nbExec;
    }

    // Show and save results

    ofstream outputStatsFile (statFile);
    if(outputStatsFile.is_open()){

        outputStatsFile << "Results for " << N << " times " << K << "-Cross validation :\n\n";
        cout << "\n---------------------------------------------------------" << endl;
        cout << "---------------------------------------------------------" << endl << endl;
        cout << "Results for " << N << " times " << K << "-Cross validation :" << endl << endl;
        if (isFidex){
            outputStatsFile << "Fidex :\n";
            outputStatsFile << "The mean covering size per rule is : " << meanCovSizeFid << "\n";
            outputStatsFile << "The mean number of antecedents per rule is : " << meanNbAntFid << "\n";
            outputStatsFile << "The mean rule fidelity rate is : " << meanFidelFid << "\n";
            outputStatsFile << "The mean rule accuracy is : " << meanAccFid << "\n";
            outputStatsFile << "The mean rule confidence is : " << meanConfidFid << "\n";
            outputStatsFile << "\n---------------------------------------------------------\n";
            outputStatsFile << "---------------------------------------------------------\n";
            cout << "Fidex :" << endl;
            cout << "The mean covering size per rule is : " << meanCovSizeFid << endl;
            cout << "The mean number of antecedents per rule is : " << meanNbAntFid << endl;
            cout << "The mean rule fidelity rate is : " << meanFidelFid << endl;
            cout << "The mean rule accuracy is : " << meanAccFid << endl;
            cout << "The mean rule confidence is : " << meanConfidFid << endl;
            cout << "\n---------------------------------------------------------" << endl;
            cout << "---------------------------------------------------------" << endl;
        }
        if (isFidex && isFidexGlo){
            cout << endl;
        }
        if (isFidexGlo){
            cout << "FidexGlo :" << endl;
            cout << "The mean covering size per rule is : " << meanCovSizeFid << endl;
            cout << "The mean number of antecedents per rule is : " << meanNbAntFid << endl;
            cout << "The mean rule fidelity rate is : " << meanFidelFid << endl;
            cout << "The mean rule accuracy is : " << meanAccFid << endl;
            cout << "The mean rule confidence is : " << meanConfidFid << endl;
            cout << "\n---------------------------------------------------------" << endl;
            cout << "---------------------------------------------------------" << endl;
        }
    }
    else{
        throw std::runtime_error("Error : Couldn't open stats extraction file " + std::string(statFile) + ".");
    }


    char toDelete[160];
    string toDeleteTemp = root + "consoleTemp.txt";
    strcpy(toDelete, toDeleteTemp.c_str());
    remove(toDelete);

    t2 = clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    std::cout << "\nFull execution time = " << temps << " sec\n";



}

// .\CrossValid.exe -L datanorm -1 dataclass2 -K 3 -N 2 -Q 50 -I 16 -H2 5 -O 2 -F CrossValidation -S ../dimlp/datafiles -h 5 -n 100 -v 25 -x 0.5 -y 0.5 -C both
