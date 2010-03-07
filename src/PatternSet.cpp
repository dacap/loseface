// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
