// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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

class Matrix
{
  friend class Vector;

  size_t m_rows;
  size_t m_cols;
  std::vector<double> m_data;

public:

  Matrix();
  Matrix(size_t rows, size_t cols);
  Matrix(const Matrix& A);

  double* getRaw() { return &m_data[0]; }
  const double* getRaw() const { return &m_data[0]; }

  size_t rows() const { return m_rows; }
  size_t cols() const { return m_cols; }
  bool isSquare() const { return m_rows == m_cols; }

  Matrix& resize(size_t rows, size_t cols);
  Matrix& zero();
  void makeIdentity();
  double getMin() const;
  double getMax() const;

  std::pair<size_t,size_t> getMinPos() const;
  std::pair<size_t,size_t> getMaxPos() const;

  void getRow(size_t i, Vector& row) const;
  void getCol(size_t j, Vector& col) const;
  Vector getRow(size_t i) const;
  Vector getCol(size_t j) const;
  Matrix& setRow(size_t i, const Vector& u);
  Matrix& setCol(size_t j, const Vector& u);
  Matrix& addRow(size_t i, const Vector& u);
  Matrix& addCol(size_t j, const Vector& u);
  void meanRow(Vector& row) const;
  void meanCol(Vector& col) const;
  Vector meanRow() const;
  Vector meanCol() const;
  void getTranspose(Matrix& A) const;
  Matrix getTranspose() const;

  Matrix& operator=(const Matrix& A);
  Matrix& operator+=(const Matrix& B);
  Matrix& operator-=(const Matrix& B);
  Matrix& operator*=(double s);
  Matrix& operator/=(double s);
  Matrix operator+(const Matrix& B) const;
  Matrix operator-(const Matrix& B) const;
  Matrix operator*(double s) const;
  Matrix operator*(const Matrix& B) const;
  Matrix& operator*=(const Matrix& B);
  void multiply(const Vector& u, Vector& v) const;
  Vector operator*(const Vector& u) const;
  bool operator==(const Matrix& B) const;
  bool operator!=(const Matrix& B) const;

  Matrix dist(const Matrix& B) const;
  Vector distEachRow(const Vector& column_vector) const;

  inline double& operator()(size_t i, size_t j) {
    assert(i >= 0 && i < m_rows);
    assert(j >= 0 && j < m_cols);
    return m_data[i+j*m_rows];
  }

  inline const double& operator()(size_t i, size_t j) const {
    assert(i >= 0 && i < m_rows);
    assert(j >= 0 && j < m_cols);
    return m_data[i+j*m_rows];
  }

  void eig_sym(Vector& eigenvalues,
	       Matrix& eigenvectors) const;

  //////////////////////////////////////////////////////////////////////
  // Binary I/O
  //////////////////////////////////////////////////////////////////////

  void save(const char* filename) const;
  void load(const char* filename);
  void write(std::ostream& s) const;
  void read(std::istream& s);

}; // class Matrix

bool approx_eq(const Matrix& A, const Matrix& B, unsigned precision);

//////////////////////////////////////////////////////////////////////
// Text I/O
//////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& s, const Matrix& A);
std::istream& operator>>(std::istream& s, Matrix& A);

#endif // LOSEFACE_MATRIX_H
