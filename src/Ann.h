// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_ANN_H
#define LOSEFACE_ANN_H

#include <vector>
#include <cassert>

#include "Vector.h"
#include "Matrix.h"
#include "Random.h"

template<typename> class Pattern;
template<typename> class PatternSet;

template<typename> struct Purelin;
template<typename> struct Logsig;
template<typename> struct Tansig;
template<typename> struct Radbas;
template<typename, class, class> class Mlp;
template<class> class NetArray;

template<class> class WithMomentum;
template<class> class AccumulateWeights;
template<class> class AccumulateWeightsWithMomentum;
template<class> class UpdateWeightsImmediately;
template<class> class UpdateWeightsImmediatelyWithMomentum;
template<class> class NoAdaptativeLearningRate;
template<class> class BoldDriverMethod;
template<class,
	 template<class> class,
	 template<class> class>
class Backpropagation;

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

template<typename T>
class PatternSet
{
  std::vector< Pattern<T>* > m_set;

public:

  typedef typename std::vector< Pattern<T>* >::iterator iterator;
  typedef typename std::vector< Pattern<T>* >::const_iterator const_iterator;

  iterator begin() { return m_set.begin(); }
  iterator end() { return m_set.end(); }
  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }

  PatternSet() { }
  PatternSet(const PatternSet& copy) {
    operator=(copy);
  }
  ~PatternSet() {
    for (iterator it = begin(); it != end(); ++it)
      delete *it;
    m_set.clear();
  }

  PatternSet& operator=(const PatternSet& set) {
    m_set.reserve(set.size());
    for (const_iterator it = set.begin(); it != set.end(); ++it)
      push_back(**it);
    return *this;
  }

  void push_back(const Pattern<T>& p) {
    m_set.push_back(new Pattern<T>(p));
  }

  bool empty() const {
    return m_set.empty();
  }

  size_t size() const {
    return m_set.size();
  }

  Pattern<T>& operator[](size_t index) {
    return *m_set[index];
  }

  const Pattern<T>& operator[](size_t index) const {
    return *m_set[index];
  }

  void shuffle() {
    std::random_shuffle(begin(), end());
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
  typedef ::PatternSet<value_type> PatternSet;

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
    T range = (max_value - min_value);

    for (j=0; j<w1.rows(); ++j)
      for (i=0; i<w1.cols(); ++i)
	w1(j, i) = min_value + range*Random::getReal();

    for (k=0; k<w2.rows(); ++k)
      for (j=0; j<w2.cols(); ++j)
	w2(k, j) = min_value + range*Random::getReal();

    for (j=0; j<b1.size(); ++j)
      b1(j) = min_value + range*Random::getReal();

    for (k=0; k<b2.size(); ++k)
      b2(k) = min_value + range*Random::getReal();
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
  value_type calcSSE(const PatternSet& set) const {
    assert(!set.empty());

    typename PatternSet::const_iterator pattern, set_end = set.end();
    Vector input, hidden, output, target;
    value_type error = 0.0;

    // for each pattern of the set
    for (pattern=set.begin(); pattern!=set_end; ++pattern) {
      input = (*pattern)->input;
      target = (*pattern)->output;
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
  value_type calcMSE(const PatternSet& set) const {
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

/// An array of neural networks.
///
/// If you have a set of networks with one output (or a small number
/// of outputs), you could use this class to join all the networks'
/// outputs in just one vector.
///
/// E.g. With this class you could compare the performance of a MLP of
/// 10 outputs (Mlp) versus 10 MLPs of 1 output (NetArray).
///
template<class Net>
class NetArray
{
  typedef typename std::vector<Net> Nets;
  Nets m_nets;
  int m_outputs;

public:
  typedef typename Net::value_type value_type;
  typedef ::Vector<value_type> Vector;
  typedef ::Matrix<value_type> Matrix;
  typedef ::Pattern<value_type> Pattern;
  typedef ::PatternSet<value_type> PatternSet;

  NetArray()
  {
    m_outputs = 0;
  }

  NetArray(const NetArray& net)
    : m_nets(net.m_nets)
    , m_outputs(net.m_outputs)
  {
  }

  size_t getInputs() const { return m_nets.empty() ? 0: m_nets[0].getInputs(); }
  size_t getOutputs() const { return m_outputs; }

  Vector createInput() const { return Vector(getInputs()); }
  Vector createOutput() const { return Vector(getOutputs()); }

  /// Adds a new network to the array.
  ///
  /// @param net
  ///   The new neural network to be copied and added to the array. This network
  ///   must contain the same quantity of inputs as all other networks in the array.
  ///
  void add(const Net& net)
  {
    assert(m_nets.empty() || net.getInputs() == getInputs());

    m_nets.push_back(net);
    m_outputs += net.getOutputs();
  }

  NetArray& operator=(const NetArray& net) {
    m_nets = net.m_nets;
    return *this;
  }

  void recall(const Vector& input, Vector& output) const
  {
    assert(!m_nets.empty());

    Vector hidden, it_output;
    size_t i = 0;

    output = createOutput();

    for (typename Nets::const_iterator
	   it = m_nets.begin(); it != m_nets.end(); ++it) {
      it->recall(input, hidden, it_output);

      for (size_t j=0; j<it_output.size(); ++j, ++i)
	output(i) = it_output(j);
    }
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
    size_t n = m_nets.size();
    s.write((char*)&n, sizeof(size_t));

    for (typename Nets::const_iterator
  	   it = m_nets.begin(); it != m_nets.end(); ++it) {
      it->write(s);
    }
  }

  void read(std::istream& s)
  {
    size_t n;
    s.read((char*)&n, sizeof(size_t));

    for (size_t c=0; c<n; ++c) {
      Net n;
      n.read(s);
      add(n);
    }
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

  value_type m_mu;

  WithMomentum() {
    m_mu = 0.5;
  }

public:

  inline void setMomentum(value_type mu) {
    m_mu = mu;
  }

  inline value_type getMomentum() const {
    return m_mu;
  }
};

template<class Net>
class AccumulateWeights
{
protected:
  typedef typename Net::value_type value_type;
  typedef typename Net::Vector Vector;
  typedef typename Net::Matrix Matrix;

  Net m_accumDelta;

  inline void beforeAllPatterns(Net& net)
  {
    m_accumDelta = net;
    m_accumDelta.zero();
  }

  inline void onePatternStep(Net& net, Net& delta) {
    m_accumDelta += delta;
  }

  inline void afterAllPatterns(Net& net) {
    net += m_accumDelta;
  }

};

template<class Net>
class AccumulateWeightsWithMomentum : public AccumulateWeights<Net>
				    , public WithMomentum<Net>
{
protected:
  Net m_oldDelta, tmp;
  bool valid;

  AccumulateWeightsWithMomentum() {
    valid = false;
  }

  inline void beforeAllPatterns(Net& net) {
    AccumulateWeights<Net>::beforeAllPatterns(net);

    if (!valid) {
      valid = true;
      m_oldDelta = net;
    }
  }

  inline void afterAllPatterns(Net& net) {
    AccumulateWeights<Net>::afterAllPatterns(net);

#if 0
    net += m_oldDelta * WithMomentum<Net>::m_mu;
#else  // optimized (without temporary objects)
    tmp = m_oldDelta;
    tmp *= WithMomentum<Net>::m_mu;
    net += tmp;
#endif

    m_oldDelta = AccumulateWeights<Net>::m_accumDelta;
  }

};

template<class Net>
class UpdateWeightsImmediately
{
protected:
  inline void beforeAllPatterns(Net& net) {
    // do nothing
  }
  
  inline void onePatternStep(Net& net, Net& delta) {
    net += delta;
  }

  inline void afterAllPatterns(Net& net) {
    // do nothing
  }
};

template<class Net>
class UpdateWeightsImmediatelyWithMomentum : public UpdateWeightsImmediately<Net>
					   , public WithMomentum<Net>
{
protected:
  Net m_oldDelta, tmp;
  bool valid;

  UpdateWeightsImmediatelyWithMomentum() {
    valid = false;
  }

  inline void onePatternStep(Net& net, Net& delta) {
    if (!valid) {
      valid = true;
      m_oldDelta = delta;
      m_oldDelta.zero();
    }

#if 0
    m_oldDelta = delta + m_oldDelta * WithMomentum<Net>::m_mu;
    net += m_oldDelta;
#else  // optimized (without temporary objects)
    tmp = m_oldDelta;
    tmp *= WithMomentum<Net>::m_mu;
    tmp += delta;
    net += tmp;
    m_oldDelta = tmp;
#endif
  }

};

template<class Net>
class NoAdaptativeLearningRate
{
public:
  template<class Algorithm>
  inline void beforeAllPatterns(Algorithm& self, const typename Net::PatternSet& training_set) {
    // do nothing
  }

  template<class Algorithm>
  inline void afterAllPatterns(Algorithm& self, const typename Net::PatternSet& training_set) {
    // do nothing
  }
};

template<class Net>
class BoldDriverMethod
{
  /// This variable is a backup of the net before processing all
  /// patterns, so with it we can cancel the whole training epoch.
  Net m_netBackup;

  /// Previous MSE at the epoch's beginning (to know if we have a
  /// performance improvement with the last epoch).
  double m_mse;

  /// How much we have to increment learning rate if we get better
  /// performance in each epoch.
  double m_increaseFactor;

  /// How much we have to decrement learning rate if we get worse
  /// performance in each epoch.
  double m_decreaseFactor;

public:
  BoldDriverMethod() {
    m_mse = -1;
    m_increaseFactor = 1.1;
    m_decreaseFactor = 0.5;
  }

  template<class Algorithm>
  inline void beforeAllPatterns(Algorithm& self, const typename Net::PatternSet& training_set) {
    m_netBackup = self.getNet();
    m_mse = self.getNet().calcMSE(training_set);
  }

  template<class Algorithm>
  inline void afterAllPatterns(Algorithm& self, const typename Net::PatternSet& training_set) {
    double newMse = self.getNet().calcMSE(training_set);

    // We are getting better (less error)
    if (newMse < m_mse) {
      // Increase learning rate
      self.setLearningRate(m_increaseFactor * self.getLearningRate());
    }
    // We are getting worse (more error)
    else {
      // Undo the this epoch
      self.setNet(m_netBackup);

      // Decrease learning rate
      self.setLearningRate(m_decreaseFactor * self.getLearningRate());
    }
  }
};

//////////////////////////////////////////////////////////////////////
// Training Algorithms
//////////////////////////////////////////////////////////////////////

/// Steepest descent backpropagation[1] algorithm to train MLP models.
/// 
/// @tparam Net Neural network model to be trained.
/// @tparam LearningRateModificator Policy to specify how to modify learning rate
///   in the training process:
///   - NoAdaptativeLearningRate (default)
///   - BoldDriverMethod
/// @tparam WeightsModificator Special policy to modify weights:
///   - AccumulateWeights: for batch processing. It accumulates all
///     neurons' weight variation for each pattern and then updates the
///     weights.
///   - AccumulateWeightsWithMomentum: with momentum at the final weight update.
///   - UpdateWeightsImmediately: for online training (updates the weights after
///     each pattern is used)
///   - UpdateWeightsImmediatelyWithMomentum (default): same as UpdateWeightsImmediately
///     but with momentum
///
/// [1] D. E. Rumelhart., G. E. Hinton, R. J. Williams. 1986. "Learning internal representations by
/// error propagation,", In <em>Parallel Distributed Processing: Explorations in the Microstructure
/// of Cognition, Vol. 1: Foundations</em>, D. E. Rumelhart, J. L. McClelland, Eds. Mit Press
/// Computational Models Of Cognition And Perception Series. MIT Press, Cambridge, MA, 318-362.
/// 
template<class Net,
	 template<class Net> class LearningRatePolicy = NoAdaptativeLearningRate,
	 template<class Net> class WeightsPolicy = UpdateWeightsImmediatelyWithMomentum>
class Backpropagation : public LearningRatePolicy<Net>
		      , public WeightsPolicy<Net>
{
public:
  typedef typename Net::value_type value_type;
  typedef ::Vector<value_type> Vector;
  typedef ::Matrix<value_type> Matrix;
  typedef ::Pattern<value_type> Pattern;
  typedef ::PatternSet<value_type> PatternSet;

private:
  /// Training epoch.
  /// 
  unsigned m_epoch;

  /// Learning rate.
  /// 
  value_type m_eta;

  /// Net in training.
  /// 
  Net& m_net;

public:

  /// Creates a new instance of the Back-Propagation algorithm to train
  /// the specified net.
  /// 
  /// @param net The MLP to be trained (the initial weights of this net
  ///   will be used as start point).
  /// 
  Backpropagation(Net& net)
    : m_net(net)
  {
    m_epoch = 0;
    m_eta = 0.0001;
  }

  inline const Net& getNet() const {
    return m_net;
  }

  inline void setNet(const Net& net) {
    m_net = net;
  }

  inline value_type getLearningRate() const {
    return m_eta;
  }

  inline void setLearningRate(value_type rate) {
    m_eta = rate;
  }

  inline unsigned getEpoch() const {
    return m_epoch;
  }

  /// Trains just one epoch.
  /// 
  void train(const PatternSet& training_set) {
    typename PatternSet::const_iterator pattern;
    size_t i, j, k;

    // vectors
    Vector hidden0, hidden, delta_hidden;
    Vector output0, output, delta_output;

    // delta for weights
    Net delta(m_net);
    delta.zero();

    // policies pre-processing
    this->LearningRatePolicy<Net>::beforeAllPatterns(*this, training_set);
    this->WeightsPolicy<Net>::beforeAllPatterns(m_net);
  
    // for each pattern in the training set
    for (pattern=training_set.begin();
	 pattern!=training_set.end(); ++pattern) {
      const Vector& input((*pattern)->input);
      const Vector& target((*pattern)->output);

      // forward propagation phase
      m_net.recall(input, hidden0, hidden, output0, output);

      // backward pass

      // ...for output neurons
      delta_output = target - output;
      for (k=0; k<m_net.getOutputs(); ++k) {
	delta_output(k) *= Net::OutputFunc::df(output0(k), output(k));
	delta.b2(k) = m_eta * delta_output(k);

	for (j=0; j<m_net.getHiddens(); ++j)
	  delta.w2(k, j) = m_eta * delta_output(k) * hidden(j);
      }

      // ..for hidden neurons
      delta_hidden = m_net.w2.getTranspose() * delta_output;
      for (j=0; j<m_net.getHiddens(); ++j) {
	delta_hidden(j) *= Net::HiddenFunc::df(hidden0(j), hidden(j));
	delta.b1(j) = m_eta * delta_hidden(j);

	for (i=0; i<m_net.getInputs(); ++i)
	  delta.w1(j, i) = m_eta * delta_hidden(j) * input(i);
      }

      this->WeightsPolicy<Net>::onePatternStep(m_net, delta);
    }

    // policies post-processing
    this->WeightsPolicy<Net>::afterAllPatterns(m_net);
    this->LearningRatePolicy<Net>::afterAllPatterns(*this, training_set);

    m_epoch++;
  }

};

//////////////////////////////////////////////////////////////////////

#endif // LOSEFACE_ANN_H
