#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"
 
struct Vector2D{
	float x;
	float y;
};
struct Vector3D{
	float x;
	float y;
	float z;
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
	 
	 
	Vector3D color1;
	Vector3D color2;
	float s_seaHeight;
	float s_seaFrequency;
	float s_seaChoppy;
	float s_skyIntensity;
	Anim seaAnim;

	int m_initResources;
	Vector3D hsvScale;
	Vector3D cameraRotation;
	Vector2D center; 

	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_locationLocation; 
	  

	
	GLint m_bokehRadiusLocation; 
	GLint m_bokehAmountLocation; 
	GLint m_resolutionLocation;
	GLint m_speedLocation;
	GLint m_circleWidthsLocation;
	GLint m_hsvScaleLocation; 
	GLint m_centerLocation;


	
	GLint m_waterBaseLocation;
	GLint m_waterColorLocation;
	GLint m_seaHeightLocation;
	GLint m_seaChoppyLocation;
	GLint m_seaFrequencyLocation;
	GLint m_cameraRotationLocation;
	GLint m_seaTimeLocation;
	GLint m_skyIntensityLocation;
};


#endif
