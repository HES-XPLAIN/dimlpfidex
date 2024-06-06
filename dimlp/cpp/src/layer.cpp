#include "layer.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief Assigns parameters for the layer.
 *
 * @param eta Learning rate.
 * @param mu Momentum factor.
 * @param flat Flatness factor.
 * @param nbDown Number of neurons in the previous layer.
 * @param nbUp Number of neurons in this layer.
 * @param nbWeights Number of weights in this layer.
 * @param nbWeightsForInit Number of weights for initialization.
 */
void Layer::AssignParam(
    float eta,
    float mu,
    float flat,
    int nbDown,
    int nbUp,
    int nbWeights,
    int nbWeightsForInit)

{
  Eta = eta;
  Mu = mu;
  Flat = flat;
  NbDown = nbDown;
  NbUp = nbUp;
  NbWeights = nbWeights;
  NbWeightsForInit = nbWeightsForInit;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Creates the data structures for the layer.
 */
void Layer::CreateStruct()

{
  Up.resize(NbUp);
  DeltaUp.resize(NbUp);
  Weights.resize(NbWeights);
  OldWeights.resize(NbWeights);
  ValidWeights.resize(NbWeights);
  BiasWeights.resize(NbUp);
  OldBiasWeights.resize(NbUp);
  ValidBiasWeights.resize(NbUp);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the weights of the layer.
 */
void Layer::InitWeights()

{
  int k;
  float *ptrW;
  float *oldW;
  const auto sqrt3 = static_cast<float>(sqrt(3));

  float bound = sqrt3 / static_cast<float>(sqrt(NbWeightsForInit));
  FloatRandomFunction randVal(-bound, bound);

  oldW = OldWeights.data();

  for (k = 0, ptrW = Weights.data(); k < NbWeights; k++, ptrW++, oldW++) {
    *ptrW = randVal.RandomFloat();
    *oldW = *ptrW;
  }

  oldW = OldBiasWeights.data();

  for (k = 0, ptrW = BiasWeights.data(); k < NbUp; k++, ptrW++, oldW++) {
    *ptrW = randVal.RandomFloat();
    *oldW = *ptrW;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Applies the first activation function to the layer's outputs.
 */
void Layer::ApplyTransf1() {
  for (float &value : Up) {
    value = Activation1(value);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Applies the second activation function to the layer's outputs.
 */
void Layer::ApplyTransf2()

{
  for (float &value : Up) {
    value = Activation2(value);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the delta values for the input neurons using the standard method.
 */
void Layer::ComputeDeltaDownStand()

{
  int j;
  float sum;
  const float *down;
  float *deltaDown;
  const float *weights;
  const float *deltaUp;

  down = Down;
  deltaDown = DeltaDown;

  for (int i = 0; i < NbDown; i++, down++, deltaDown++) {
    weights = Weights.data() + i;
    deltaUp = DeltaUp.data();

    for (j = 0, sum = 0.0; j < NbUp; j++, deltaUp++, weights += NbDown)
      sum += (*weights) * (*deltaUp);

    *deltaDown = ((*down) * (1.0f - *down) + Flat) * sum;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the delta values for the input neurons using a specific method.
 */
void Layer::ComputeDeltaDownSpec2()

{
  int j;
  float sum;
  const float *down;
  float *deltaDown;
  const float *weights;
  const float *deltaUp;

  down = Down;
  deltaDown = DeltaDown;

  for (int i = 0; i < NbDown; i++, down++, deltaDown++) {
    weights = Weights.data() + i;
    deltaUp = DeltaUp.data() + i;

    for (j = 0, sum = 0.0; j < NbUp; j += NbDown, deltaUp += NbDown, weights += NbDown)
      sum += (*weights) * (*deltaUp);

    *deltaDown = (*down) * sum;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Updates the bias weights using the standard method.
 */
void Layer::AdaptBiasStand()

{
  float biasDelta;

  float *bias = BiasWeights.data();
  float *oldBias = OldBiasWeights.data();
  const float *deltaUp = DeltaUp.data();

  for (int i = 0; i < NbUp; i++, bias++, oldBias++, deltaUp++) {
    biasDelta = (Eta * (*deltaUp)) + (Mu * ((*bias) - (*oldBias)));

    *oldBias = *bias;

    *bias += biasDelta;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Updates the bias weights using a specific method.
 */
void Layer::AdaptBiasSpec2()

{
  int i;
  int j;
  float biasDelta;
  float norm;

  const float *down = Down;
  const float *up = Up.data();
  float *bias = BiasWeights.data();
  const float *weights = Weights.data();
  const float *oldBias = OldBiasWeights.data();
  const float *deltaUp = DeltaUp.data();

  for (i = 0, j = 0; i < NbUp; i++, j++, bias++, weights++, oldBias++, deltaUp++, down++, up++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    norm = (*down - *weights) * (*down - *weights) /
           ((*bias) * (*bias) * (*bias));

    biasDelta = (EtaSpread * (*deltaUp) * norm);
    *bias += biasDelta;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Reads the weights from a file.
 * @param inFile Input file stream.
 */
void Layer::ReadWeights(std::istream &inFile)

{
  int w;
  float *ptrW;

  float *ptrOldW = OldBiasWeights.data();

  for (w = 0, ptrW = BiasWeights.data(); w < NbUp; w++, ptrW++, ptrOldW++) {
    inFile >> *ptrW;
    *ptrOldW = *ptrW;
  }

  ptrOldW = OldWeights.data();
  for (w = 0, ptrW = Weights.data(); w < NbWeights; w++, ptrW++, ptrOldW++) {
    inFile >> *ptrW;
    *ptrOldW = *ptrW;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Writes the weights to a file.
 * @param outFile Output file stream.
 */
void Layer::WriteWeights(std::ostream &outFile)

{
  int w;
  const float *ptrW;

  for (w = 0, ptrW = BiasWeights.data(); w < NbUp; w++, ptrW++)
    outFile << *ptrW << " ";
  outFile << "" << std::endl;

  for (w = 0, ptrW = Weights.data(); w < NbWeights; w++, ptrW++)
    outFile << *ptrW << " ";
  outFile << "" << std::endl;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Pushes the current weights to the validated weights.
 */
void Layer::PushWeights()

{
  int w;

  for (w = 0; w < NbUp; w++)
    ValidBiasWeights[w] = BiasWeights[w];

  for (w = 0; w < NbWeights; w++)
    ValidWeights[w] = Weights[w];
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Pops the validated weights to the current weights.
 */
void Layer::PopWeights()

{
  int w;

  for (w = 0; w < NbUp; w++)
    BiasWeights[w] = ValidBiasWeights[w];

  for (w = 0; w < NbWeights; w++)
    Weights[w] = ValidWeights[w];
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward propagation for a fully connected layer.
 */
void Layer::ForwFully()

{
  const float *down;

  float *up = Up.data();
  const float *weights = Weights.data();
  const float *bias = BiasWeights.data();

  for (int i = 0; i < NbUp; i++, up++, bias++) {
    down = Down;
    *up = *bias;

    for (int j = 0; j < NbDown; j++, down++, weights++)
      *up += (*down) * (*weights);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward propagation using a specific method.
 */
void Layer::ForwSpec()

{
  int i;
  int j;

  float *up = Up.data();
  const float *down = Down;
  const float *weights = Weights.data();
  const float *bias = BiasWeights.data();
  for (i = 0, j = 0; i < NbUp; i++, j++, down++, weights++, up++, bias++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }
    *up = *bias;
    *up += (*down) * (*weights);
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward propagation using a second specific method.
 */
void Layer::ForwSpec2()

{
  int i;
  int j;

  float *up = Up.data();
  const float *down = Down;
  const float *weights = Weights.data();
  const float *bias = BiasWeights.data();

  for (i = 0, j = 0; i < NbUp; i++, j++, down++, weights++, up++, bias++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    *up = (*down) - (*weights);
    *up = *up / *bias;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Performs forward propagation for radial basis functions.
 */
void Layer::ForwRadial()

{
  const float *down;

  float *up = Up.data();
  const float *weights = Weights.data();
  const float *bias = BiasWeights.data();

  for (int i = 0; i < NbUp; i++, up++, bias++) {
    down = Down;
    *up = 0.0;

    for (int j = 0; j < NbDown; j++, down++, weights++)
      *up += (((*down) - (*weights)) * ((*down) - (*weights)));

    *up = sqrt(*up);
    *up = *up * *bias;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the delta values for the output neurons.
 * @param target Target values.
 */
void Layer::ComputeDeltaOut(const float target[])

{

  const float *out = Up.data();
  float *deltaOut = DeltaUp.data();

  for (int i = 0; i < NbUp; i++, out++, deltaOut++, target++)
    *deltaOut = ((*out) * (1.0f - *out) + Flat) * (*target - *out);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Updates the weights for a fully connected layer.
 */
void Layer::AdaptWeightsFully()

{
  int j;
  float weightDelta;

  const float *down;
  const float *deltaUp = DeltaUp.data();
  float *weights = Weights.data();
  float *oldWeights = OldWeights.data();

  for (int i = 0; i < NbUp; i++, deltaUp++) {
    for (j = 0, down = Down; j < NbDown; j++, down++, weights++, oldWeights++) {
      weightDelta = (Eta * (*deltaUp) * (*down)) +
                    (Mu * ((*weights) - (*oldWeights)));

      *oldWeights = *weights;

      *weights += weightDelta;
    }
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Updates the weights using a specific method.
 */
void Layer::AdaptWeightsSpec()

{
  int i;
  int j;
  float weightDelta;

  const float *down = Down;
  const float *deltaUp = DeltaUp.data();
  float *weights = Weights.data();
  float *oldWeights = OldWeights.data();

  for (i = 0, j = 0; i < NbUp; i++, j++, down++, weights++, oldWeights++, deltaUp++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    weightDelta = (Eta * (*deltaUp) * (*down)) +
                  (Mu * ((*weights) - (*oldWeights)));

    *oldWeights = *weights;

    *weights += weightDelta;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Updates the weights using a second specific method.
 */
void Layer::AdaptWeightsSpec2()

{
  int i;
  int j;
  float weightDelta;
  float norm;

  const float *down = Down;
  const float *up = Up.data();
  const float *deltaUp = DeltaUp.data();
  const float *bias = BiasWeights.data();
  float *weights = Weights.data();
  const float *oldWeights = OldWeights.data();

  for (i = 0, j = 0; i < NbUp; i++, j++, down++, up++, weights++, bias++, oldWeights++, deltaUp++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    norm = (*down - *weights) / ((*bias) * (*bias));

    weightDelta = (EtaCentre * (*deltaUp) * norm);
    *weights += weightDelta;
  }
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for the Layer class.
 * @param eta Learning rate.
 * @param mu Momentum factor.
 * @param flat Flatness factor.
 * @param nbDown Number of neurons in the previous layer.
 * @param nbUp Number of neurons in this layer.
 * @param nbWeights Number of weights in this layer.
 * @param nbWeightsForInit Number of weights for initialization.
 */
Layer::Layer(
    float eta,
    float mu,
    float flat,
    int nbDown,
    int nbUp,
    int nbWeights,
    int nbWeightsForInit)

{
  AssignParam(eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit);
  CreateStruct();
  InitWeights();
}

///////////////////////////////////////////////////////////////////
