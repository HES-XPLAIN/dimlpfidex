using namespace std;
#include <math.h>

///////////////////////////////////////////////////////////////////

float Sigmoid(float x)

{
  return 1.0 / (1.0 + exp(-x));
}

///////////////////////////////////////////////////////////////////

float HyperTang(float x)

{
  float expMinx, expPlusx;

  expPlusx = exp(x);
  expMinx = 1.0 / expPlusx;

  return (expPlusx - expMinx) / (expPlusx + expMinx);
}

///////////////////////////////////////////////////////////////////

float Gaussian(float x)

{
  return exp(-x * x);
}

///////////////////////////////////////////////////////////////////

float Identity(float x)

{
  return x;
}

///////////////////////////////////////////////////////////////////

float Power3(float x)

{
  return x * x * x;
}

///////////////////////////////////////////////////////////////////

float Power4(float x)

{
  return x * x * x * x;
}

///////////////////////////////////////////////////////////////////

float Power5(float x)

{
  return x * x * x * x * x;
}

///////////////////////////////////////////////////////////////////
