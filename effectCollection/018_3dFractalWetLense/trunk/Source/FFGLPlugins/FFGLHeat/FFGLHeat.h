#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"
 
struct Point2D{
	float x;
	float y;
};

struct Point3D{
	float x;
	float y;
	float z;
};
struct ColorData{
	float hue;
	float strength;
};
struct AxisAngle{
	float x;
	float y;
	float z;
	float scalex;
	float scaley;
	float scalez;
	float angle;
};

struct Anim{
	float countx;
		float speedx;
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
	
	Anim anim1; 
	 
	float m_blend; 

	int m_initResources; 
	 

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation1;
	GLint m_inputTextureLocation2; 
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation;  
	GLint m_blendLocation;  
	   
	GLint m_resolutionLocation; 
};


#endif
