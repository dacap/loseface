// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "PatternSet.h"

PatternSet::PatternSet()
{
}

PatternSet::PatternSet(const PatternSet& copy)
{
  operator=(copy);
}

PatternSet::~PatternSet()
{
  for (iterator it = begin(); it != end(); ++it)
    delete *it;
  m_set.clear();
}

PatternSet& PatternSet::operator=(const PatternSet& set)
{
  m_set.reserve(set.size());
  for (const_iterator it = set.begin(); it != set.end(); ++it)
    push_back(**it);
  return *this;
}

void PatternSet::push_back(const Pattern& p)
{
  m_set.push_back(new Pattern(p));
}

void PatternSet::shuffle()
{
  std::random_shuffle(begin(), end());
}
