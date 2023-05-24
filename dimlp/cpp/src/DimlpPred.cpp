#include "DimlpPredFct.h"

////////////////////////////////////////////////////////////

int main(int nbParam, char **param)

{
  std::string command = "dimlpPred";
  if (nbParam >= 1) {
    for (int p = 1; p < nbParam; p++) {
      command += " ";
      command += param[p];
    }
  }
  dimlpPred(command);
}
