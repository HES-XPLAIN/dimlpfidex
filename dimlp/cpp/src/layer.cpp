using namespace std;
#include <math.h>

#include "layer.h"
#include "randFun.h"

///////////////////////////////////////////////////////////////////

void Layer::AssignParam(float eta, float mu, float flat, int nbDown, int nbUp,
                        int nbWeights, int nbWeightsForInit)

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

void Layer::CreateStruct()

{
  Up = new float[NbUp];
  DeltaUp = new float[NbUp];
  Weights = new float[NbWeights];
  OldWeights = new float[NbWeights];
  ValidWeights = new float[NbWeights];
  BiasWeights = new float[NbUp];
  OldBiasWeights = new float[NbUp];
  ValidBiasWeights = new float[NbUp];
}

///////////////////////////////////////////////////////////////////

void Layer::InitWeights()

{
  int k;
  float *ptrW;
  float *oldW;
  const float sqrt3 = sqrt(3);

  float bound = sqrt3 / sqrt(NbWeightsForInit);
  FloatRandomFunction randVal(-bound, bound);
  //   FloatRandomFunction randVal(-1.0, 1.0);

  oldW = OldWeights;

  for (k = 0, ptrW = Weights; k < NbWeights; k++, ptrW++, oldW++) {
    *ptrW = randVal.RandomFloat();
    *oldW = *ptrW;
  }

  oldW = OldBiasWeights;

  for (k = 0, ptrW = BiasWeights; k < NbUp; k++, ptrW++, oldW++) {
    *ptrW = randVal.RandomFloat();
    *oldW = *ptrW;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::ApplyTransf1()

{
  float *up = Up;
  int n;

  for (n = 0; n < NbUp; n++, up++)
    *up = Activation1(*up);
  /*
  up = Up;
     for (n=0; n<NbUp; n++, up++)
         cout << *up << " ";
  cout << "\n";
  up = Up;
  */
}

///////////////////////////////////////////////////////////////////

void Layer::ApplyTransf2()

{
  float *up = Up;
  int n;

  for (n = 0; n < NbUp; n++, up++)
    *up = Activation2(*up);
}

///////////////////////////////////////////////////////////////////

void Layer::ComputeDeltaDownStand()

{
  int i, j;
  float sum;
  float *down;
  float *deltaDown;
  float *weights;
  float *deltaUp;

  down = Down;
  deltaDown = DeltaDown;

  for (i = 0; i < NbDown; i++, down++, deltaDown++) {
    weights = Weights + i;
    deltaUp = DeltaUp;

    for (j = 0, sum = 0.0; j < NbUp; j++, deltaUp++, weights += NbDown)
      sum += (*weights) * (*deltaUp);

    *deltaDown = ((*down) * (1.0 - *down) + Flat) * sum;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::ComputeDeltaDownSpec2()

{
  int i, j;
  float sum;
  float *down;
  float *deltaDown;
  float *weights;
  float *deltaUp;

  down = Down;
  deltaDown = DeltaDown;

  for (i = 0; i < NbDown; i++, down++, deltaDown++) {
    weights = Weights + i;
    deltaUp = DeltaUp + i;

    for (j = 0, sum = 0.0; j < NbUp;
         j += NbDown, deltaUp += NbDown, weights += NbDown)
      sum += (*weights) * (*deltaUp);

    *deltaDown = (*down) * sum;

    // cout << *deltaDown << " ";
  }
  // cout << "\n";
}

///////////////////////////////////////////////////////////////////

void Layer::AdaptBiasStand()

{
  int i;
  float biasDelta;

  float *bias = BiasWeights;
  float *oldBias = OldBiasWeights;
  float *deltaUp = DeltaUp;

  for (i = 0; i < NbUp; i++, bias++, oldBias++, deltaUp++) {
    biasDelta = (Eta * (*deltaUp)) + (Mu * ((*bias) - (*oldBias)));

    *oldBias = *bias;

    *bias += biasDelta;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::AdaptBiasSpec2()

{
  int i, j;
  float biasDelta;
  float norm;

  float *down = Down;
  float *up = Up;
  float *bias = BiasWeights;
  float *weights = Weights;
  float *oldBias = OldBiasWeights;
  float *deltaUp = DeltaUp;

  for (i = 0, j = 0; i < NbUp;
       i++, j++, bias++, weights++, oldBias++, deltaUp++, down++, up++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    norm =
        (*down - *weights) * (*down - *weights) / ((*bias) * (*bias) * (*bias));

    biasDelta = (EtaSpread * (*deltaUp) * norm); /* +
                (Mu * ((*bias) - (*oldBias)));

    *oldBias = *bias;
*/
    *bias += biasDelta;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::ReadWeights(istream &inFile)

{
  int w;
  float *ptrW;

  float *ptrOldW = OldBiasWeights;

  for (w = 0, ptrW = BiasWeights; w < NbUp; w++, ptrW++, ptrOldW++) {
    inFile >> *ptrW;
    *ptrOldW = *ptrW;
  }

  ptrOldW = OldWeights;

  for (w = 0, ptrW = Weights; w < NbWeights; w++, ptrW++, ptrOldW++) {
    inFile >> *ptrW;
    *ptrOldW = *ptrW;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::WriteWeights(ostream &outFile)

{
  int w;
  float *ptrW;

  for (w = 0, ptrW = BiasWeights; w < NbUp; w++, ptrW++)
    outFile << *ptrW << " ";
  outFile << "\n";

  for (w = 0, ptrW = Weights; w < NbWeights; w++, ptrW++)
    outFile << *ptrW << " ";
  outFile << "\n";
}

///////////////////////////////////////////////////////////////////

void Layer::PushWeights()

{
  int w;

  for (w = 0; w < NbUp; w++)
    ValidBiasWeights[w] = BiasWeights[w];

  for (w = 0; w < NbWeights; w++)
    ValidWeights[w] = Weights[w];
}

///////////////////////////////////////////////////////////////////

void Layer::PopWeights()

{
  int w;

  for (w = 0; w < NbUp; w++)
    BiasWeights[w] = ValidBiasWeights[w];

  for (w = 0; w < NbWeights; w++)
    Weights[w] = ValidWeights[w];
}

///////////////////////////////////////////////////////////////////

void Layer::ForwFully()

{
  int i, j;
  float *down;

  float *up = Up;
  float *weights = Weights;
  float *bias = BiasWeights;

  for (i = 0; i < NbUp; i++, up++, bias++) {
    down = Down;
    *up = *bias;

    for (j = 0; j < NbDown; j++, down++, weights++)
      *up += (*down) * (*weights);
    // cout << *up << " ";
  }
  // cout << "\n";
}

///////////////////////////////////////////////////////////////////

void Layer::ForwSpec()

{
  int i, j;

  float *up = Up;
  float *down = Down;
  float *weights = Weights;
  float *bias = BiasWeights;

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

void Layer::ForwSpec2()

{
  int i, j;

  float *up = Up;
  float *down = Down;
  float *weights = Weights;
  float *bias = BiasWeights;

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

void Layer::ForwRadial()

{
  int i, j;
  float *down;

  float *up = Up;
  float *weights = Weights;
  float *bias = BiasWeights;

  for (i = 0; i < NbUp; i++, up++, bias++) {
    down = Down;
    *up = 0.0;

    for (j = 0; j < NbDown; j++, down++, weights++)
      *up += (((*down) - (*weights)) * ((*down) - (*weights)));

    *up = sqrt(*up);
    //       *bias = sqrt(*bias);
    *up = *up * *bias;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::ComputeDeltaOut(float target[])

{
  int i;

  float *out = Up;
  float *deltaOut = DeltaUp;

  for (i = 0; i < NbUp; i++, out++, deltaOut++, target++)
    *deltaOut = ((*out) * (1.0 - *out) + Flat) * (*target - *out);
}

///////////////////////////////////////////////////////////////////

void Layer::AdaptWeightsFully()

{
  int i, j;
  float weightDelta;

  float *down = Down;
  float *deltaUp = DeltaUp;
  float *weights = Weights;
  float *oldWeights = OldWeights;

  for (i = 0; i < NbUp; i++, deltaUp++) {
    for (j = 0, down = Down; j < NbDown; j++, down++, weights++, oldWeights++) {
      weightDelta =
          (Eta * (*deltaUp) * (*down)) + (Mu * ((*weights) - (*oldWeights)));

      *oldWeights = *weights;

      *weights += weightDelta;
    }
  }
}

///////////////////////////////////////////////////////////////////

void Layer::AdaptWeightsSpec()

{
  int i, j;
  float weightDelta;

  float *down = Down;
  float *deltaUp = DeltaUp;
  float *weights = Weights;
  float *oldWeights = OldWeights;

  for (i = 0, j = 0; i < NbUp;
       i++, j++, down++, weights++, oldWeights++, deltaUp++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    weightDelta =
        (Eta * (*deltaUp) * (*down)) + (Mu * ((*weights) - (*oldWeights)));

    *oldWeights = *weights;

    *weights += weightDelta;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::AdaptWeightsSpec2()

{
  int i, j;
  float weightDelta, norm;

  float *down = Down;
  float *up = Up;
  float *deltaUp = DeltaUp;
  float *bias = BiasWeights;
  float *weights = Weights;
  float *oldWeights = OldWeights;

  for (i = 0, j = 0; i < NbUp;
       i++, j++, down++, up++, weights++, bias++, oldWeights++, deltaUp++) {
    if (j == NbDown) {
      down = Down;
      j = 0;
    }

    norm = (*down - *weights) / ((*bias) * (*bias));

    weightDelta = (EtaCentre * (*deltaUp) * norm); /*+
                  (Mu * ((*weights) - (*oldWeights)));

    *oldWeights = *weights;
*/
    *weights += weightDelta;
  }
}

///////////////////////////////////////////////////////////////////

void Layer::Del()

{
  delete Up;
  delete DeltaUp;

  delete Weights;
  delete OldWeights;
  delete ValidWeights;

  delete BiasWeights;
  delete OldBiasWeights;
  delete ValidBiasWeights;
}

///////////////////////////////////////////////////////////////////

Layer::Layer(float eta, float mu, float flat, int nbDown, int nbUp,
             int nbWeights, int nbWeightsForInit)

{
  AssignParam(eta, mu, flat, nbDown, nbUp, nbWeights, nbWeightsForInit);
  CreateStruct();
  InitWeights();
}

///////////////////////////////////////////////////////////////////
