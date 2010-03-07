// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Matrix.h"
#include "Vector.h"
#include "approx_eq.h"

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

Matrix::Matrix()
{
  m_rows = 1;
  m_cols = 1;
  m_data.resize(m_rows*m_cols);
}

Matrix::Matrix(size_t rows, size_t cols)
{
  assert(rows >= 1);
  assert(cols >= 1);
  m_rows = rows;
  m_cols = cols;
  m_data.resize(m_rows*m_cols);
}

Matrix::Matrix(const Matrix& A)
{
  m_rows = A.m_rows;
  m_cols = A.m_cols;
  m_data = A.m_data;
}

Matrix& Matrix::resize(size_t rows, size_t cols)
{
  assert(rows >= 1);
  assert(cols >= 1);

  if (m_rows != rows || m_cols != cols) {
    size_t old_rows = m_rows;
    size_t old_cols = m_cols;
    std::vector<double> old_data(m_data);

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

Matrix& Matrix::zero()
{
  std::fill(m_data.begin(), m_data.end(), double(0));
  return *this;
}

void Matrix::makeIdentity()
{
  zero();

  size_t k = m_rows < m_cols ? m_rows: m_cols;
  for (size_t i=0; i<k; ++i)
    operator()(i, i) = double(1);
}

double Matrix::getMin() const
{
  double min = m_data[0];

  size_t k = m_rows*m_cols;
  for (size_t i=1; i<k; ++i)
    if (min > m_data[i])
      min = m_data[i];

  return min;
}

double Matrix::getMax() const
{
  double max = m_data[0];
  size_t i, k = m_rows*m_cols;

  for (i=1; i<k; ++i)
    if (max < m_data[i])
      max = m_data[i];

  return max;
}

std::pair<size_t,size_t> Matrix::getMinPos() const
{
  std::pair<size_t,size_t> pos(0, 0);
  double min = m_data[0];
  size_t i, j;

  for (j=0; j<m_cols; ++j)
    for (i=0; i<m_rows; ++i)
      if (min > operator()(i, j)) {
	min = operator()(i, j);
	pos = std::make_pair(i, j);
      }

  return pos;
}

std::pair<size_t,size_t> Matrix::getMaxPos() const
{
  std::pair<size_t,size_t> pos(0, 0);
  double max = m_data[0];
  size_t i, j;

  for (j=0; j<m_cols; ++j)
    for (i=0; i<m_rows; ++i)
      if (max < operator()(i, j)) {
	max = operator()(i, j);
	pos = std::make_pair(i, j);
      }

  return pos;
}

void Matrix::getRow(size_t i, Vector& row) const
{
  row.resize(m_cols);
  for (size_t j=0; j<m_cols; ++j)
    row(j) = operator()(i, j);
}

void Matrix::getCol(size_t j, Vector& col) const
{
  col.resize(m_rows);
  for (size_t i=0; i<m_rows; ++i)
    col(i) = operator()(i, j);
}

Vector Matrix::getRow(size_t i) const
{
  Vector row(m_cols);
  getRow(i, row);
  return row;
}

Vector Matrix::getCol(size_t j) const
{
  Vector col(m_rows);
  getCol(j, col);
  return col;
}

Matrix& Matrix::setRow(size_t i, const Vector& u)
{
  assert(m_cols == u.size());

  for (size_t j=0; j<m_cols; ++j)
    operator()(i, j) = u(j);

  return *this;
}

Matrix& Matrix::setCol(size_t j, const Vector& u)
{
  assert(m_rows == u.size());

  for (size_t i=0; i<m_rows; ++i)
    operator()(i, j) = u(i);

  return *this;
}

Matrix& Matrix::addRow(size_t i, const Vector& u)
{
  assert(i <= m_rows);
  resize(m_rows+1, m_cols);

  if (i < m_rows)
    for (size_t k=m_rows-1; k>i; ++k)
      setRow(k, getRow(k-1));

  setRow(i, u);
  return *this;
}

Matrix& Matrix::addCol(size_t j, const Vector& u)
{
  assert(j <= m_cols);
  resize(m_rows, m_cols+1);

  if (j < m_cols)
    for (size_t k=m_cols-1; k>j; ++k)
      setCol(k, getCol(k-1));

  setCol(j, u);
  return *this;
}

void Matrix::meanRow(Vector& row) const
{
  row.resize(m_cols);
  row.zero();

  for (size_t j=0; j<m_cols; ++j) {
    for (size_t i=0; i<m_rows; ++i) {
      row(j) += operator()(i, j);
    }
    row(j) /= m_rows;
  }
}

void Matrix::meanCol(Vector& col) const
{
  col.resize(m_rows);
  col.zero();

  for (size_t i=0; i<m_rows; ++i) {
    for (size_t j=0; j<m_cols; ++j) {
      col(i) += operator()(i, j);
    }
    col(i) /= m_cols;
  }
}

Vector Matrix::meanRow() const
{
  Vector row(m_cols);
  meanRow(row);
  return row;
}

Vector Matrix::meanCol() const
{
  Vector col(m_rows);
  meanCol(col);
  return col;
}

void Matrix::getTranspose(Matrix& A) const
{
  A.resize(m_cols, m_rows);		// transpose dimensions MxN -> NxM
  for (size_t i=0; i<m_rows; ++i)
    for (size_t j=0; j<m_cols; ++j)
      A(j, i) = operator()(i, j);
}

Matrix Matrix::getTranspose() const
{
  Matrix A;
  getTranspose(A);
  return A;
}

Matrix& Matrix::operator=(const Matrix& A)
{
  m_rows = A.m_rows;
  m_cols = A.m_cols;
  m_data = A.m_data;
  return *this;
}

Matrix& Matrix::operator+=(const Matrix& B)
{
  assert(m_rows == B.m_rows);
  assert(m_cols == B.m_cols);
#if 1
  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    m_data[i] += B.m_data[i];
#else  // optimized
  double* ptr = m_data;
  double* B_ptr = B.m_data;

  size_t k = m*n;
  for (size_t i=0; i<k; ++i)
    *(ptr++) += *(B_ptr++);
#endif
  return *this;
}

Matrix& Matrix::operator-=(const Matrix& B)
{
  assert(m_rows == B.m_rows);
  assert(m_cols == B.m_cols);
#if 1
  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    m_data[i] -= B.m_data[i];
#else  // optimized
  double* ptr = m_data;
  double* B_ptr = B.m_data;

  size_t k = m*n;
  for (size_t i=0; i<k; ++i)
    *(ptr++) -= *(B_ptr++);
#endif

  return *this;
}

Matrix& Matrix::operator*=(double s)
{
#if 1
  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    m_data[i] *= s;
#else  // optimized
  double* ptr = m_data;

  size_t k = m_rows*n;
  for (size_t i=0; i<k; ++i)
    *(ptr++) *= s;
#endif

  return *this;
}

Matrix& Matrix::operator/=(double s)
{
  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    m_data[i] /= s;
  return *this;
}

Matrix Matrix::operator+(const Matrix& B) const
{
  assert(m_rows == B.m_rows);
  assert(m_cols == B.m_cols);

  Matrix C(m_rows, m_cols);

  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    C.m_data[i] = m_data[i] + B.m_data[i];

  return C;
}

Matrix Matrix::operator-(const Matrix& B) const
{
  assert(m_rows == B.m_rows);
  assert(m_cols == B.m_cols);

  Matrix C(m_rows, m_cols);

  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    C.m_data[i] = m_data[i] - B.m_data[i];

  return C;
}

Matrix Matrix::operator*(double s) const
{
  Matrix B(rows(), cols());
  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    B.m_data[i] = m_data[i] * s;

  return B;
}

Matrix Matrix::operator*(const Matrix& B) const
{
  assert(cols() == B.rows());

  Matrix C(rows(), B.cols());
  size_t i, j, k;
  double result;

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

Matrix& Matrix::operator*=(const Matrix& B)
{
  *this = *this * B;
  return *this;
}

void Matrix::multiply(const Vector& u, Vector& v) const
{
  assert(m_cols == u.size());

  v.resize(m_rows);

#if 1
  double r;
  size_t i, j;

  for (i=0; i<m_rows; ++i) {
    r = 0.0;
    for (j=0; j<m_cols; ++j)
      r += operator()(i, j) * u(j);
    v(i) = r;
  }
#else // optimized
  double r;
  size_t i, j;

  double* ptr;
  double* u_ptr;
  double* v_ptr = v.m_data;

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
  
Vector Matrix::operator*(const Vector& u) const 
{
  Vector v(m_rows);
  multiply(u, v);
  return v;
}

bool Matrix::operator==(const Matrix& B) const
{
  if (m_rows != B.m_rows || m_cols != B.m_cols)
    return false;

  size_t k = m_rows*m_cols;
  for (size_t i=0; i<k; ++i)
    if (m_data[i] != B.m_data[i])
      return false;

  return true;
}

bool Matrix::operator!=(const Matrix& B) const
{
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
Matrix Matrix::dist(const Matrix& B) const
{
  assert(m_cols == B.m_rows);

  Matrix C(m_rows, B.m_cols);
  size_t i, j, k;
  double d, d2;

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

Vector Matrix::distEachRow(const Vector& column_vector) const
{
  assert(m_cols == column_vector.size());

  Vector w(m_rows);
  size_t i, k;
  double d, d2;

  for (i=0; i<m_rows; ++i) {
    d = 0.0;

    for (k=0; k<m_cols; ++k) {
      d2 = operator()(i, k) - column_vector(k);
      d += d2*d2;
    }

    w(i) = static_cast<double>(std::sqrt(static_cast<double>(d)));
  }

  return w;
}

void Matrix::eig_sym(Vector& eigenvalues,
		     Matrix& eigenvectors) const
{
  assert(m_rows == m_cols);

  int n = m_cols;
  int lda = m_rows;
  int lwork = 4*n;
  int info = 1;
  char jobz[] = { 'N', 0 };
  char uplo[] = { 'U', 0 };

  Vector work(lwork);

  eigenvectors = *this;
  eigenvalues.resize(n);
  eigenvalues.zero();

  dsyev_(jobz, uplo, &n,
	 eigenvectors.getRaw(), &lda,
	 eigenvalues.getRaw(), work.getRaw(), &lwork, &info);

  if (info < 0) {
    char buf[1024];
    std::sprintf(buf, "DSYEV: argument info=%d is invalid.", info);
    throw std::invalid_argument(std::string(buf));
  }
  else if (info > 0)
    throw std::runtime_error("Eigenvalues calculation does not converge.");
}

#if 0
void Matrix::eig(Vector& eigenvalues,
		 Matrix& eigenvectors) const
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
    std::sprintf(buf, "DSYEV: argument info=%d is invalid.", info);
    throw std::invalid_argument(std::string(buf));
  }

  if (info > 0)
    throw std::runtime_error("Eigenvalues calculation does not converge.");

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
  
void Matrix::save(const char* filename) const
{
  std::ofstream f(filename, std::ios::binary);
  write(f);
}

void Matrix::load(const char* filename)
{
  std::ifstream f(filename, std::ios::binary);
  read(f);
}

void Matrix::write(std::ostream& s) const
{
  s.write((char*)&m_rows, sizeof(size_t));
  s.write((char*)&m_cols, sizeof(size_t));
  s.write((char*)getRaw(), sizeof(double)*m_rows*m_cols);
}

void Matrix::read(std::istream& s)
{
  size_t m, n;
  s.read((char*)&m, sizeof(size_t));
  s.read((char*)&n, sizeof(size_t));
  resize(m, n);
  s.read((char*)getRaw(), sizeof(double)*m*n);
}

bool approx_eq(const Matrix& A, const Matrix& B, unsigned precision)
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

//////////////////////////////////////////////////////////////////////
// Text I/O
//////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& s, const Matrix& A)
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

std::istream& operator>>(std::istream& s, Matrix& A)
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
