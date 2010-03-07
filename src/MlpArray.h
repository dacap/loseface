// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_MLPARRAY_H
#define LOSEFACE_MLPARRAY_H

#include <list>
#include "Mlp.h"

/// An array of neural networks.
///
/// If you have a set of networks with one output (or a small number
/// of outputs), you could use this class to join all the networks'
/// outputs in just one vector.
///
/// E.g. With this class you could compare the performance of a MLP of
/// 10 outputs (Mlp) versus 10 MLPs of 1 output (MlpArray).
///
class MlpArray
{
  typedef std::list<Mlp> Nets;
  Nets m_nets;
  size_t m_outputs;

public:
  MlpArray();
  MlpArray(const MlpArray& net);
  MlpArray& operator=(const MlpArray& net);

  size_t getInputs() const { return m_nets.empty() ? 0: m_nets.front().getInputs(); }
  size_t getOutputs() const { return m_outputs; }

  Vector createInput() const { return Vector(getInputs()); }
  Vector createOutput() const { return Vector(getOutputs()); }

  void add(const Mlp& net);
  void recall(const Vector& input, Vector& output) const;

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////
  
  void save(const char* filename) const;
  void load(const char* filename);
  void write(std::ostream& s) const;
  void read(std::istream& s);

};

#endif // LOSEFACE_MLPARRAY_H
