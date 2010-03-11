// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_VECTOR_H
#define LOSEFACE_VECTOR_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include "approx_eq.h"

class Matrix;

class Vector
{
public:
  typedef std::vector<double>::iterator iterator;
  typedef std::vector<double>::const_iterator const_iterator;

private:
  std::vector<double> m_data;	        // values of each element in the vector

public:
  iterator begin() { return m_data.begin(); }
  iterator end() { return m_data.end(); }
  const_iterator begin() const { return m_data.begin(); }
  const_iterator end() const { return m_data.end(); }

  Vector();
  explicit Vector(size_t n);
  Vector(const Vector& u);

  double* getRaw() { return &m_data[0]; }
  const double* getRaw() const { return &m_data[0]; }

  size_t size() const { return m_data.size(); }

  double magnitude() const;
  double mean() const;
  Vector& resize(size_t n);
  Vector& zero();
  double getMin() const;
  double getMax() const;
  size_t getMinPos() const;
  size_t getMaxPos() const;
  Matrix diagonalMatrix() const;

  Vector& operator=(const Vector& u);
  Vector& operator+=(const Vector& u);
  Vector& operator-=(const Vector& u);
  Vector& operator*=(double s);
  Vector& operator/=(double s);
  Vector operator*(double s) const;
  Vector operator/(double s) const;
  Vector operator+(const Vector& u) const;
  Vector operator-(const Vector& u) const;
  double operator*(const Vector& u) const;
  bool operator==(const Vector& u) const;
  bool operator!=(const Vector& u) const;

  inline double& operator()(size_t i) {
    assert(i >= 0 && i < size());
    return m_data[i];
  }

  inline const double& operator()(size_t i) const {
    assert(i >= 0 && i < size());
    return m_data[i];
  }

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////
  
  void save(const char* filename) const;
  void load(const char* filename);
  void write(std::ostream& s) const;
  void read(std::istream& s);

}; // class Vector

Vector operator*(double s, const Vector& u);
bool approx_eq(const Vector& u, const Vector& v, unsigned precision);

//////////////////////////////////////////////////////////////////////
// Text I/O
//////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& o, const Vector& v);
std::istream& operator>>(std::istream& s, Vector& v);

#endif // LOSEFACE_VECTOR_H
