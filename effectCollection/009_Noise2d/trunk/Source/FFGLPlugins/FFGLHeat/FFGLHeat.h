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
double systemTimeToDouble(SYSTEMTIME *time);
protected:	
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;   
	
	NoiseAnim seedAnim1; 
	
	int m_initResources;
	float m_hueScale;
	float m_satScale;
	float m_valueScale;
	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_locationLocation; 
	GLint m_hueScaleLocation; 
	  

	
	GLint m_seed1Location; 
	GLint m_persistenceLocation; 

};


#endif
