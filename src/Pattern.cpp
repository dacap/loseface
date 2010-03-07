// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Pattern.h"

Pattern::Pattern()
{
}

Pattern::Pattern(int inputs, int outputs)
  : m_input(inputs)
  , m_output(outputs)
{
}

Pattern::Pattern(const Pattern& p)
  : m_input(p.m_input)
  , m_output(p.m_output)
{
}

Pattern::Pattern(const Vector& input,
		 const Vector& output)
  : m_input(input)
  , m_output(output)
{
}

Pattern& Pattern::operator=(const Pattern& p)
{
  m_input = p.m_input;
  m_output = p.m_output;
  return *this;
}
