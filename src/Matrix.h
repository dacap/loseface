// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LOSEFACE_MATRIX_H
#define LOSEFACE_MATRIX_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>

#include "approx_eq.h"
#include "Vector.h"

// LAPACK
extern "C" {
  extern int dsyev_(char *jobz, char *uplo, int *n, double *a,
		    int *lda, double *w, double *work, int *lwork, 
		    int *info);
  extern int dgeev_(char *jobvl, char *jobvr, int *n, double *a,
		    int *lda, double *wr, double *wi, double *vl, 
		    int *ldvl, double *vr, int *ldvr, double *work, 
		    int *lwork, int *info);
}

template<typename T>
class Matrix
{
  friend class Vector<T>;

  size_t m_rows;
  size_t m_cols;
  std::vector<T> m_data;

public:

  Matrix() {
    m_rows = 1;
    m_cols = 1;
    m_data.resize(m_rows*m_cols);
  }

  Matrix(size_t rows, size_t cols) {
    assert(rows >= 1);
    assert(cols >= 1);
    m_rows = rows;
    m_cols = cols;
    m_data.resize(m_rows*m_cols);
  }

  Matrix(const Matrix& A) {
    m_rows = A.m_rows;
    m_cols = A.m_cols;
    m_data = A.m_data;
  }

  T* getRaw() { return &m_data[0]; }
  const T* getRaw() const { return &m_data[0]; }

  size_t rows() const { return m_rows; }
  size_t cols() const { return m_cols; }

  Matrix& resize(size_t rows, size_t cols) {
    assert(rows >= 1);
    assert(cols >= 1);

    if (m_rows != rows || m_cols != cols) {
      size_t old_rows = m_rows;
      size_t old_cols = m_cols;
      std::vector<T> old_data(m_data);

      m_rows = rows;
      m_cols = cols;
      m_data.resize(m_rows*m_cols);
      zero();

      size_t min_rows = old_rows < m_rows ? old_rows: m_rows;
      size_t min_cols = old_cols < m_cols ? old_cols: m_cols;

      for (size_t j=0; j<min_cols; ++j)
	for (size_t i=0; i<min_rows; ++i)
	  operator()(i, j) = old_data[i+j*old_rows];
    }
    return *this;
  }

  Matrix& zero() {
    std::fill(m_data.begin(), m_data.end(), T(0));
    return *this;
  }

  void makeIdentity() {
    zero();

    size_t k = m_rows < m_cols ? m_rows: m_cols;
    for (size_t i=0; i<k; ++i)
      operator()(i, i) = T(1);
  }

  bool isSquare() const {
    return m_rows == m_cols;
  }

  T getMin() const {
    T min = m_data[0];

    size_t k = m_rows*m_cols;
    for (size_t i=1; i<k; ++i)
      if (min > m_data[i])
	min = m_data[i];

    return min;
  }

  T getMax() const {
    T max = m_data[0];
    size_t i, k = m_rows*m_cols;

    for (i=1; i<k; ++i)
      if (max < m_data[i])
	max = m_data[i];

    return max;
  }

  std::pair<size_t,size_t> getMinPos() const {
    std::pair<size_t,size_t> pos(0, 0);
    T min = m_data[0];
    size_t i, j;

    for (j=0; j<m_cols; ++j)
      for (i=0; i<m_rows; ++i)
	if (min > operator()(i, j)) {
	  min = operator()(i, j);
	  pos = std::make_pair(i, j);
	}

    return pos;
  }

  std::pair<size_t,size_t> getMaxPos() const {
    std::pair<size_t,size_t> pos(0, 0);
    T max = m_data[0];
    size_t i, j;

    for (j=0; j<m_cols; ++j)
      for (i=0; i<m_rows; ++i)
	if (max < operator()(i, j)) {
	  max = operator()(i, j);
	  pos = std::make_pair(i, j);
	}

    return pos;
  }

  void getRow(size_t i, Vector<T>& row) const {
    row.resize(m_cols);
    for (size_t j=0; j<m_cols; ++j)
      row(j) = operator()(i, j);
  }

  void getCol(size_t j, Vector<T>& col) const {
    col.resize(m_rows);
    for (size_t i=0; i<m_rows; ++i)
      col(i) = operator()(i, j);
  }

  Vector<T> getRow(size_t i) const {
    Vector<T> row(m_cols);
    getRow(i, row);
    return row;
  }

  Vector<T> getCol(size_t j) const {
    Vector<T> col(m_rows);
    getCol(j, col);
    return col;
  }

  Matrix& setRow(size_t i, const Vector<T>& u) {
    assert(m_cols == u.size());

    for (size_t j=0; j<m_cols; ++j)
      operator()(i, j) = u(j);

    return *this;
  }

  Matrix& setCol(size_t j, const Vector<T>& u) {
    assert(m_rows == u.size());

    for (size_t i=0; i<m_rows; ++i)
      operator()(i, j) = u(i);

    return *this;
  }

  Matrix& addRow(size_t i, const Vector<T>& u) {
    assert(i <= m_rows);
    resize(m_rows+1, m_cols);

    if (i < m_rows)
      for (size_t k=m_rows-1; k>i; ++k)
	setRow(k, getRow(k-1));

    setRow(i, u);
    return *this;
  }

  Matrix& addCol(size_t j, const Vector<T>& u) {
    assert(j <= m_cols);
    resize(m_rows, m_cols+1);

    if (j < m_cols)
      for (size_t k=m_cols-1; k>j; ++k)
	setCol(k, getCol(k-1));

    setCol(j, u);
    return *this;
  }

  void meanRow(Vector<T>& row) const {
    row.resize(m_cols);
    row.zero();

    for (size_t j=0; j<m_cols; ++j) {
      for (size_t i=0; i<m_rows; ++i) {
	row(j) += operator()(i, j);
      }
      row(j) /= m_rows;
    }
  }

  void meanCol(Vector<T>& col) const {
    col.resize(m_rows);
    col.zero();

    for (size_t i=0; i<m_rows; ++i) {
      for (size_t j=0; j<m_cols; ++j) {
	col(i) += operator()(i, j);
      }
      col(i) /= m_cols;
    }
  }

  Vector<T> meanRow() const {
    Vector<T> row(m_cols);
    meanRow(row);
    return row;
  }

  Vector<T> meanCol() const {
    Vector<T> col(m_rows);
    meanCol(col);
    return col;
  }

  void getTranspose(Matrix& A) const {
    A.resize(m_cols, m_rows);		// transpose dimensions MxN -> NxM
    for (size_t i=0; i<m_rows; ++i)
      for (size_t j=0; j<m_cols; ++j)
	A(j, i) = operator()(i, j);
  }

  Matrix getTranspose() const {
    Matrix A;
    getTranspose(A);
    return A;
  }

  Matrix& operator=(const Matrix& A) {
    m_rows = A.m_rows;
    m_cols = A.m_cols;
    m_data = A.m_data;
    return *this;
  }

  Matrix& operator+=(const Matrix& B) {
    assert(m_rows == B.m_rows);
    assert(m_cols == B.m_cols);
#if 1
    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      m_data[i] += B.m_data[i];
#else  // optimized
    T* ptr = m_data;
    T* B_ptr = B.m_data;

    size_t k = m*n;
    for (size_t i=0; i<k; ++i)
      *(ptr++) += *(B_ptr++);
#endif
    return *this;
  }

  Matrix& operator-=(const Matrix& B) {
    assert(m_rows == B.m_rows);
    assert(m_cols == B.m_cols);
#if 1
    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      m_data[i] -= B.m_data[i];
#else  // optimized
    T* ptr = m_data;
    T* B_ptr = B.m_data;

    size_t k = m*n;
    for (size_t i=0; i<k; ++i)
      *(ptr++) -= *(B_ptr++);
#endif

    return *this;
  }

  Matrix& operator*=(T s) {
#if 1
    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      m_data[i] *= s;
#else  // optimized
    T* ptr = m_data;

    size_t k = m_rows*n;
    for (size_t i=0; i<k; ++i)
      *(ptr++) *= s;
#endif

    return *this;
  }

  Matrix operator+(const Matrix& B) const {
    assert(m_rows == B.m_rows);
    assert(m_cols == B.m_cols);

    Matrix C(m_rows, m_cols);

    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      C.m_data[i] = m_data[i] + B.m_data[i];

    return C;
  }

  Matrix operator-(const Matrix& B) const {
    assert(m_rows == B.m_rows);
    assert(m_cols == B.m_cols);

    Matrix C(m_rows, m_cols);

    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      C.m_data[i] = m_data[i] - B.m_data[i];

    return C;
  }

  Matrix operator*(T s) const {
    Matrix B(rows(), cols());
    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      B.m_data[i] = m_data[i] * s;

    return B;
  }

  Matrix operator*(const Matrix& B) const {
    assert(cols() == B.rows());

    Matrix C(rows(), B.cols());
    size_t i, j, k;
    T result;

    for (j=0; j<C.cols(); ++j) {
      for (i=0; i<C.rows(); ++i) {
	result = 0.0;

	for (k=0; k<cols(); ++k)
	  result += operator()(i, k) * B(k, j);

	C(i, j) = result;
      }
    }

    return C;
  }

  Matrix& operator*=(const Matrix& B) {
    *this = *this * B;
    return *this;
  }

  void multiply(const Vector<T>& u, Vector<T>& v) const {
    assert(m_cols == u.size());

    v.resize(m_rows);

#if 1
    T r;
    size_t i, j;

    for (i=0; i<m_rows; ++i) {
      r = 0.0;
      for (j=0; j<m_cols; ++j)
	r += operator()(i, j) * u(j);
      v(i) = r;
    }
#else // optimized
    T r;
    size_t i, j;

    T* ptr;
    T* u_ptr;
    T* v_ptr = v.m_data;

    for (i=0; i<m_rows; ++i) {
      r = 0.0;

      ptr = m_data+i;
      u_ptr = u.m_data;

      for (j=0; j<n; ++j) {
	r += (*ptr) * (*u_ptr);
	ptr += m_rows;
	++u_ptr;
      }

      *(v_ptr++) = r;
    }
#endif
  }
  
  Vector<T> operator*(const Vector<T>& u) const {
    Vector<T> v(m_rows);
    multiply(u, v);
    return v;
  }

  bool operator==(const Matrix& B) const {
    if (m_rows != B.m_rows || m_cols != B.m_cols)
      return false;

    size_t k = m_rows*m_cols;
    for (size_t i=0; i<k; ++i)
      if (m_data[i] != B.m_data[i])
	return false;

    return true;
  }

  bool operator!=(const Matrix& B) const {
    return !operator==(B);
  }

  // Euclidean distance
  //  
  //  A = [ 1 2		(A = this)
  //        3 4 ]
  //  B = [ 5
  //        6 ]
  //  C = A.dist(B) = [6.4 7.8
  //                   3.6 5.0]
  //  
  //          | 5    6
  //          | 7    8
  //  --------+----------
  //      1 2 | 6.4  7.8
  //      3 4 | 3.6  5.0
  // 
  Matrix dist(const Matrix& B) const
  {
    assert(m_cols == B.m_rows);

    Matrix C(m_rows, B.m_cols);
    size_t i, j, k;
    T d, d2;

    for (j=0; j<C.m_cols; ++j) {
      for (i=0; i<C.m_rows; ++i) {
	d = 0.0;

	for (k=0; k<m_cols; ++k) {
	  d2 = operator()(i, k) - B(k, j);
	  d += d2*d2;
	}

	C(i, j) = std::sqrt(d);
      }
    }

    return C;
  }

  Vector<T> distEachRow(const Vector<T>& column_vector) const
  {
    assert(m_cols == column_vector.size());

    Vector<T> w(m_rows);
    size_t i, k;
    T d, d2;

    for (i=0; i<m_rows; ++i) {
      d = 0.0;

      for (k=0; k<m_cols; ++k) {
	d2 = operator()(i, k) - column_vector(k);
	d += d2*d2;
      }

      w(i) = static_cast<T>(std::sqrt(static_cast<double>(d)));
    }

    return w;
  }

  inline T& operator()(size_t i, size_t j) {
    assert(i >= 0 && i < m_rows);
    assert(j >= 0 && j < m_cols);
    return m_data[i+j*m_rows];
  }

  inline const T& operator()(size_t i, size_t j) const {
    assert(i >= 0 && i < m_rows);
    assert(j >= 0 && j < m_cols);
    return m_data[i+j*m_rows];
  }

  void eig_sym(Vector<T>& eigenvalues,
	       Matrix<T>& eigenvectors) const
  {
    assert(m_rows == m_cols);

    int n = m_cols;
    int lda = m_rows;
    int lwork = 4*n;
    int info = 1;
    char jobz[] = { 'N', 0 };
    char uplo[] = { 'U', 0 };

    Vector<double> work(lwork);

    eigenvectors = *this;
    eigenvalues.resize(n);
    eigenvalues.zero();

    dsyev_(jobz, uplo, &n,
	   eigenvectors.getRaw(), &lda,
	   eigenvalues.getRaw(), work.getRaw(), &lwork, &info);

    if (info < 0) {
      char buf[1024];
      std::sprintf(buf, "DSYEV: El argumento %d es inválido.", -info);
      throw std::invalid_argument(std::string(buf));
    }
    else if (info > 0)
      throw std::runtime_error("El cálculo de los eigenvalores no converge.");
  }

#if 0
  void eig(Vector<T>& eigenvalues,
	   Matrix<T>& eigenvectors) const
  {
    assert(m_rows == m_cols);

    int n = n_cols;
    int lda = m_rows;
    int ldvl = 1;
    int ldvr = n;
    int lwork = 4*n;

    double* A = new double[m_rows*m_cols];
    double* wr = new double[n];
    double* wi = new double[n];
    double* vl = new double[n*ldvl];
    double* vr = new double[n*ldvr];
    double* work = new double[lwork];
    int info = 1;

    memcpy(A, m_data, sizeof(double[m*n]));
    memset(vr, 0, sizeof(double[n*ldvr]));

    dgeev_("N", "V",
	   &n, A, &lda,
	   wr, wi,
	   vl, &ldvl,
	   vr, &ldvr,
	   work, &lwork,
	   &info);

    if (info < 0) {
      char buf[1024];
      std::sprintf(buf, "DGEEV: El argumento %d es inválido.", -info);
      throw std::invalid_argument(std::string(buf));
    }

    if (info > 0)
      throw std::runtime_error("El cálculo de los eigenvalores no converge.");

    // TODO complex eigenvalues

    eigenvalues.resize(n);
    eigenvectors.resize(n, ldvr);

    memcpy(eigenvalues.v, wr, sizeof(double[n]));
    memcpy(eigenvectors.v, vr, sizeof(double[n*ldvr]));

    delete[] A;
    delete[] wr;
    delete[] wi;
    delete[] vl;
    delete[] vr;
    delete[] work;
  }
#endif

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////
  
  void save(const char* filename) const
  {
    std::ofstream f(filename, std::ios::binary);
    write(f);
  }

  void load(const char* filename)
  {
    std::ifstream f(filename, std::ios::binary);
    read(f);
  }

  void write(std::ostream& s) const
  {
    s.write((char*)&m_rows, sizeof(size_t));
    s.write((char*)&m_cols, sizeof(size_t));
    s.write((char*)getRaw(), sizeof(T)*m_rows*m_cols);
  }

  void read(std::istream& s)
  {
    size_t m, n;
    s.read((char*)&m, sizeof(size_t));
    s.read((char*)&n, sizeof(size_t));
    resize(m, n);
    s.read((char*)getRaw(), sizeof(T)*m*n);
  }

}; // class Matrix

template<typename T>
bool approx_eq(const Matrix<T>& A, const Matrix<T>& B, unsigned precision)
{
  if (A.rows() != B.rows() ||
      A.cols() != B.cols())
    return false;
  
  size_t i, j;
  for (j=0; j<A.cols(); ++j) {
    for (i=0; i<A.rows(); i++) {
      if (!approx_eq(A(i, j), B(i, j), precision))
	return false;
    }
  }
  return true;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const Matrix<T>& A)
{
  s.precision(16);

  s << "(" << A.rows() << "x" << A.cols() << ")";

  size_t i, j;
  for (i=0; i<A.rows(); ++i) {
    if (i == 0)
      s << "[ ";
    else
      s << "  ";

    for (j=0; j<A.cols(); ++j)
      s << A(i, j) << " ";

    if (i < A.rows()-1)
      s << std::endl;
    else
      s << "]";
  }

  return s;
}

template<typename T>
std::istream& operator>>(std::istream& s, Matrix<T>& A)
{
  char c = 0;

  s >> c;
  if (c != '(') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  int rows, cols;
  s >> rows >> c >> cols;
  if (c != 'x') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  s >> c;
  if (c != ')') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  s >> c;
  if (c != '[') {
    s.clear(std::ios_base::badbit);
    return s;
  }

  A.resize(rows, cols);
  for (int i=0; i<rows; ++i)
    for (int j=0; j<cols; ++j)
      s >> A(i, j);

  s >> c;
  if (c != ']') s.clear(std::ios_base::badbit);

  return s;
}

#endif // LOSEFACE_MATRIX_H
