#include <math.h>

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the sigmoid activation function.
 *
 * The sigmoid function is defined as \f$ \sigma(x) = \frac{1}{1 + e^{-x}} \f$.
 *
 * @param x The input value.
 * @return The sigmoid of the input value.
 */
float Sigmoid(float x)

{
  return static_cast<float>(1.0 / (1.0 + exp(-x)));
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the hyperbolic tangent activation function.
 *
 * The hyperbolic tangent function is defined as \f$ \tanh(x) = \frac{e^x - e^{-x}}{e^x + e^{-x}} \f$.
 *
 * @param x The input value.
 * @return The hyperbolic tangent of the input value.
 */
float HyperTang(float x)

{
  float expMinx;
  float expPlusx;

  expPlusx = exp(x);
  expMinx = static_cast<float>(1.0) / expPlusx;

  return (expPlusx - expMinx) / (expPlusx + expMinx);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the Gaussian activation function.
 *
 * The Gaussian function is defined as \f$ f(x) = e^{-x^2} \f$.
 *
 * @param x The input value.
 * @return The Gaussian of the input value.
 */
float Gaussian(float x)

{
  return exp(-x * x);
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the identity activation function.
 *
 * The identity function is defined as \f$ f(x) = x \f$.
 *
 * @param x The input value.
 * @return The identity of the input value.
 */
float Identity(float x)

{
  return x;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the cubic power activation function.
 *
 * The cubic power function is defined as \f$ f(x) = x^3 \f$.
 *
 * @param x The input value.
 * @return The cubic power of the input value.
 */
float Power3(float x)

{
  return x * x * x;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the quartic power activation function.
 *
 * The quartic power function is defined as \f$ f(x) = x^4 \f$.
 *
 * @param x The input value.
 * @return The quartic power of the input value.
 */
float Power4(float x)

{
  return x * x * x * x;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief Computes the quintic power activation function.
 *
 * The quintic power function is defined as \f$ f(x) = x^5 \f$.
 *
 * @param x The input value.
 * @return The quintic power of the input value.
 */
float Power5(float x)

{
  return x * x * x * x * x;
}

///////////////////////////////////////////////////////////////////
