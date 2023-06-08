#include "hyperLocusFct.h"

void showParams() {
  std::cout << "\n-------------------------------------------------\n\n";

  std::cout << "Obligatory parameters : \n\n";
  std::cout << "hyperLocus -S <Folder where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>";
  std::cout << "-W <file of weights> ";
  std::cout << "-Q <number of stairs in staircase activation function> ";
  std::cout << "-I <high side of the interval>";

  std::cout << " <Options>\n\n";

  std::cout << "Options are: \n\n";
  std::cout << "-O <output file>\n";
  std::cout << "-h <1 to hide notes on terminal>\n";

  std::cout << "\n-------------------------------------------------\n\n";
}

std::vector<std::vector<double>> calcHypLocus(int nbQuantLevels, double lowKnot, double hiKnot, std::vector<double> vectBias, std::vector<double> vectWeights) {

  size_t nbIn = vectBias.size();   // Number of neurons in the first hidden layer (May be the number of input variables)
  int nbKnots = nbQuantLevels + 1; // Number of separations per dimension

  double dist = hiKnot - lowKnot;         // Size of the interval
  double binWidth = dist / nbQuantLevels; // Width of a box between 2 separations

  std::vector<std::vector<double>> matHypLocus(nbIn, std::vector<double>(nbKnots)); // Matrix of hyperplans (dim x hyp)
  std::vector<double> knots(nbKnots);                                               // std::vector of location of the separations for one dimension (hyperplans will be placed close)

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

int hyperLocus(const std::string &command) {
  try {

    float temps;
    clock_t t1;
    clock_t t2;

    t1 = clock();

    // Parsing the command
    std::vector<std::string> commandList;
    const char delim = ' ';
    std::string s;
    std::stringstream ss(command);
    while (std::getline(ss, s, delim)) {
      commandList.push_back(s);
    }
    size_t nbParam = commandList.size();

    // Parameters declaration

    std::string dataFileWeightsTemp; // Weights obtained with dimlp
    bool dataFileWeightsInit = false;
    std::string outputFile = "hyperLocus.txt";
    std::string rootFolderTemp;

    int nbQuantLevels; // Number of steps of the step function
    bool nbQuantLevelsInit = false;
    double hiKnot; // High end of the interval for each dimension, a hyperplan can't be after
    bool hiKnotInit = false;
    bool hideNotes = false;

    std::vector<std::vector<double>> hyperLocus;

    // Import parameters

    if (nbParam <= 1) {
      showParams();
      return 0;
    }

    int p = 1; // We skip "fidex"
    while (p < nbParam) {
      if (commandList[p][0] == '-') {
        p++;

        if (p >= nbParam) {
          throw CommandArgumentException("Missing something at the end of the command.");
        }

        char option = commandList[p - 1][1];
        const char *arg = &(commandList[p])[0];
        const char *lastArg = &(commandList[p - 1])[0];

        switch (option) { // Get letter after the -

        case 'W':
          dataFileWeightsTemp = arg;
          dataFileWeightsInit = true;
          break;

        case 'Q':
          if (CheckPositiveInt(arg)) {
            nbQuantLevels = atoi(arg);
            nbQuantLevelsInit = true;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", positive integer requested");
          }
          break;

        case 'I':
          if (CheckFloatFid(arg) && atof(arg) > 0) {
            hiKnot = atof(arg);
            hiKnotInit = true;
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", strictly positive float requested");
          }
          break;

        case 'O':
          outputFile = arg;
          break;

        case 'h':
          if (CheckPositiveInt(arg)) {
            hideNotes = atoi(arg);
          } else {
            throw CommandArgumentException("Error : invalide type for parameter " + std::string(lastArg) + ", positive integer requested");
          }
          break;

        case 'S':
          rootFolderTemp = arg;
          break;

        default: // If we put another -X option
          throw CommandArgumentException("Illegal option : " + std::string(lastArg));
        }
      }

      p++;
    }

    // ----------------------------------------------------------------------
    // create paths with root foler

    const char *dataFileWeights = nullptr;

#if defined(__unix__) || defined(__APPLE__)
    std::string root = rootFolderTemp + "/";
#elif defined(_WIN32)
    std::string root = rootFolderTemp + "\\";
#endif

    if (dataFileWeightsInit) {
      dataFileWeightsTemp = root + dataFileWeightsTemp;
      dataFileWeights = &dataFileWeightsTemp[0];
    }

    outputFile = root + outputFile;

    // ----------------------------------------------------------------------

    if (!dataFileWeightsInit) {
      throw CommandArgumentException("The weight file has to be given with option -W");
    }
    if (!nbQuantLevelsInit) {
      throw CommandArgumentException("The number of steps in the step function has to be given with option -Q");
    }
    if (!hiKnotInit) {
      throw CommandArgumentException("The right end of the interval in which hyperplans are contained has to be given with option -I");
    }

    double lowKnot = -hiKnot;

    if (!hideNotes) {
      std::cout << "\nParameters :\n\n";
      std::cout << "- Number of stairs " << nbQuantLevels << std::endl;
      std::cout << "- Interval : [" << lowKnot << "," << hiKnot << "]" << std::endl
                << std::endl;

      std::cout << "Import weight file..." << std::endl;
    }
    DataSetFid weightDatas(dataFileWeights);

    std::vector<std::vector<double>> allWeights = weightDatas.getWeights();
    std::vector<double> biais = weightDatas.getInBiais();
    std::vector<double> weights = weightDatas.getInWeights();

    if (!hideNotes) {

      std::cout << "Weight file imported" << std::endl
                << std::endl;

      std::cout << "computation of hyperLocus" << std::endl;
    }
    hyperLocus = calcHypLocus(nbQuantLevels, lowKnot, hiKnot, biais, weights);
    if (!hideNotes) {
      std::cout << "HyperLocus computed" << std::endl
                << std::endl;

      std::cout << "Extraction of the hyperLocus" << std::endl;
    }

    std::ofstream hyperLocusFile(outputFile);
    if (hyperLocusFile.is_open()) {
      for (const auto &row : hyperLocus) {
        for (const auto &element : row) {
          hyperLocusFile << element << " ";
        }
        hyperLocusFile << "\n";
      }
      hyperLocusFile.close();
    } else {
      throw CannotOpenFileError("Error : Couldn't open file " + std::string(outputFile) + ".");
    }

    t2 = clock();
    temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
    if (!hideNotes) {
      printf("\nTime to compute the hyperLocus = %f sec\n", temps);
    }
  }

  catch (const char *msg) {
    std::cerr << msg << std::endl;
    return -1;
  }

  return 0;
}

/* Exemples pour lancer le code :

  .\hyperLocus.exe -W dimlp.wts -Q 50 -I 5 -O hyperLocus -S ../fidexGlo/datafiles/

  .\hyperLocus.exe -W covid.wts -Q 50 -I 5 -O hyperLocusCovid -S ../dimlp/datafiles/covidDataset
  .\hyperLocus.exe -W spam.wts -Q 50 -I 5 -O hyperLocusSpam -S ../dimlp/datafiles/spamDataset
  .\hyperLocus.exe -W isoletV2.wts -Q 50 -I 5 -O hyperLocusIsoletV2 -S ../dimlp/datafiles/isoletDataset
  .\hyperLocus.exe -W HAPTV2.wts -Q 50 -I 5 -O hyperLocusHAPTV2 -S ../dimlp/datafiles/HAPTDataset

*/
