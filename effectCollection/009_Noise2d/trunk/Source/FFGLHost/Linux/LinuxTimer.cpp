#include <stdio.h>
#include <sys/time.h>
#include "../Timer.h"

class LinuxTimer :
public Timer
{
public:
  double start_time;

  LinuxTimer()
  {
    Reset();
  }

  void Reset()
  {
    timeval tv;
    gettimeofday(&tv, NULL);
    start_time = tv.tv_sec + tv.tv_usec/1000000.0;
  }

  double GetElapsedTime()
  {
    timeval tv;
    gettimeofday(&tv, NULL);
    double curtime = tv.tv_sec + tv.tv_usec/1000000.0;
    return (curtime - start_time);
  }

  virtual ~LinuxTimer()
  {}
};

Timer *Timer::New()
{
  return new LinuxTimer();
}
