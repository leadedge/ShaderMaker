#include "../Timer.h"
#include <windows.h>

class WinTimer :
public Timer
{
public:
  DWORD start_time;

  WinTimer()
  {
    Reset();
  }

  void Reset()
  {
    start_time = timeGetTime();
  }

  double GetElapsedTime()
  {
    DWORD systime = timeGetTime();

    //timeGetTime is in milliseconds, convert to seconds
    return ((double)(systime - start_time))/1000.0;
  }
  
  virtual ~WinTimer()
  {
  }
};

Timer *Timer::New()
{
  return new WinTimer();
}