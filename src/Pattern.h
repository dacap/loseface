// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_PATTERN_H
#define LOSEFACE_PATTERN_H

#include "Vector.h"

/// A pair of input/output vectors.
///
class Pattern
{
  Vector m_input;
  Vector m_output;

public:
  Pattern();
  Pattern(int inputs, int outputs);
  Pattern(const Pattern& p);
  Pattern(const Vector& input,
	  const Vector& output);

  Pattern& operator=(const Pattern& p);

  const Vector& getInput() const { return m_input; }
  const Vector& getOutput() const { return m_output; }
  void setInput(const Vector& input) { m_input = input; }
  void setOutput(const Vector& output) { m_output = output; }

  void resizeInput(size_t n) { m_input.resize(n); }
  void resizeOutput(size_t n) { m_output.resize(n); }

  double getInput(size_t index) const { return m_input(index); }
  double getOutput(size_t index) const { return m_output(index); }
  void setInput(size_t index, double value) { m_input(index) = value; }
  void setOutput(size_t index, double value) { m_output(index) = value; }
};

#endif // LOSEFACE_PATTERN_H
