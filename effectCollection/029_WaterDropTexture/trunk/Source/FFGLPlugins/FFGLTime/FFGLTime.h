#ifndef FFGLTime_H
#define FFGLTime_H

#include "../FFGLPluginSDK.h"

class FFGLTime :
public CFreeFrameGLPlugin
{
public:
	FFGLTime();
  virtual ~FFGLTime();

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////

	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
  DWORD SetTime(double time);

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppInstance)
  {
	*ppInstance = new FFGLTime();
	  if (*ppInstance != NULL) return FF_SUCCESS;
	  return FF_FAIL;
  }

protected:

  void Draw0();
  void Draw1();
  void Draw2();
  void Draw3();
  void Draw4();
  void Draw5();
  void Draw6();
  void Draw7();
  void Draw8();
  void Draw9();
  void DrawDecimal();

  // Time
  double m_curTime;
};


#endif

