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

#include <cassert>
#include <sstream>

#define VECTOR_WITH_IO
#define MATRIX_WITH_IO

#include "Matrix.h"
#include "Vector.h"
#include "Random.h"

using namespace std;

void test_vector_index()
{
  // Create a vector of 3 elements
  Vector v(3);
  assert(v.size() == 3);

  // Create an array of 3 elements with some value
  double a[3];
  for (size_t i=0; i<3; ++i)
    a[i] = Random::getReal();

  // Check if get/set elements by index is working
  for (size_t i=0; i<3; ++i) {
    v(i) = a[i];			// set
    assert(approx_eq(v(i), a[i], 10));	// get
  }

  // Resize to two elements
  v.resize(2);
  assert(v.size() == 2);
  for (size_t i=0; i<2; ++i)
    assert(approx_eq(v(i), a[i], 10)); // Elements should not be modified

  // Resize to four elements, the new elements should be zero
  v.resize(4);
  assert(approx_eq(v(2), double(0.0), 10));
  assert(approx_eq(v(3), double(0.0), 10));
}

void test_vector_equal()
{
  Vector v(3), u(3);

  for (size_t i=0; i<3; ++i)
    v(i) = u(i) = Random::getReal();

  assert(v == u);

  u(2) = -v(2);
  assert(v != u);

  u.resize(2);
  assert(v != u);		// different sizes
}

void test_matrix_mult_vector()
{
  Matrix A(3, 2);
  Vector u(2), v(3);

  A(0,0) = 0; A(0,1) = 2;
  A(1,0) = 2; A(1,1) = 3;
  A(2,0) = 4; A(2,1) = 1;

  u(0) = 5;
  u(1) = 2;

  v(0) = 0*5 + 2*2;
  v(1) = 2*5 + 3*2;
  v(2) = 4*5 + 1*2;

  assert(approx_eq(A*u, v, 10));
}

void test_matrix_resize()
{
  Matrix A(3, 3), B;

  for (size_t j=0; j<A.cols(); ++j)
    for (size_t i=0; i<A.rows(); ++i)
      A(i, j) = Random::getReal();

  B = A;

  assert(A.cols() == 3 && A.rows() == 3);
  A.resize(4, 4);
  assert(A.cols() == 4 && A.rows() == 4);

  // Check that elements in 3x3 does not changed
  for (size_t j=0; j<B.cols(); ++j)
    for (size_t i=0; i<B.rows(); ++i) {
      assert(approx_eq(A(i, j), B(i, j), 10));
    }

  // Check that new rows and columns are zero
  Vector zero(4);
  zero.zero();

  assert(approx_eq(zero, A.getCol(3), 10));
  assert(approx_eq(zero, A.getRow(3), 10));

  // Make A of 2x2
  A.resize(2, 2);
  for (size_t j=0; j<A.cols(); ++j)
    for (size_t i=0; i<A.rows(); ++i) {
      assert(approx_eq(A(i, j), B(i, j), 10));
    }
}

void test_vector_io()
{
  Vector a(6);
  a(0) = 2;
  a(1) = 3.2;
  a(2) = 50.23;
  a(3) = 768;
  a(4) = 93.0;
  a(5) = 0;

  Vector b;

  stringstream s;
  s << a;
  s >> b;

  assert(approx_eq(a, b, 10));
}

void test_matrix_io()
{
  Matrix A(2, 2);
  A(0,0) = 432.2;
  A(0,1) = 43254.2;
  A(1,0) = 43.3;
  A(1,1) = 342.33;

  Matrix B;

  stringstream s;
  s << A;
  s >> B;

  assert(approx_eq(A, B, 10));
}

int main(int argc, char *argv[])
{
  for (int i=0; i<10; ++i) {
    Random::init(i);
    test_vector_index();
    test_vector_equal();
    test_matrix_mult_vector();
    test_matrix_resize();
    test_vector_io();
    test_matrix_io();
  }
  return 0;
}
