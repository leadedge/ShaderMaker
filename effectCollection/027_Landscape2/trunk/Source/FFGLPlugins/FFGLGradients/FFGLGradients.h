#ifndef FFGLGradients_H
#define FFGLGradients_H

#include "../FFGLPluginSDK.h"
#include "FFGLExtensions.h"


class FFGLGradients : public CFreeFrameGLPlugin
{
public:
	FFGLGradients();
  ~FFGLGradients() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
	DWORD   InitGL(const FFGLViewportStruct *vp);
	DWORD   DeInitGL();

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLGradients();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }


protected:	

	float m_Hue1;
	float m_Hue2;
	float m_Saturation;
	float m_Brightness;
	
};

void HSVtoRGB(double h, double s, double v, double* r, double* g, double* b);

#endif
