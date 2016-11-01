// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "Mlp.h"
#include "ActivationFunctions.h"
#include "PatternSet.h"
#include "Random.h"

Mlp::Mlp()
  : m_hiddenFunc(new Logsig())
  , m_outputFunc(new Purelin())
{
}

Mlp::Mlp(size_t inputs, size_t hiddens, size_t outputs)
  : m_weight1(hiddens, inputs)
  , m_weight2(outputs, hiddens)
  , m_bias1(hiddens)
  , m_bias2(outputs)
  , m_hiddenFunc(new Logsig())
  , m_outputFunc(new Purelin())
{
  m_bias1.zero();
  m_bias2.zero();
}

Mlp::Mlp(const Mlp& mlp)
  : m_weight1(mlp.m_weight1)
  , m_weight2(mlp.m_weight2)
  , m_bias1(mlp.m_bias1)
  , m_bias2(mlp.m_bias2)
  , m_hiddenFunc(mlp.m_hiddenFunc->clone())
  , m_outputFunc(mlp.m_outputFunc->clone())
{
}

Mlp::~Mlp()
{
  delete m_hiddenFunc;
  delete m_outputFunc;
}

Mlp& Mlp::operator=(const Mlp& mlp)
{
  delete m_hiddenFunc;
  delete m_outputFunc;

  m_weight1 = mlp.m_weight1;
  m_weight2 = mlp.m_weight2;
  m_bias1 = mlp.m_bias1;
  m_bias2 = mlp.m_bias2;
  m_hiddenFunc = mlp.m_hiddenFunc->clone();
  m_outputFunc = mlp.m_outputFunc->clone();

  return *this;
}

Mlp& Mlp::operator+=(const Mlp& delta)
{
  m_weight1 += delta.m_weight1;
  m_weight2 += delta.m_weight2;
  m_bias1 += delta.m_bias1;
  m_bias2 += delta.m_bias2;
  return *this;
}

Mlp& Mlp::operator*=(double s)
{
  m_weight1 *= s;
  m_weight2 *= s;
  m_bias1 *= s;
  m_bias2 *= s;
  return *this;
}

void Mlp::setHiddenActivationFunction(const ActivationFunction& func)
{
  delete m_hiddenFunc;
  m_hiddenFunc = func.clone();
}

void Mlp::setOutputActivationFunction(const ActivationFunction& func)
{
  delete m_outputFunc;
  m_outputFunc = func.clone();
}

void Mlp::zero()
{
  m_weight1.zero();
  m_weight2.zero();
  m_bias1.zero();
  m_bias2.zero();
}

void Mlp::initRandom(double min_value, double max_value)
{
  size_t i, j, k;
  double range = (max_value - min_value);

  for (j=0; j<m_weight1.rows(); ++j)
    for (i=0; i<m_weight1.cols(); ++i)
      m_weight1(j, i) = min_value + range*Random::getReal();

  for (k=0; k<m_weight2.rows(); ++k)
    for (j=0; j<m_weight2.cols(); ++j)
      m_weight2(k, j) = min_value + range*Random::getReal();

  for (j=0; j<m_bias1.size(); ++j)
    m_bias1(j) = min_value + range*Random::getReal();

  for (k=0; k<m_bias2.size(); ++k)
    m_bias2(k) = min_value + range*Random::getReal();
}

void Mlp::recall(const Vector& input, Vector& hidden, Vector& output) const
{
#if 1
  hidden = m_weight1 * input + m_bias1;
  for (size_t j=0; j<hidden.size(); ++j)
    hidden(j) = m_hiddenFunc->f(hidden(j));

  output = m_weight2 * hidden + m_bias2;
  for (size_t k=0; k<output.size(); ++k)
    output(k) = m_outputFunc->f(output(k));
#else  // optimized (to avoid temporary objects)
  m_weight1.multiply(input, hidden);
  hidden += m_bias1;
  for (size_t j=0; j<hidden.size(); ++j)
    hidden(j) = hiddenFunc.f(hidden(j));

  m_weight2.multiply(hidden, output);
  output += m_bias2;
  for (size_t k=0; k<output.size(); ++k)
    output(k) = outputFunc.f(output(k));
#endif
}

void Mlp::recall(const Vector& input,
		 Vector& hidden0, Vector& hidden,
		 Vector& output0, Vector& output) const
{
#if 1
  hidden0 = m_weight1 * input + m_bias1;
  hidden.resize(hidden0.size());
  for (size_t j=0; j<hidden0.size(); ++j)
    hidden(j) = m_hiddenFunc->f(hidden0(j));

  output0 = m_weight2 * hidden + m_bias2;
  output.resize(output0.size());
  for (size_t k=0; k<output0.size(); ++k)
    output(k) = m_outputFunc->f(output0(k));
#else  // optimized
  m_weight1.multiply(input, hidden0);
  hidden0 += m_bias1;
  hidden.resize(hidden0.size());
  for (size_t j=0; j<hidden0.size(); ++j)
    hidden(j) = hiddenFunc.f(hidden0(j));

  m_weight2.multiply(hidden, output0);
  output0 += m_bias2;
  output.resize(output0.size());
  for (size_t k=0; k<output0.size(); ++k)
    output(k) = outputFunc.f(output0(k));
#endif
}

/// Calculates the sum of squared errors.
///
double Mlp::calcSSE(const PatternSet& set) const
{
  assert(!set.empty());

  PatternSet::const_iterator pattern, set_end = set.end();
  Vector hidden, output;
  double error = 0.0;

  // for each pattern of the set
  for (pattern=set.begin(); pattern!=set_end; ++pattern) {
    const Vector& input((*pattern)->getInput());
    const Vector& target((*pattern)->getOutput());
    recall(input, hidden, output);

    // calculate the difference between each "target" and "output"
    Vector::const_iterator target_k = target.begin(), target_end = target.end();
    Vector::iterator output_k = output.begin();
    for (; target_k != target_end; ++target_k, ++output_k) {
      // accumulate error
      error += std::pow(*target_k - *output_k, 2.0);
    }
  }

  return error;
}

/// Calculates the mean squared error (MSE).
///
double Mlp::calcMSE(const PatternSet& set) const
{
  return calcSSE(set) / (set.size() * getOutputs());
}

//////////////////////////////////////////////////////////////////////
// Binary I/O
//////////////////////////////////////////////////////////////////////

void Mlp::save(const char* filename) const
{
  std::ofstream f(filename, std::ios::binary);
  write(f);
}

void Mlp::load(const char* filename)
{
  std::ifstream f(filename, std::ios::binary);
  read(f);
}

void Mlp::write(std::ostream& s) const
{
  m_weight1.write(s);
  m_weight2.write(s);
  m_bias1.write(s);
  m_bias2.write(s);
}

void Mlp::read(std::istream& s)
{
  m_weight1.read(s);
  m_weight2.read(s);
  m_bias1.read(s);
  m_bias2.read(s);
}
