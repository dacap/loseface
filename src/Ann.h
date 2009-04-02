// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LOSEFACE_ANN_H
#define LOSEFACE_ANN_H

#include <vector>
#include "Vector.h"
#include "Matrix.h"

//////////////////////////////////////////////////////////////////////
// Patterns
//////////////////////////////////////////////////////////////////////

/// A pair of input/output vectors.
///
template<typename T>
class Pattern
{
public:
  typedef T value_type;
  typedef std::vector< Pattern<T> > Set;

  Vector<T> input;
  Vector<T> output;

  Pattern() { }
  Pattern(int inputs, int outputs)
    : input(inputs)
    , output(outputs) { }
  Pattern(const Pattern& p)
    : input(p.input)
    , output(p.output) { }
  Pattern(const Vector<T>& input,
	  const Vector<T>& output)
    : input(input)
    , output(output) { }
  Pattern& operator=(const Pattern& p) {
    input = p.input;
    output = p.output;
    return *this;
  }
};

//////////////////////////////////////////////////////////////////////
// Activation functions
//////////////////////////////////////////////////////////////////////

/// Linear activation function.
/// 
template<typename T>
struct Purelin
{
  inline static T  f(T x)      { return x; }
  inline static T df(T x, T s) { return T(1); }
};

/// Logarithmic sigmoid activation function.
/// 
template<typename T>
struct Logsig
{
  inline static T  f(T x)      { return T(1) / (T(1) + std::exp(-x)); }
  inline static T df(T x, T s) { return s * (T(1) - s); }
};

/// Hyperbolic tangent sigmoid activation function.
/// 
template<typename T>
struct Tansig
{
  inline static T  f(T x)      { return std::tanh(x); }
  inline static T df(T x, T s) { return T(1) - s*s; }
};

/// Radial basis function.
/// 
template<typename T>
struct Radbas
{
  inline static T  f(T x)      { return std::exp(-x*x); }
  inline static T df(T x, T s) { return -2*x*s; }
};

//////////////////////////////////////////////////////////////////////
// Artificial Neural Networks
//////////////////////////////////////////////////////////////////////

/// A specific feedforward multilayer perceptron (MLP) neural network
/// with 3 layers of neurons: input, hidden, output.
/// 
/// @tparam T scalar type to be used in matrices/vectors (e.g. double).
/// @tparam HAF Activation function in the hidden neurons.
/// @tparam OAF Activation function in the output neurons.
/// 
template<typename T,
	 class HAF = Logsig<T>,
	 class OAF = Purelin<T> >
class Mlp
{
public:
  typedef T value_type;
  typedef HAF HiddenFunc;
  typedef OAF OutputFunc;
  typedef ::Vector<value_type> Vector;
  typedef ::Matrix<value_type> Matrix;
  typedef ::Pattern<value_type> Pattern;
  typedef typename Pattern::Set Set;

  Matrix w1, w2; // weights
  Vector b1, b2; // bias

  Mlp()
  {
  }

  Mlp(size_t inputs, size_t hiddens, size_t outputs)
    : w1(hiddens, inputs)
    , w2(outputs, hiddens)
    , b1(hiddens)
    , b2(outputs)
  {
    b1.zero();
    b2.zero();
  }

  Mlp(const Mlp& mlp)
    : w1(mlp.w1)
    , w2(mlp.w2)
    , b1(mlp.b1)
    , b2(mlp.b2)
  {
  }

  size_t getInputs() const { return w1.cols(); }
  size_t getHiddens() const { return w1.rows(); }
  size_t getOutputs() const { return w2.rows(); }

  Vector createInput() const { return Vector(getInputs()); }
  Vector createHidden() const { return Vector(getHiddens()); }
  Vector createOutput() const { return Vector(getOutputs()); }

  Mlp& operator=(const Mlp& mlp) {
    w1 = mlp.w1;
    w2 = mlp.w2;
    b1 = mlp.b1;
    b2 = mlp.b2;
    return *this;
  }

  Mlp& operator+=(const Mlp& delta) {
    w1 += delta.w1;
    w2 += delta.w2;
    b1 += delta.b1;
    b2 += delta.b2;
    return *this;
  }

  Mlp& operator*=(value_type s) {
    w1 *= s;
    w2 *= s;
    b1 *= s;
    b2 *= s;
    return *this;
  }

  void zero() {
    w1.zero();
    w2.zero();
    b1.zero();
    b2.zero();
  }

  void initRandom(T min_value, T max_value) {
    size_t i, j, k;
    T range = (max_value - min_value) / static_cast<T>(10000);

    for (j=0; j<w1.rows(); ++j)
      for (i=0; i<w1.cols(); ++i)
	w1(j, i) = min_value + static_cast<T>(std::rand() % 10001) * range;

    for (k=0; k<w2.rows(); ++k)
      for (j=0; j<w2.cols(); ++j)
	w2(k, j) = min_value + static_cast<T>(std::rand() % 10001) * range;

    for (j=0; j<b1.size(); ++j)
      b1(j) = min_value + static_cast<T>(std::rand() % 10001) * range;

    for (k=0; k<b2.size(); ++k)
      b2(k) = min_value + static_cast<T>(std::rand() % 10001) * range;
  }

  void recall(const Vector& input, Vector& hidden, Vector& output) const
  {
#if 1
    hidden = w1 * input + b1;
    for (size_t j=0; j<hidden.size(); ++j)
      hidden(j) = HiddenFunc::f(hidden(j));

    output = w2 * hidden + b2;
    for (size_t k=0; k<output.size(); ++k)
      output(k) = OutputFunc::f(output(k));
#else  // optimized (to avoid temporary objects)
    w1.multiply(input, hidden);
    hidden += b1;
    for (size_t j=0; j<hidden.size(); ++j)
      hidden(j) = HiddenFunc::f(hidden(j));

    w2.multiply(hidden, output);
    output += b2;
    for (size_t k=0; k<output.size(); ++k)
      output(k) = OutputFunc::f(output(k));
#endif
  }

  void recall(const Vector& input,
	      Vector& hidden0, Vector& hidden,
	      Vector& output0, Vector& output) const
  {
#if 1
    hidden0 = w1 * input + b1;
    hidden.resize(hidden0.size());
    for (size_t j=0; j<hidden0.size(); ++j)
      hidden(j) = HiddenFunc::f(hidden0(j));

    output0 = w2 * hidden + b2;
    output.resize(output0.size());
    for (size_t k=0; k<output0.size(); ++k)
      output(k) = OutputFunc::f(output0(k));
#else  // optimized
    w1.multiply(input, hidden0);
    hidden0 += b1;
    hidden.resize(hidden0.size());
    for (size_t j=0; j<hidden0.size(); ++j)
      hidden(j) = HiddenFunc::f(hidden0(j));

    w2.multiply(hidden, output0);
    output0 += b2;
    output.resize(output0.size());
    for (size_t k=0; k<output0.size(); ++k)
      output(k) = OutputFunc::f(output0(k));
#endif
  }

  /// Calculates the sum of squared errors.
  /// 
  value_type calcSSE(const Set& set) const {
    assert(!set.empty());

    typename Set::const_iterator pattern, set_end = set.end();
    Vector input, hidden, output, target;
    value_type error = 0.0;

    // for each pattern of the set
    for (pattern=set.begin(); pattern!=set_end; ++pattern) {
      input = pattern->input;
      target = pattern->output;
      recall(input, hidden, output);

      // calculate the difference between each "target" and "output"
      typename Vector::iterator target_k = target.begin(), target_end = target.end();
      typename Vector::iterator output_k = output.begin();
      for (; target_k != target_end; ++target_k, ++output_k) {
	// accumulate error
	error += std::pow(*target_k - *output_k, 2.0);
      }
    }

    return error;
  }

  /// Calculates the mean squared error (MSE).
  /// 
  value_type calcMSE(const Set& set) const {
    return calcSSE(set) / (set.size() * getOutputs());
  }

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////
  
  void save(const char* filename) const
  {
    std::ofstream f(filename, std::ios::binary);
    write(f);
  }

  void load(const char* filename)
  {
    std::ifstream f(filename, std::ios::binary);
    read(f);
  }

  void write(std::ostream& s) const
  {
    w1.write(s);
    w2.write(s);
    b1.write(s);
    b2.write(s);
  }

  void read(std::istream& s)
  {
    w1.read(s);
    w2.read(s);
    b1.read(s);
    b2.read(s);
  }

};

//////////////////////////////////////////////////////////////////////
// Helpers/Modifications for the Training Algorithms
//////////////////////////////////////////////////////////////////////

template<class Net>
class WithMomentum
{
protected:
  typedef typename Net::value_type value_type;

  value_type mu;

public:
  inline void setMomentum(value_type mu) {
    this->mu = mu;
  }
};
/*
template<class Net>
class AccumulateWeights
{
protected:
  typedef typename Net::value_type value_type;
  typedef typename Net::Vector Vector;
  typedef typename Net::Matrix Matrix;

  Net accum_delta;

  inline void initDeltaWeights(Net& net)
  {
    accum_delta = net;
    accum_delta.zero();
  }

  inline void stepWeights(Net& net, Net& delta) {
    accum_delta += delta;
  }

  inline void updateWeights(Net& net) {
    net += accum_delta;
  }

};

template<class Net>
class AccumulateWeightsWithMomentum : public AccumulateWeights<Net>
				    , public WithMomentum<Net>
{
protected:
  Net old_delta, tmp;
  bool valid;

  AccumulateWeightsWithMomentum() {
    valid = false;
    WithMomentum<Net>::setMomentum(0.09);
  }

  inline void initDeltaWeights(Net& net) {
    AccumulateWeights<Net>::initDeltaWeights(net);

    if (!valid) {
      valid = true;
      old_delta = net;
    }
  }

  inline void updateWeights(Net& net) {
    AccumulateWeights<Net>::updateWeights(net);

#if 1
    net += old_delta * WithMomentum<Net>::mu;
#else  // optimized (without temporary objects)
    tmp = old_delta;
    tmp *= WithMomentum<Net>::mu;
    net += tmp;
#endif

    old_delta = AccumulateWeights<Net>::accum_delta;
  }

};
*/
template<class Net>
class UpdateWeightsImmediately
{
protected:
  inline void initDeltaWeights(Net& net) {
    // do nothing
  }
  
  inline void stepWeights(Net& net, Net& delta) {
    net += delta;
  }

  inline void updateWeights(Net& net) {
    // do nothing
  }
};

template<class Net>
class UpdateWeightsImmediatelyWithMomentum : public UpdateWeightsImmediately<Net>
					   , public WithMomentum<Net>
{
protected:
  Net old_delta, tmp;
  bool valid;

  UpdateWeightsImmediatelyWithMomentum() {
    valid = false;
    WithMomentum<Net>::setMomentum(0.99);
  }

  inline void stepWeights(Net& net, Net& delta) {
    if (!valid) {
      valid = true;
      old_delta = delta;
      old_delta.zero();
    }

#if 0
    old_delta = delta + old_delta * WithMomentum<Net>::mu;
    net += old_delta;
#else  // optimized (without temporary objects)
    tmp = old_delta;
    tmp *= WithMomentum<Net>::mu;
    tmp += delta;
    net += tmp;
    old_delta = tmp;
#endif
  }

};

/// @todo 
/// 
template<class Net>
class MaximumTolerance
{
public:
};

/// @todo 
/// 
template<class Net>
class OutputTolerance
{
public:
};

//////////////////////////////////////////////////////////////////////
// Training Algorithms
//////////////////////////////////////////////////////////////////////

/// Steepest descent back-propagation algorithm to train MLP models.
/// 
/// @tparam Net Neural network to be trained.
/// @tparam WeightsModificator Special policy to modify weights:
///   - AccumulateWeights: for batch processing. It accumulates all
///     neurons' weight variation for each pattern and then updates the
///     weights.
///   - AccumulateWeightsWithMomentum: with momentum at the final weight update.
///   - UpdateWeightsImmediately: for online training (updates the weights after
///     each pattern is used)
///   - UpdateWeightsImmediatelyWithMomentum: same as UpdateWeightsImmediately but
///     with momentum
///
/// @verbatim
/// D. E. Rumelhart., G. E. Hinton, R. J. Williams. 1986. "Learning internal representations by
/// error propagation,", In Parallel Distributed Processing: Explorations in the Microstructure
/// of Cognition, Vol. 1: Foundations, D. E. Rumelhart, J. L. McClelland, Eds. Mit Press
/// Computational Models Of Cognition And Perception Series. MIT Press, Cambridge, MA, 318-362.
/// @endverbatim
/// 
template<class Net,
	 template<class Net> class WeightsModificator = UpdateWeightsImmediatelyWithMomentum,
	 template<class Net> class Tolerance = MaximumTolerance>
class BackPropagation : public WeightsModificator<Net>
		      , public Tolerance<Net>
{
public:
  typedef typename Net::value_type value_type;
  typedef ::Vector<value_type> Vector;
  typedef ::Matrix<value_type> Matrix;
  typedef ::Pattern<value_type> Pattern;
  typedef typename Pattern::Set Set;

private:
  /// Training epoch.
  /// 
  unsigned epoch;

  /// Learning rate.
  /// 
  value_type eta;

  /// Net in training.
  /// 
  Net& net;

public:

  /// Creates a new instance of the Back-Propagation algorithm to train
  /// the specified net.
  /// 
  /// @param net The MLP to be trained (the initial weights of this net
  ///   will be used as start point).
  /// 
  BackPropagation(Net& net)
    : net(net)
  {
    epoch = 0;
    eta = 0.0001;
  }

  value_type getLearningRate() const { return eta; }
  unsigned getEpoch() const { return epoch; }
  const Net& getNet() const { return net; }

  void setLearningRate(value_type lr) { eta = lr; }

  /// Trains just one epoch.
  /// 
  void train(const Set& training_set) {
    typename Set::const_iterator pattern;
    size_t i, j, k;

    // vectors
    Vector hidden0, hidden, delta_hidden;
    Vector output0, output, delta_output;

    // delta for weights
    Net delta = net;
    delta.zero();

    // initialize the weights modification policy
    initDeltaWeights(net);

    // for each pattern in the training set
    for (pattern=training_set.begin();
	 pattern!=training_set.end(); ++pattern) {
      const Vector& input(pattern->input);
      const Vector& target(pattern->output);

      // forward propagation phase
      net.recall(input, hidden0, hidden, output0, output);

      // backward pass

      // ...for output neurons
      delta_output = target - output;
      for (k=0; k<net.getOutputs(); ++k) {
	delta_output(k) *= Net::OutputFunc::df(output0(k), output(k));
	delta.b2(k) = eta * delta_output(k);

	for (j=0; j<net.getHiddens(); ++j)
	  delta.w2(k, j) = eta * delta_output(k) * hidden(j);
      }

      // ..for hidden neurons
      delta_hidden = net.w2.getTranspose() * delta_output;
      for (j=0; j<net.getHiddens(); ++j) {
	delta_hidden(j) *= Net::HiddenFunc::df(hidden0(j), hidden(j));
	delta.b1(j) = eta * delta_hidden(j);

	for (i=0; i<net.getInputs(); ++i)
	  delta.w1(j, i) = eta * delta_hidden(j) * input(i);
      }

      stepWeights(net, delta);
    }
    updateWeights(net);

    epoch++;
  }

};

#endif // LOSEFACE_ANN_H
