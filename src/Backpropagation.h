// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_BACKPROPAGATION_H
#define LOSEFACE_BACKPROPAGATION_H

#include "Mlp.h"

class PatternSet;
class Backpropagation;
class UpdateWeightsHelper;

/// It specifies how to modify learning rate in the training process.
///
class AdaptativeLearningRate
{
public:
  virtual ~AdaptativeLearningRate() { }
  virtual void beforePatterns(const Mlp& net, const PatternSet& training_set) = 0;
  virtual void afterPatterns(Backpropagation& bp, Mlp& net, const PatternSet& training_set) = 0;
  virtual AdaptativeLearningRate* clone() const = 0;
};

/// Default adaptative learning rate method
///
class NoAdaptativeLearningRate : public AdaptativeLearningRate
{
public:
  void beforePatterns(const Mlp& net, const PatternSet& training_set) { }
  void afterPatterns(Backpropagation& bp, Mlp& net, const PatternSet& training_set) { }
  NoAdaptativeLearningRate* clone() const { return new NoAdaptativeLearningRate(*this); }
};

class BoldDriverMethod : public AdaptativeLearningRate
{
  /// This variable is a backup of the net before processing all
  /// patterns, so with it we can cancel the whole training epoch.
  Mlp m_netBackup;

  /// Previous MSE at the epoch's beginning (to know if we have a
  /// performance improvement with the last epoch).
  double m_mse;

  /// How much we have to increment learning rate if we get better
  /// performance in each epoch.
  ///
  /// It's default value is 1.1.
  ///
  double m_increaseFactor;

  /// How much we have to decrement learning rate if we get worse
  /// performance in each epoch.
  ///
  /// It's default value is 0.5.
  ///
  double m_decreaseFactor;

public:
  BoldDriverMethod();

  double getIncreaseFactor() const { return m_increaseFactor; }
  double getDecreaseFactor() const { return m_decreaseFactor; }
  void setIncreaseFactor(double value) { m_increaseFactor = value; }
  void setDecreaseFactor(double value) { m_decreaseFactor = value; }

  void beforePatterns(const Mlp& net, const PatternSet& training_set);
  void afterPatterns(Backpropagation& bp, Mlp& net, const PatternSet& training_set);
  BoldDriverMethod* clone() const { return new BoldDriverMethod(*this); }
};

/// Steepest descent backpropagation[1] algorithm to train MLP models.
/// 
/// [1] D. E. Rumelhart., G. E. Hinton, R. J. Williams. 1986. "Learning internal representations by
/// error propagation,", In <em>Parallel Distributed Processing: Explorations in the Microstructure
/// of Cognition, Vol. 1: Foundations</em>, D. E. Rumelhart, J. L. McClelland, Eds. Mit Press
/// Computational Models Of Cognition And Perception Series. MIT Press, Cambridge, MA, 318-362.
/// 
class Backpropagation
{
  /// Training epoch.
  /// 
  unsigned m_epoch;

  /// Learning rate.
  /// 
  double m_eta;

  /// Net in training.
  /// 
  Mlp& m_net;

  /// Type of adaptative learning rate.
  ///
  AdaptativeLearningRate* m_adaptativeLearningRate;

  /// Momentum
  double m_mu;

  /// Helper to update MLP weights
  UpdateWeightsHelper* m_updateWeightsHelper;

public:
  Backpropagation(Mlp& net);
  ~Backpropagation();

  unsigned getEpoch() const { return m_epoch; }
  const Mlp& getNet() const { return m_net; }

  double getLearningRate() const { return m_eta; }
  double getMomentum() const { return m_mu; }
  void setLearningRate(double rate) { m_eta = rate; }
  void setMomentum(double mu) { m_mu = mu; }

  const AdaptativeLearningRate& getAdaptativeLearningRate() const;
  AdaptativeLearningRate& getAdaptativeLearningRate();
  void setAdaptativeLearningRate(const AdaptativeLearningRate& method);

  void train(const PatternSet& training_set);

};

#endif // LOSEFACE_BACKPROPAGATION_H
