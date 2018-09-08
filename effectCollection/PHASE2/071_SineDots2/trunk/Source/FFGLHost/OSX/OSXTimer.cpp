#include "../Timer.h"
#include <mach/mach.h>
#include <mach/mach_time.h>

class OSXTimer :
public Timer
{
public:

  OSXTimer()
  :m_startTime(0)
  {
    //make sure we query the timebase before doing any timing
    if (g_firstTime==0)
    {
      g_firstTime = 1;
      
      mach_timebase_info_data_t timebase;
      
      mach_timebase_info(&timebase);
      
      g_conversion = (1.0 / 1000000000.0) * ((double)timebase.numer / (double)timebase.denom );
    }
    
    //start the timer now
    m_startTime = mach_absolute_time();
  }

  static int g_firstTime;
  static double g_conversion;
  
  uint64_t m_startTime;
  
  void Reset()
  {
    //restart the timer now
    m_startTime = mach_absolute_time();
  }
  
  double GetElapsedTime()
  {
    //whats the current time?
    uint64_t curTime = mach_absolute_time();

    uint64_t elapsed = curTime - m_startTime;
    
    return g_conversion * (double)elapsed;
  }
  
  virtual ~OSXTimer()
  {}
};

int OSXTimer::g_firstTime = 0;
double OSXTimer::g_conversion = 0.0;

Timer *Timer::New()
{
  return new OSXTimer();
}
