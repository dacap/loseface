// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
