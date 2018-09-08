#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
  static Timer *New();
  
  virtual void Reset() = 0;
  virtual double GetElapsedTime() = 0;
  
  virtual ~Timer() {}
};

#endif
