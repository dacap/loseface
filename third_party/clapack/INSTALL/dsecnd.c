#include "blaswrap.h"
#include "f2c.h"
#ifdef LINUX
#include <sys/times.h>
#include <sys/types.h>
#endif
#include <time.h>

#ifndef CLK_TCK
#define CLK_TCK 60
#endif

doublereal dsecnd_()
{
#ifdef LINUX
  struct tms rusage;

  times(&rusage);
  return (doublereal)(rusage.tms_utime) / CLK_TCK;
#else
  return (doublereal)1.0;
#endif

} /* dsecnd_ */

