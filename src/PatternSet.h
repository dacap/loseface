// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_PATTERNSET_H
#define LOSEFACE_PATTERNSET_H

#include <vector>
#include "Pattern.h"

class PatternSet
{
public:
  typedef std::vector<Pattern*> Patterns;
  typedef Patterns::iterator iterator;
  typedef Patterns::const_iterator const_iterator;

private:
  Patterns m_set;

public:
  iterator begin() { return m_set.begin(); }
  iterator end() { return m_set.end(); }
  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }

  bool empty() const { return m_set.empty(); }
  size_t size() const { return m_set.size(); }

  PatternSet();
  PatternSet(const PatternSet& copy);
  ~PatternSet();

  PatternSet& operator=(const PatternSet& set);

  void push_back(const Pattern& p);
  void shuffle();

  Pattern& operator[](size_t index) {
    return *m_set[index];
  }

  const Pattern& operator[](size_t index) const {
    return *m_set[index];
  }

};

#endif // LOSEFACE_PATTERNSET_H
