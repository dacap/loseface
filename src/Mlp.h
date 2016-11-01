// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_MLP_H
#define LOSEFACE_MLP_H

#include "Matrix.h"

class ActivationFunction;
class Backpropagation;
class PatternSet;

/// A specific feedforward multilayer perceptron (MLP) neural network
/// with 3 layers of neurons: input, hidden, output.
///
class Mlp
{
  friend class Backpropagation;

  Matrix m_weight1;
  Matrix m_weight2;
  Vector m_bias1;
  Vector m_bias2;
  ActivationFunction* m_hiddenFunc;
  ActivationFunction* m_outputFunc;

public:

  Mlp();
  Mlp(size_t inputs, size_t hiddens, size_t outputs);
  Mlp(const Mlp& mlp);
  ~Mlp();

  size_t getInputs() const { return m_weight1.cols(); }
  size_t getHiddens() const { return m_weight1.rows(); }
  size_t getOutputs() const { return m_weight2.rows(); }

  Vector createInput() const { return Vector(getInputs()); }
  Vector createHidden() const { return Vector(getHiddens()); }
  Vector createOutput() const { return Vector(getOutputs()); }

  Mlp& operator=(const Mlp& mlp);
  Mlp& operator+=(const Mlp& delta);
  Mlp& operator*=(double s);

  void setHiddenActivationFunction(const ActivationFunction& func);
  void setOutputActivationFunction(const ActivationFunction& func);

  void zero();
  void initRandom(double min_value, double max_value);
  void recall(const Vector& input, Vector& hidden, Vector& output) const;
  void recall(const Vector& input,
	      Vector& hidden0, Vector& hidden,
	      Vector& output0, Vector& output) const;

  double calcSSE(const PatternSet& set) const;
  double calcMSE(const PatternSet& set) const;

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////

  void save(const char* filename) const;
  void load(const char* filename);
  void write(std::ostream& s) const;
  void read(std::istream& s);

};

#endif // LOSEFACE_MLP_H
