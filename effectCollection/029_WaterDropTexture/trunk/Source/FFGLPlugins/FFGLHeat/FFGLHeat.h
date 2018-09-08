#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"
 
struct Point2D{
	float x;
	float y;
};

struct NoiseAnim{
	float countx;
	float countz;
	float angle;
	float scale;
	float persistence;
		float speedx;
		float speedz;
};

class FFGLSineWave2D :


public CFreeFrameGLPlugin
{
public:
	FFGLSineWave2D();
  virtual ~FFGLSineWave2D() {}

	///////////////////////////////////////////////////
	// FreeFrameGL plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
  DWORD InitGL(const FFGLViewportStruct *vp);
  DWORD DeInitGL();
  	DWORD SetTime(double time);
	

double GetTimeInSecondsSinceStart();
	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLSineWave2D();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }
	
void updateRenderTargets(int newWidth,int newHeight);
double systemTimeToDouble(SYSTEMTIME *time);
protected:	
	int lastWidth;
	int lastHeight;
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;   
	
	NoiseAnim seedAnim1; 
	
	float m_Amount;
	float m_Radius;

	int m_initResources;

	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
    FFGLShader m_shaderSimple;
	GLint m_lastframeTextureLocation;
	GLint m_inputTextureLocation;
	GLint m_inputTextureLocationSimple; 
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_locationLocation; 
	  

	
	GLint m_bokehRadiusLocation; 
	GLint m_bokehAmountLocation; 
	GLint m_resolutionLocation;

	GLuint m_FramebufferId;
  GLuint renderedTexture1;
  GLuint renderedTexture2; 

  int frameindexforswapping;

};


#endif
