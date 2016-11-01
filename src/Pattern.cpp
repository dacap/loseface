// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
