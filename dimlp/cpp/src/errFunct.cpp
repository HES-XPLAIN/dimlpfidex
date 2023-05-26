#include <vector>
///////////////////////////////////////////////////////////////////

float Lmse(int nbTar, std::vector<float> netOut, std::vector<float> target)

{
  float diff;
  float sum = 0.0;

  for (int i = 0; i < nbTar; i++) {
    diff = target[i] - netOut[i];
    sum += diff * diff;
  }

  return sum;
}

///////////////////////////////////////////////////////////////////
