// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_ACTIVATIONFUNCTIONS_H
#define LOSEFACE_ACTIVATIONFUNCTIONS_H

#include <cmath>

/// Base class for activation functions.
///
class ActivationFunction
{
public:
  virtual double f(double x) = 0;
  virtual double df(double x, double s) = 0;

  virtual ActivationFunction* clone() const = 0;
};

/// Linear activation function.
///
class Purelin : public ActivationFunction
{
public:
  double f(double x)            { return x; }
  double df(double x, double s) { return double(1.0); }

  Purelin* clone() const { return new Purelin(*this); }
};

/// Logarithmic sigmoid activation function.
///
class Logsig : public ActivationFunction
{
public:
  double  f(double x)           { return double(1) / (double(1) + std::exp(-x)); }
  double df(double x, double s) { return s * (double(1) - s); }

  Logsig* clone() const { return new Logsig(*this); }
};

/// Hyperbolic tangent sigmoid activation function.
///
class Tansig : public ActivationFunction
{
public:
  double  f(double x)		{ return std::tanh(x); }
  double df(double x, double s)	{ return double(1) - s*s; }

  Tansig* clone() const { return new Tansig(*this); }
};

/// Radial basis function.
///
class Radbas : public ActivationFunction
{
public:
  double  f(double x)           { return std::exp(-x*x); }
  double df(double x, double s) { return -2*x*s; }

  Radbas* clone() const { return new Radbas(*this); }
};

#endif // LOSEFACE_ACTIVATIONFUNCTIONS_H
