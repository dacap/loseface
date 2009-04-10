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

#include "Matrix.h"
#include "Vector.h"

static void test_vector_mean()
{
  Vector<double> v(4);
  v(0) = 10.0;
  v(1) = 3.0;
  v(2) = -4.0;
  v(3) = 30.0;
  assert(v.mean() == (10.0 + 3.0 - 4.0 + 30.0) / 4.0);
}

static void test_matrix_mean()
{
  Matrix<double> A(2, 3);

  A(0,0) = 1.0;  A(0,1) = 2.4;  A(0,2) = 0.2;
  A(1,0) = 1.4;  A(1,1) = 4.4;  A(1,2) = 0.1;

  Vector<double> u(3);
  u(0) = (1.0 + 1.4) / 2.0;
  u(1) = (2.4 + 4.4) / 2.0;
  u(2) = (0.2 + 0.1) / 2.0;
  assert(A.meanRow() == u);

  Vector<double> v(2);
  v(0) = (1.0 + 2.4 + 0.2) / 3.0;
  v(1) = (1.4 + 4.4 + 0.1) / 3.0;
  assert(A.meanCol() == v);
}

int main(int argc, char *argv[])
{
  test_vector_mean();
  test_matrix_mean();
  return 0;
}
