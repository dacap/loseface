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

#ifndef LOSEFACE_CHRONO_H
#define LOSEFACE_CHRONO_H

//////////////////////////////////////////////////////////////////////
//  For Windows

#ifdef _WIN32

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

template<typename F>
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
