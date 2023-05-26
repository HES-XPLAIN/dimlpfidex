#include "DimlpBTFct.h"

////////////////////////////////////////////////////////////

int main(int nbParam, char **param)

{
  std::string command = "dimlpBT";
  if (nbParam >= 1) {
    for (int p = 1; p < nbParam; p++) {
      command += " ";
      command += param[p];
    }
  }
  dimlpBT(command);
}
