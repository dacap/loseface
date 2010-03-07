// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Vector.h"
#include "Matrix.h"
#include "approx_eq.h"

Vector::Vector()
{
  m_data.resize(1);
}

Vector::Vector(size_t n)
{
  assert(n >= 1);
  m_data.resize(n);
}

Vector::Vector(const Vector& u)
{
  m_data = u.m_data;
}

double Vector::magnitude() const
{
  double sq_sum = 0.0;

  for (size_t i=0; i<size(); ++i)
    sq_sum += m_data[i]*m_data[i];

  return std::sqrt(sq_sum);
}

double Vector::mean() const
{
  double result = 0.0;

  for (size_t i=0; i<size(); ++i)
    result += m_data[i];

  return result / double(size());
}

Vector& Vector::resize(size_t n)
{
  assert(n >= 1);
  m_data.resize(n);
  return *this;
}

Vector& Vector::zero()
{
  std::fill(m_data.begin(), m_data.end(), double(0));
  return *this;
}

double Vector::getMin() const
{
  double a = m_data[0];

  for (size_t i=1; i<size(); ++i)
    if (a > m_data[i])
      a = m_data[i];

  return a;
}

double Vector::getMax() const
{
  double a = m_data[0];

  for (size_t i=1; i<size(); ++i)
    if (a < m_data[i])
      a = m_data[i];

  return a;
}

size_t Vector::getMinPos() const
{
  size_t k = 0;
  double a = m_data[0];

  for (size_t i=1; i<size(); ++i)
    if (a > m_data[i])
      a = m_data[k=i];

  return k;
}

size_t Vector::getMaxPos() const
{
  size_t k = 0;
  double a = m_data[0];

  for (size_t i=1; i<size(); ++i)
    if (a < m_data[i])
      a = m_data[k=i];

  return k;
}

Matrix Vector::diagonalMatrix() const
{
  Matrix A(size(), size());
  A.zero();

  for (size_t i=0; i<size(); ++i)
    A(i, i) = m_data[i];

  return A;
}

Vector& Vector::operator=(const Vector& u)
{
  m_data = u.m_data;
  return *this;
}

Vector& Vector::operator+=(const Vector& u)
{
  assert(size() == u.size());

  for (size_t i=0; i<size(); ++i)
    m_data[i] += u.m_data[i];

  return *this;
}

Vector& Vector::operator-=(const Vector& u)
{
  assert(size() == u.size());

  for (size_t i=0; i<size(); ++i)
    m_data[i] -= u.m_data[i];

  return *this;
}

Vector& Vector::operator*=(double s)
{
  for (size_t i=0; i<size(); ++i)
    m_data[i] *= s;

  return *this;
}

Vector& Vector::operator/=(double s)
{
  for (size_t i=0; i<size(); ++i)
    m_data[i] /= s;

  return *this;
}

Vector Vector::operator*(double s) const
{
  Vector w(size());

  for (size_t i=0; i<size(); ++i)
    w.m_data[i] = s * m_data[i];

  return w;
}

Vector Vector::operator/(double s) const
{
  Vector w(size());

  for (size_t i=0; i<size(); ++i)
    w.m_data[i] = m_data[i] / s;

  return w;
}

Vector Vector::operator+(const Vector& u) const
{
  assert(size() == u.size());
  Vector w(size());

  for (size_t i=0; i<size(); ++i)
    w.m_data[i] = m_data[i] + u.m_data[i];

  return w;
}

Vector Vector::operator-(const Vector& u) const
{
  assert(size() == u.size());
  Vector w(size());

  for (size_t i=0; i<size(); ++i)
    w.m_data[i] = m_data[i] - u.m_data[i];

  return w;
}

double Vector::operator*(const Vector& u) const
{
  assert(size() == u.size());
  double dot_product = 0.0;

  for (size_t i=0; i<size(); ++i)
    dot_product += m_data[i] * u.m_data[i];

  return dot_product;
}

bool Vector::operator==(const Vector& u) const
{
  if (size() != u.size())
    return false;

  for (size_t i=0; i<size(); ++i)
    if (m_data[i] != u.m_data[i])
      return false;

  return true;
}

bool Vector::operator!=(const Vector& u) const
{
  return !operator==(u);
}

//////////////////////////////////////////////////////////////////////
// Binary I/O
//////////////////////////////////////////////////////////////////////
  
void Vector::save(const char* filename) const
{
  std::ofstream f(filename, std::ios::binary);
  write(f);
}

void Vector::load(const char* filename)
{
  std::ifstream f(filename, std::ios::binary);
  read(f);
}

void Vector::write(std::ostream& s) const
{
  size_t n = size();
  s.write((char*)&n, sizeof(size_t));
  s.write((char*)getRaw(), sizeof(double)*n);
}

void Vector::read(std::istream& s)
{
  size_t n;
  s.read((char*)&n, sizeof(size_t));
  resize(n);
  s.read((char*)getRaw(), sizeof(double)*n);
}

Vector operator*(double s, const Vector& u)
{
  return u.operator*(s);
}

bool approx_eq(const Vector& u, const Vector& v, unsigned precision)
{
  if (u.size() != v.size())
    return false;

  for (size_t i=0; i<u.size(); ++i) {
    if (!approx_eq(v(i), u(i), precision))
      return false;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////
// Text I/O
//////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& o, const Vector& v)
{
  o.precision(16);

  o << "(" << v.size() << ")";
  o << "[ ";

  for (size_t i=0; i<v.size(); ++i)
    o << v(i) << " ";

  o << "]";

  return o;
}

std::istream& operator>>(std::istream& s, Vector& v)
{
  char c = 0;
  s >> c;
  if (c != '(') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  int size = 0;
  s >> size >> c;
  if (c != ')') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  s >> c;
  if (c != '[') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  double elem;
  int i = 0;

  v.resize(size);
  while (s >> elem)
    v(i++) = elem;

  s >> c;
  if (c != ']')
    s.clear(std::ios_base::badbit);

  return s;
}
