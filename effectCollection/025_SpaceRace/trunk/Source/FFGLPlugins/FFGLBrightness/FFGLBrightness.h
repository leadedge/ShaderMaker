#ifndef FFGLBRIGHTNESS_H
#define FFGLBRIGHTNESS_H

#include "../FFGLPluginSDK.h"

class FFGLBrightness :
public CFreeFrameGLPlugin
{
public:
	FFGLBrightness();
  virtual ~FFGLBrightness();

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppInstance)
  {
  	*ppInstance = new FFGLBrightness();
	  if (*ppInstance != NULL) return FF_SUCCESS;
	  return FF_FAIL;
  }

protected:	
	// Parameters
	float m_brightness;
};


#endif
