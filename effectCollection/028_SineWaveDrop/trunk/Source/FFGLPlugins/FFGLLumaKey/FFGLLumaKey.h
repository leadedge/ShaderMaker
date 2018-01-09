#ifndef FFGLLumaKey_H
#define FFGLLumaKey_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"

class FFGLLumaKey :
public CFreeFrameGLPlugin
{
public:
  FFGLLumaKey();
  virtual ~FFGLLumaKey() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////

	DWORD	SetParameter(const SetParameterStruct* pParam);
	DWORD	GetParameter(DWORD dwIndex);
	DWORD	ProcessOpenGL(ProcessOpenGLStruct *pGL);
  DWORD InitGL(const FFGLViewportStruct *vp);
  DWORD DeInitGL();


	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
    *ppOutInstance = new FFGLLumaKey();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }

protected:
	// Parameters
	float m_Luma;

	int m_initResources;
	FFGLExtensions m_extensions;
	FFGLShader m_shader;
	GLint m_inputTextureLocation1;
	GLint m_inputTextureLocation2;
	GLint m_maxCoordsLocation;
	GLint m_LumaLocation;
};


#endif
