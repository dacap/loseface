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

#ifndef LOSEFACE_RANDOM_H
#define LOSEFACE_RANDOM_H

extern "C" {

extern void init_genrand(unsigned long s);
extern unsigned long genrand_int32(void);
extern long genrand_int31(void);
extern double genrand_real1(void);

}

/// Mersenne Twister random number generator wrapper.
///
/// This is a wrapper class for MT19937 coded by Takuji Nishimura and
/// Makoto Matsumoto.
class Random
{
public:

  /// Initializes the generator with a seed.
  inline static void init(unsigned long seed) {
    init_genrand(seed);
  }

  /// Generates a random number on [0,0xffffffff]-interval.
  static unsigned long getUInt() {
    return genrand_int32();
  }

  /// Generates a random number on [0,0x7fffffff]-interval.
  static long getInt() {
    return genrand_int31();
  }

  /// Generates a random number on [0,1]-real-interval.
  static double getReal() {
#if 0
    return genrand_real1();
#else
    return static_cast<double>(std::rand() % 10001) / 10000.0;
#endif
  }

};

#endif // LOSEFACE_RANDOM_H
