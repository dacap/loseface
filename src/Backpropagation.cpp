// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Backpropagation.h"
#include "Mlp.h"
#include "PatternSet.h"
#include "ActivationFunctions.h"

//////////////////////////////////////////////////////////////////////
// Helper class to update weights
//////////////////////////////////////////////////////////////////////

class UpdateWeightsHelper
{
  Mlp m_oldDelta, m_tmp;
  bool m_valid;

public:
  UpdateWeightsHelper()
  {
  }

  void beforePatterns()
  {
    m_valid = false;
  }

  void applyWeights(Mlp& net, Mlp& delta, double momentum)
  {
    if (!m_valid) {
      m_valid = true;
      m_oldDelta = delta;	// It is only to setup an MLP of the same dimmensions
      m_oldDelta.zero();	// <- because here we put all weights to zero
    }

#if 0
    m_oldDelta = delta + m_oldDelta * momentum;
    net += m_oldDelta;
#else  // optimized (without temporary objects)
    m_tmp = m_oldDelta;
    m_tmp *= momentum;
    m_tmp += delta;
    net += m_tmp;
    m_oldDelta = m_tmp;
#endif
  }

};

//////////////////////////////////////////////////////////////////////
// BoldDriverMethod
//////////////////////////////////////////////////////////////////////

BoldDriverMethod::BoldDriverMethod()
{
  m_increaseFactor = 1.1;
  m_decreaseFactor = 0.5;
}

void BoldDriverMethod::beforePatterns(const Mlp& net, const PatternSet& training_set)
{
  m_netBackup = net;	// Copy the whole net
  m_mse       = net.calcMSE(training_set);
}

void BoldDriverMethod::afterPatterns(Backpropagation& bp, Mlp& net, const PatternSet& training_set)
{
  double newMse = net.calcMSE(training_set);

  // We are getting better (less error)
  if (newMse < m_mse) {
    // Increase learning rate
    bp.setLearningRate(m_increaseFactor * bp.getLearningRate());
  }
  // We are getting worse (more error)
  else {
    // Undo the this epoch
    net = m_netBackup;

    // Decrease learning rate
    bp.setLearningRate(m_decreaseFactor * bp.getLearningRate());
  }
}

//////////////////////////////////////////////////////////////////////
// Backpropagation
//////////////////////////////////////////////////////////////////////

/// Creates a new instance of the Back-Propagation algorithm to train
/// the specified net.
/// 
/// @param net The MLP to be trained (the initial weights of this net
///   will be used as start point).
/// 
Backpropagation::Backpropagation(Mlp& net)
  : m_net(net)
{
  m_epoch = 0;
  m_eta = 0.0001;
  m_adaptativeLearningRate = new NoAdaptativeLearningRate();
  m_mu = 0.0;
  m_updateWeightsHelper = new UpdateWeightsHelper();
}

Backpropagation::~Backpropagation()
{
  delete m_adaptativeLearningRate;
  delete m_updateWeightsHelper;
}

AdaptativeLearningRate& Backpropagation::getAdaptativeLearningRate()
{
  return *m_adaptativeLearningRate;
}

const AdaptativeLearningRate& Backpropagation::getAdaptativeLearningRate() const
{
  return *m_adaptativeLearningRate;
}

void Backpropagation::setAdaptativeLearningRate(const AdaptativeLearningRate& method)
{
  delete m_adaptativeLearningRate;
  m_adaptativeLearningRate = method.clone();
}

/// Trains just one epoch.
/// 
void Backpropagation::train(const PatternSet& training_set)
{
  PatternSet::const_iterator pattern;
  size_t i, j, k;

  // vectors
  Vector hidden0, hidden, delta_hidden;
  Vector output0, output, delta_output;

  // delta for weights
  Mlp delta(m_net);
  delta.zero();

  // Pre-processing policies
  m_updateWeightsHelper->beforePatterns();
  m_adaptativeLearningRate->beforePatterns(m_net, training_set);
  
  // for each pattern in the training set
  for (pattern=training_set.begin();
       pattern!=training_set.end(); ++pattern) {
    const Vector& input((*pattern)->getInput());
    const Vector& target((*pattern)->getOutput());

    // forward propagation phase
    m_net.recall(input, hidden0, hidden, output0, output);

    // backward pass

    // ...for output neurons
    delta_output = target - output;
    for (k=0; k<m_net.getOutputs(); ++k) {
      delta_output(k) *= m_net.m_outputFunc->df(output0(k), output(k));
      delta.m_bias2(k) = m_eta * delta_output(k);

      for (j=0; j<m_net.getHiddens(); ++j)
	delta.m_weight2(k, j) = m_eta * delta_output(k) * hidden(j);
    }

    // ..for hidden neurons
    delta_hidden = m_net.m_weight2.getTranspose() * delta_output;
    for (j=0; j<m_net.getHiddens(); ++j) {
      delta_hidden(j) *= m_net.m_hiddenFunc->df(hidden0(j), hidden(j));
      delta.m_bias1(j) = m_eta * delta_hidden(j);

      for (i=0; i<m_net.getInputs(); ++i)
	delta.m_weight1(j, i) = m_eta * delta_hidden(j) * input(i);
    }

    // Apply delta to the weights
    m_updateWeightsHelper->applyWeights(m_net, delta, m_mu);
  }

  // Post-processing policies
  m_adaptativeLearningRate->afterPatterns(*this, m_net, training_set);

  m_epoch++;
}
