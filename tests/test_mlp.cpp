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

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#define VECTOR_WITH_IO
#define SAVE_DATA 1

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#include "Ann.h"
#include "Chrono.h"

template<class Net, class Bp>
void train_until_mse_is_reached(Net& net, Bp& bp, typename Bp::PatternSet& set,
				double target_mse, int show_epochs)
{
  int i = 0;
  while (net.calcMSE(set) >= target_mse) {
    std::random_shuffle(set.begin(), set.end());
    bp.train(set);
    if (show_epochs > 0 && !((i++)%show_epochs))
      std::cout << "MSE=" << net.calcMSE(set) << std::endl;
  }
  std::cout << "MSE=" << net.calcMSE(set) << std::endl;
}

template<class Net, class Bp>
void train_net_adapting_learning_rate(Net& net, Bp& bp, typename Net::PatternSet& set)
{
  int i = 1;
  double lastE, E = net.calcMSE(set);
  Net lastNet;

  while (E > 0.1) {
    std::random_shuffle(set.begin(), set.end());

    lastE = E;
    lastNet = net;

    bp.train(set);

    E = net.calcMSE(set);
    while (E > lastE) {
      bp.setLearningRate(bp.getLearningRate()/2.0);
      net = lastNet;
      bp.train(set);
      E = net.calcMSE(set);

      if (!((i++)%1000))
	std::cout << "MSE=" << net.calcMSE(set)
		  << " LR=" << bp.getLearningRate() << std::endl;
    }
    bp.setLearningRate(bp.getLearningRate()*2.0);
  }
}

template<typename T>
void normalize_inputs(PatternSet<T>& set,
		      T min_value,
		      T max_value,
		      Vector<T>& min_vector,
		      Vector<T>& max_vector)
{
  if (!set.empty()) {
    min_vector = set[0].input;
    max_vector = set[0].input;

    for (size_t c=1; c<set.size(); c++) {
      for (size_t i=0; i<set[c].input.size(); i++) {
	if (min_vector(i) > set[c].input(i)) min_vector(i) = set[c].input(i);
	if (max_vector(i) < set[c].input(i)) max_vector(i) = set[c].input(i);
      }
    }

    for (size_t c=0; c<set.size(); c++)
      for (size_t i=0; i<set[c].input.size(); i++) {
	set[c].input(i) =
	  min_value +
	  (max_value - min_value) *
	  (set[c].input(i) - min_vector(i)) /
	  (  max_vector(i) - min_vector(i));
      }
  }
}

void test_logical_op(const std::string& name,
		     int a_x0, int a_x1, int a_y0,
		     int b_x0, int b_x1, int b_y0,
		     int c_x0, int c_x1, int c_y0,
		     int d_x0, int d_x1, int d_y0)
{
  typedef Mlp<double> Net;

  std::cout << name << std::endl;

  Net net(2, 3, 1);
  net.initRandom(0.0, 1.0);

  Backpropagation<Net> bp(net);
  bp.setLearningRate(0.25);
  bp.setMomentum(0.9);

  PatternSet<double> set;
  for (int c=0; c<4; c++) {
    Pattern<double> pat;
    pat.input = net.createInput();
    pat.output = net.createOutput();
    set.push_back(pat);
  }

  set[0].input(0) = a_x0;
  set[0].input(1) = a_x1;
  set[0].output(0) = a_y0;

  set[1].input(0) = b_x0;
  set[1].input(1) = b_x1;
  set[1].output(0) = b_y0;

  set[2].input(0) = c_x0;
  set[2].input(1) = c_x1;
  set[2].output(0) = c_y0;

  set[3].input(0) = d_x0;
  set[3].input(1) = d_x1;
  set[3].output(0) = d_y0;

  train_until_mse_is_reached(net, bp, set, 0.001, 10000);

  for (size_t p=0; p<set.size(); p++) {
    Vector<double> hidden, output;
    net.recall(set[p].input, hidden, output);

    std::cout << "  " << set[p].input
	      << " => " << output << std::endl;
  }

#if SAVE_DATA
  {
    std::ofstream f((name + ".dat").c_str());
    for (double x=0.0; x<=1.0; x+=0.01) {
      for (double y=0.0; y<=1.0; y+=0.01) {
	Vector<double> input(2);
	Vector<double> hidden, output;
	input(0) = x;
	input(1) = y;
	net.recall(input, hidden, output);
	f << input(0) << "\t" << input(1) << "\t" << output(0) << "\n";
      }
    }
  }
#endif
}

void test_grid()
{
  typedef Mlp<double> Net;

  std::cout << "grid" << std::endl;

  Net net(2, 16, 1);
  net.initRandom(0.0, 1.0);

  Backpropagation<Net> bp(net);
  bp.setLearningRate(0.1);
  bp.setMomentum(0.8);

  PatternSet<double> set;
  for (int y=0; y<=2; ++y) {
    for (int x=0; x<=2; ++x) {
      Pattern<double> pat;
      pat.input = net.createInput();
      pat.output = net.createOutput();
      pat.input(0) = x / 2.0;
      pat.input(1) = y / 2.0;
      pat.output(0) = ((x>y?x-y:y-x)&1) ? 0.0: 1.0;
      set.push_back(pat);
    }
  }

  train_until_mse_is_reached(net, bp, set, 0.01, 1000);
  
  for (size_t p=0; p<set.size(); p++) {
    Vector<double> hidden, output;
    net.recall(set[p].input, hidden, output);

    std::cout << "  " << set[p].input
	      << " => " << output << std::endl;
  }

#if SAVE_DATA
  {
    std::ofstream f("grid.dat");
    for (double x=0.0; x<=1.0; x+=0.01) {
      for (double y=0.0; y<=1.0; y+=0.01) {
	Vector<double> input(2);
	Vector<double> hidden, output;
	input(0) = x;
	input(1) = y;
	net.recall(input, hidden, output);
	f << input(0) << "\t" << input(1) << "\t" << output(0) << "\n";
      }
    }
  }
#endif
}

void test_sin()
{
  typedef Mlp<double, Tansig<double> > Net;

  std::cout << "sin(x)" << std::endl;

  Net net(1, 10, 1);

  net.initRandom(-1.0, 1.0);

  Backpropagation<Net> bp(net);
  bp.setLearningRate(0.05);
  bp.setMomentum(0.1);

  PatternSet<double> set;
  for (int c=0; c<=100; c++) {
    Pattern<double> pat;
    pat.input = net.createInput();
    pat.output = net.createOutput();

    double x = (M_PI*8) * static_cast<double>(c) / 100.0 - M_PI*4;
    pat.input(0) = x;
    pat.output(0) = std::sin(x);
    set.push_back(pat);
  }

  train_until_mse_is_reached(net, bp, set, 0.01, 100);

#if SAVE_DATA
  {
    std::ofstream f("sin.dat");
    for (int c=-2000; c<4000; ++c) {
      Vector<double> input(1);
      Vector<double> hidden, output;
      input(0) = (M_PI*2) * static_cast<double>(c) / 1000.0 - M_PI;
      net.recall(input, hidden, output);
      f << input(0) << "\t" << output(0) << "\n";
    }
  }
#endif
}

void test_iris()
{
  typedef Mlp<double> Net;

  Net net(4, 3, 3);

  net.initRandom(-1.0, 1.0);

  // StdBp<Net, AccumulateWeightsWithMomentum> bp(net);
  // StdBp<Net, UpdateWeightsImmediatelyWithMomentum, OutputTolerance> bp(net);
  Backpropagation<Net> bp(net);
  bp.setLearningRate(0.2);
  bp.setMomentum(0.1);

  std::ifstream f("iris.csv");
  PatternSet<double> set;
  char buf[256];

  while (f.getline(buf, sizeof(buf)).good()) {
    std::istringstream str(buf);

    Pattern<double> pat;
    pat.input = net.createInput();
    pat.output = net.createOutput();
    char comma;

    for (int c=0; c<4; ++c)
      str >> pat.input(c) >> comma;

    int output;
    str >> output;

    for (int c=0; c<3; ++c)
      pat.output(c) = (c == output) ? 1.0: 0.0;

    set.push_back(pat);
  }

  // std::cout << "Patterns: " << set.size() << std::endl << std::endl;
  // for (int p=0; p<set.size(); ++p)
  //   std::cout << "Pattern["<<p<<"]: " << set[p].input << " => " << set[p].output << std::endl;

  // normalize patterns
  Vector<double> min_vector;
  Vector<double> max_vector;
  normalize_inputs(set, 0.0, 1.0, min_vector, max_vector);

  // go to training
  int max = 0;
  int bestEpoch = 0;
  Net theNet;
  for (int c=0; c<1000; ++c) {
    std::random_shuffle(set.begin(), set.end());
    bp.train(set);

    // performance
    int good = 0;
    for (size_t p=0; p<set.size(); ++p) {
      Vector<double> hidden, output;
      net.recall(set[p].input, hidden, output);

      if (output.getMaxPos() == set[p].output.getMaxPos())
	++good;
    }
    if (max < good) {
      max = good;
      bestEpoch = c;
      theNet = net;
    }
    // std::cout << "Epoch=" << c << " Hits=" << ((double)good / (double)set.size()) << " MSE=" << net.calcMSE(set) << std::endl;
  }
  std::cout << "Best Epoch=" << bestEpoch << " Hits=" << ((double)max / (double)set.size()) << std::endl;

  Matrix<int> distribution(3, 3);
  distribution.zero();
  for (size_t p=0; p<set.size(); ++p) {
    Vector<double> hidden, output;
    theNet.recall(set[p].input, hidden, output);
    ++distribution(set[p].output.getMaxPos(), output.getMaxPos());
  }
  std::cout << "class |  0 |  1 |  2 |" << std::endl
	    << "------+----+----+----+" << std::endl;
  for (int i=0; i<3; ++i) {
    std::printf("   %2d | %2d | %2d | %2d |\n", i,
		distribution(i, 0),
		distribution(i, 1),
		distribution(i, 2));
  }
  std::cout << "------+----+----+----+" << std::endl;
}

int main()
{
  srand(1); test_logical_op("AND", 0,0,0, 0,1,0, 1,0,0, 1,1,1);
  srand(1); test_logical_op("OR",  0,0,0, 0,1,1, 1,0,1, 1,1,1);
  srand(1); test_logical_op("XOR", 0,0,0, 0,1,1, 1,0,1, 1,1,0);
  srand(1); test_grid();
  srand(1); test_sin(); // chrono_func(test_sin, "sin(x)");
  srand(1); test_iris();
  // for (int s=0; s<10; ++s) {
  //   std::cout << "Seed " << s << std::endl;
  //   srand(s);
  //   test_iris();
  // }
  return 0;
}
