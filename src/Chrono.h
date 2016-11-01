// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_CHRONO_H
#define LOSEFACE_CHRONO_H

//////////////////////////////////////////////////////////////////////
//  For Windows

#ifdef _WIN32

  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

  class Chrono
  {
    LARGE_INTEGER m_point;
    LARGE_INTEGER m_freq;

  public:

    Chrono() {
      QueryPerformanceFrequency(&m_freq);
      reset();
    }

    void reset() {
      QueryPerformanceCounter(&m_point);
    }

    double elapsed() const {
      LARGE_INTEGER now;
      QueryPerformanceCounter(&now);
      return static_cast<double>(now.QuadPart - m_point.QuadPart)
	/ static_cast<double>(m_freq.QuadPart);
    }

  };

#else  // For UNIX like

  #include <time.h>
  #include <sys/time.h>

  class Chrono
  {
    struct timeval m_point;

  public:

    Chrono() {
      reset();
    }

    void reset() {
      gettimeofday(&m_point, NULL);
    }

    double elapsed() const {
      struct timeval now;
      gettimeofday(&now, NULL);
      return
	(double)(now.tv_sec + (double)now.tv_usec/1000000) -
	(double)(m_point.tv_sec + (double)m_point.tv_usec/1000000);
    }

  };

#endif

//////////////////////////////////////////////////////////////////////

template<class F>
double chrono_func(const F& f, const std::string& name, int times = 10)
{
  double last, accum = 0.0;
  for (int c=0; c<times; ++c) {
    srand(c);
    {
      Chrono chrono;
      f();
      last = chrono.elapsed();
    }
    accum += last;
    //std::cout << "Chrono " << name << " in " << last << " secs\n";
  }
  //std::cout << "Avg " << name << " in " << (accum/times) << " secs\n";
  return accum / times;
}

#endif // LOSEFACE_CHRONO_H
