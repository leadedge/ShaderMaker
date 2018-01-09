#ifndef FFGLMirror_H
#define FFGLMirror_H

#include "../FFGLPluginSDK.h"

class FFGLMirror :
public CFreeFrameGLPlugin
{
public:
	FFGLMirror();
  virtual ~FFGLMirror() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLMirror();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }
};


#endif
