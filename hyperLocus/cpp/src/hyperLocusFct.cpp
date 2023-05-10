#include "hyperLocusFct.h"

void showParams() {
  cout << "\n-------------------------------------------------\n\n";

  cout << "Obligatory parameters : \n\n";
  cout << "hyperLocus -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
  cout << "-W <file of weights> ";
  cout << "-Q <number of stairs in staircase activation function> ";
  cout << "-I <high side of the interval>";

  cout << " <Options>\n\n";

  cout << "Options are: \n\n";
  cout << "-O <output file>\n";
  cout << "-h <1 to hide notes on terminal>\n";

  cout << "\n-------------------------------------------------\n\n";
}

vector<vector<double>> calcHypLocus(int nbQuantLevels, double lowKnot, double hiKnot, vector<double> vectBias, vector<double> vectWeights) {

  int nbIn = vectBias.size();      // Number of neurons in the first hidden layer (May be the number of input variables)
  int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

  double dist = hiKnot - lowKnot;         // Size of the interval
  double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

  vector<vector<double>> matHypLocus(nbIn, vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
  std::vector<double> knots(nbKnots);                                // Vector of location of the separations for one dimension (hyperplans will be placed close)

  for (int k = 0; k < nbKnots; k++) {
    knots[k] = lowKnot + (binWidth * k); // location of each separation within a dimension (hyperplans will be placed close)
  }

  for (int i = 0; i < nbIn; i++) { // Loop on dimension
    for (int j = 0; j < nbKnots; j++) {
      matHypLocus[i][j] = (knots[j] - vectBias[i]) / vectWeights[i]; // Placement of the hyperplan
    }
  }

  return matHypLocus;
}

int hyperLocus(string command) {
  try {

    float temps;
    clock_t t1, t2;

    t1 = clock();

    // Parsing the command
    vector<string> commandList;
    const char delim = ' ';
    string s;
    stringstream ss(command);
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    int nbParam = commandList.size();

    // Parameters declaration

    string dataFileWeightsTemp; // Weights obtained with dimlp
    bool dataFileWeightsInit = false;
    string outputFile = "hyperLocus.txt";
    string rootFolderTemp;
    bool rootFolderInit = false;

    int nbQuantLevels; // Number of steps of the step function
    bool nbQuantLevelsInit = false;
    double hiKnot; // High end of the interval for each dimension, a hyperplan can't be after
    bool hiKnotInit = false;
    bool hideNotes = false;

    vector<vector<double>> hyperLocus;

    // Import parameters

    if (nbParam == 1) {
      showParams();
      return -1;
    }

    for (int p = 1; p < nbParam; p++) { // We skip "fidex"
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          cout << "Missing something at the end of the command.\n";
          return -1;
        }

        switch (commandList[p - 1][1]) { // Get letter after the -

        case 'W':
          dataFileWeightsTemp = &(commandList[p])[0];
          dataFileWeightsInit = true;
          break;

        case 'Q':
          if (CheckPositiveInt(&(commandList[p])[0])) {
            nbQuantLevels = atoi(&(commandList[p])[0]);
            nbQuantLevelsInit = true;
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", positive integer requested");
          }
          break;

        case 'I':
          if (CheckFloatFid(&(commandList[p])[0]) && atof(&(commandList[p])[0]) > 0) {
            hiKnot = atof(&(commandList[p])[0]);
            hiKnotInit = true;
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", strictly positive float requested");
          }
          break;

        case 'O':
          outputFile = &(commandList[p])[0];
          break;

        case 'h':
          if (CheckPositiveInt(&(commandList[p])[0])) {
            hideNotes = atoi(&(commandList[p])[0]);
          } else {
            throw std::runtime_error("Error : invalide type for parameter " + string(&(commandList[p - 1])[0]) + ", positive integer requested");
          }
          break;

        case 'S':
          rootFolderTemp = &(commandList[p])[0];
          rootFolderInit = true;
          break;

        default: // If we put another -X option
          throw std::runtime_error("Illegal option : " + string(&(commandList[p - 1])[0]));
        }
      }
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    char dataFileWeightsTmp[160];

    char *dataFileWeights = 0;

#ifdef __unix__
    string root = rootFolderTemp + "/";
#elif defined(_WIN32)
    string root = rootFolderTemp + "\\";
#endif

    if (dataFileWeightsInit) {
      dataFileWeightsTemp = root + dataFileWeightsTemp;
      if (dataFileWeightsTemp.length() >= 160) {
        cout << "Path " << dataFileWeightsTemp << "is too long"
             << "\n";
        return -1;
      }
      strcpy(dataFileWeightsTmp, dataFileWeightsTemp.c_str());
      dataFileWeights = dataFileWeightsTmp;
    }

    outputFile = root + outputFile;

    // ----------------------------------------------------------------------

    if (!dataFileWeightsInit) {
      throw std::runtime_error("The weight file has to be given with option -W");
    }
    if (!nbQuantLevelsInit) {
      throw std::runtime_error("The number of steps in the step function has to be given with option -Q");
    }
    if (!hiKnotInit) {
      throw std::runtime_error("The right end of the interval in which hyperplans are contained has to be given with option -I");
    }

    double lowKnot = -hiKnot;

    if (!hideNotes) {
      cout << "\nParameters :\n\n";
      cout << "- Number of stairs " << nbQuantLevels << endl;
      cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << endl
           << endl;

      cout << "Import weight file..." << endl;
    }
    DataSetFid weightDatas(dataFileWeights);

    vector<vector<double>> allWeights = weightDatas.getWeights();
    vector<double> biais = weightDatas.getInBiais();
    vector<double> weights = weightDatas.getInWeights();

    if (!hideNotes) {

      cout << "Weight file imported" << endl
           << endl;

      cout << "computation of hyperLocus" << endl;
    }
    hyperLocus = calcHypLocus(nbQuantLevels, lowKnot, hiKnot, biais, weights);
    if (!hideNotes) {
      cout << "HyperLocus computed" << endl
           << endl;

      cout << "Extraction of the hyperLocus" << endl;
    }

    ofstream hyperLocusFile(outputFile);
    if (hyperLocusFile.is_open()) {
      for (int i = 0; i < hyperLocus.size(); i++) {
        for (int j = 0; j < hyperLocus[0].size(); j++) {
          hyperLocusFile << hyperLocus[i][j] << " ";
        }
        hyperLocusFile << "\n";
      }
      hyperLocusFile.close();
    } else {
      throw std::runtime_error("Error : Couldn't open file " + std::string(outputFile) + ".");
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    if (!hideNotes) {
      printf("\nTime to compute the hyperLocus = %f sec\n", temps);
    }
  }

  catch (const char *msg) {
    printf(msg);
    cerr << msg << endl;
    return -1;
  }

  return 0;
}

// Exemple : hyperLocus -W dimlp.wts -Q 50 -I 5 -O hyperLocus -S ../fidexGlo/datafiles/
// .\hyperLocus.exe -W dimlp.wts -Q 50 -I 5 -O hyperLocus -S ../fidexGlo/datafiles/

// .\hyperLocus.exe -W covid.wts -Q 50 -I 5 -O hyperLocusCovid -S ../dimlp/datafiles/covidDataset
// .\hyperLocus.exe -W spam.wts -Q 50 -I 5 -O hyperLocusSpam -S ../dimlp/datafiles/spamDataset
