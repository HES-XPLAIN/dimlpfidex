#include <vector>
///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the least mean squared error (LMSE) between network output and target values.
 *
 * This function calculates the LMSE by summing the squared differences between the target values
 * and the network's output values.
 *
 * @param nbTar The number of target values.
 * @param netOut A vector containing the network's output values.
 * @param target A vector containing the target values.
 * @return The computed LMSE.
 */
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
