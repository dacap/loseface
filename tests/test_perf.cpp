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

#include <iostream>
#include <algorithm>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#include "Ann.h"
#include "Chrono.h"

using namespace std;

void train_until_mse_is_reached(Mlp& net, Backpropagation& bp, PatternSet& set,
				double target_mse)
{
  int i = 0;
  while (net.calcMSE(set) >= target_mse) {
    random_shuffle(set.begin(), set.end());
    bp.train(set);
  }
}

void test_logical_op(int a_x0, int a_x1, int a_y0,
		     int b_x0, int b_x1, int b_y0,
		     int c_x0, int c_x1, int c_y0,
		     int d_x0, int d_x1, int d_y0)
{
  Mlp net(2, 3, 1);
  net.initRandom(0.0, 1.0);

  Backpropagation bp(net);
  bp.setLearningRate(0.25);
  bp.setMomentum(0.9);

  PatternSet set;
  for (int c=0; c<4; c++) {
    Pattern pat(net.createInput(),
		net.createOutput());
    set.push_back(pat);
  }

  set[0].setInput(0, a_x0);
  set[0].setInput(1, a_x1);
  set[0].setOutput(0, a_y0);

  set[1].setInput(0, b_x0);
  set[1].setInput(1, b_x1);
  set[1].setOutput(0, b_y0);

  set[2].setInput(0, c_x0);
  set[2].setInput(1, c_x1);
  set[2].setOutput(0, c_y0);

  set[3].setInput(0, d_x0);
  set[3].setInput(1, d_x1);
  set[3].setOutput(0, d_y0);

  train_until_mse_is_reached(net, bp, set, 0.001);
}

int main()
{
  cout << "Testing speed to solve XOR problem with MSE < 0.001..." << endl;

  int times = 1000;
  double total = 0.0;
  for (int c=0; c<times; ++c) {
    srand(0);

    Chrono chrono;
    test_logical_op(0,0,0, 0,1,1, 1,0,1, 1,1,0);
    total += chrono.elapsed();
  }
  cout << "  Average: " << (total/times) << " milliseconds (based on " << times << " runs)\n";
  return 0;
}
