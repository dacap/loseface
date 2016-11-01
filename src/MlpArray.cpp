// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "MlpArray.h"

MlpArray::MlpArray()
{
  m_outputs = 0;
}

MlpArray::MlpArray(const MlpArray& net)
  : m_nets(net.m_nets)
  , m_outputs(net.m_outputs)
{
}

MlpArray& MlpArray::operator=(const MlpArray& net)
{
  m_nets = net.m_nets;
  return *this;
}

/// Adds a new network to the array.
///
/// @param net
///   The new neural network to be copied and added to the array. This network
///   must contain the same quantity of inputs as all other networks in the array.
///
void MlpArray::add(const Mlp& net)
{
  assert(m_nets.empty() || net.getInputs() == getInputs());

  m_nets.push_back(net);
  m_outputs += net.getOutputs();
}

void MlpArray::recall(const Vector& input, Vector& output) const
{
  assert(!m_nets.empty());

  Vector hidden, it_output;
  size_t i = 0;

  output = createOutput();

  for (Nets::const_iterator
	 it = m_nets.begin(); it != m_nets.end(); ++it) {
    it->recall(input, hidden, it_output);

    for (size_t j=0; j<it_output.size(); ++j, ++i)
      output(i) = it_output(j);
  }
}

//////////////////////////////////////////////////////////////////////
// Binary I/O
//////////////////////////////////////////////////////////////////////

void MlpArray::save(const char* filename) const
{
  std::ofstream f(filename, std::ios::binary);
  write(f);
}

void MlpArray::load(const char* filename)
{
  std::ifstream f(filename, std::ios::binary);
  read(f);
}

void MlpArray::write(std::ostream& s) const
{
  size_t n = m_nets.size();
  s.write((char*)&n, sizeof(size_t));

  for (Nets::const_iterator
	 it = m_nets.begin(); it != m_nets.end(); ++it) {
    it->write(s);
  }
}

void MlpArray::read(std::istream& s)
{
  size_t n;
  s.read((char*)&n, sizeof(size_t));

  for (size_t c=0; c<n; ++c) {
    Mlp n;
    n.read(s);
    add(n);
  }
}
